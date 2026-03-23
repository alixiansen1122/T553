/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_verify.h"
#include "app_verify_pub.h"
#include "verify_util_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//#ifdef LITE_WEARABLE
//#include "ohos_fs.h"
#include "verify_util_log.h"
//#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//#endif
#include "securec.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/base64.h"
#include "mbedtls/version.h"
#include "mbedtls_pkcs7.h"
#include "app_common.h"
#include "app_provision.h"

static char *g_verifyFileName = NULL;
static MessageFunc g_messageFunc = NULL;

static const TrustAppCert g_trustAppList[] = {
    {
        .maxCertPath = CERT_MAX_DEPTH,
        .name = "huawei app gallary",
        .appSignCert = "C=CN, O=Huawei, OU=HOS AppGallery, CN=HOS AppGallery Application Release",
        .profileSignCert = "C=CN, O=Huawei, OU=HOS AppGallery, CN=HOS Profile Management",
        .profileDebugSignCert = "C=CN, O=Huawei, OU=HOS AppGallery, CN=HOS Profile Management Debug",
        .issueCA = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA",
    },
    {
        CERT_MAX_DEPTH,
        "huawei system apps",
        "C=CN, O=Huawei CBG, OU=HOS Development Team, CN=HOS Application Provision Release",
        "C=CN, O=Huawei CBG, OU=HOS Development Team, CN=HOS Application Provision Profile Release",
        "C=CN, O=Huawei CBG, OU=HOS Development Team, CN=HOS Application Provision Profile Release_Debug",
        "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA",
    },
    {
        .maxCertPath = CERT_MAX_DEPTH,
        .name = "OpenHarmony apps",
        .appSignCert = "C=CN, O=OpenHarmony, OU=OpenHarmony Team, CN=OpenHarmony Application Release",
        .profileSignCert = "C=CN, O=OpenHarmony, OU=OpenHarmony Team, CN=OpenHarmony Application Profile Release",
        .profileDebugSignCert = "C=CN, O=OpenHarmony, OU=OpenHarmony Team, CN=OpenHarmony Application Profile Debug",
        .issueCA = "C=CN, O=OpenHarmony, OU=OpenHarmony Team, CN=OpenHarmony Application CA",
    }
};

static const TrustAppCert g_trustAppListTest[] = {
    {
        .maxCertPath = CERT_MAX_DEPTH,
        .name = "huawei app gallary",
        .appSignCert = "C=CN, O=Huawei, OU=HOS AppGallery, CN=HOS AppGallery Application Release",
        .profileSignCert = "C=CN, O=Huawei, OU=HOS AppGallery, CN=HOS Profile Management",
        .profileDebugSignCert = "C=CN, O=Huawei, OU=HOS AppGallery, CN=HOS Profile Management Debug",
        .issueCA = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA Test",
    },
    {
        CERT_MAX_DEPTH,
        "huawei system apps",
        "C=CN, O=Huawei CBG, OU=HOS Development Team, CN=HOS Application Provision Dev",
        "C=CN, O=Huawei CBG, OU=HOS Development Team, CN=HOS Application Provision Profile Dev",
        "C=CN, O=Huawei CBG, OU=HOS Development Team, CN=HOS Application Provision Profile Dev_Debug",
        "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA Test",
    },
};

static bool g_isDebugMode = false;

static void SendProgressMessage(unsigned char operationResult)
{
    if (g_messageFunc == NULL || g_verifyFileName == NULL) {
        return;
    }
    (void)g_messageFunc(operationResult, g_verifyFileName, 0);
}

#define VERIFY_BUFFER_LEN (4 * 1024)
static int VerifyRead(const int handle, void *buf, const unsigned int len)
{
    int readSize = 0;
    unsigned char *readBuf = (unsigned char *)buf;
    unsigned int totalLen = len;
    while (totalLen > 0) {
        int onceReadLen = (totalLen >= VERIFY_BUFFER_LEN) ? VERIFY_BUFFER_LEN : totalLen;
        int ret = read(handle, (void *)readBuf, onceReadLen);
        if (ret != onceReadLen) {
            return ret;
        }
        totalLen -= onceReadLen;
        readSize += onceReadLen;
        readBuf += onceReadLen;
    }
    return readSize;
}
static int GetInt(unsigned char *buf, int len)
{
    int value = 0;
    if (len != sizeof(int)) {
        return 0;
    }
    while (len-- > 0) {
        value = (((unsigned int)value) << BYTE_BITS) | (*buf);
        buf++;
    }
    return value;
}

static short GetShort(unsigned char *buf, int len)
{
    short value = 0;
    if (len != sizeof(short)) {
        return 0;
    }
    while (len-- > 0) {
        value = (((unsigned short)value) << BYTE_BITS) | *buf;
        buf++;
    }
    return value;
}

static void SignHeadN2H(HwSignHead *signHead)
{
    signHead->blockNum = GetInt((unsigned char *)&signHead->blockNum, sizeof(signHead->blockNum));
    signHead->size = GetInt((unsigned char *)&signHead->size, sizeof(signHead->size));
    return;
}

static void BlockHeadN2H(BlockHead *blockHead)
{
    blockHead->length = GetShort((unsigned char *)&blockHead->length, sizeof(blockHead->length));
    blockHead->offset = GetInt((unsigned char *)&blockHead->offset, sizeof(blockHead->offset));
    return;
}

static void ContentN2H(ContentInfo *content)
{
    content->blockNum = GetShort((unsigned char *)&content->blockNum, sizeof(content->blockNum));
    content->size = GetShort((unsigned char *)&content->size, sizeof(content->size));

    content->hashContent.length = GetInt((unsigned char *)&content->hashContent.length,
                                         sizeof(content->hashContent.length));
    content->hashContent.algId = GetShort((unsigned char *)&content->hashContent.algId,
                                          sizeof(content->hashContent.algId));
    return;
}
static int GetSignHead(int fp, HwSignHead **signH)
{
    struct stat fileSt;
    int ret = fstat(fp, &fileSt);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "fstat error,  %d", ret);
        return V_ERR_GET_SIGNHEAD;
    }
    if (fileSt.st_size < sizeof(HwSignHead)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "file error: filelen: %d, signhead: %d",
                    (int)fileSt.st_size, (int)sizeof(HwSignHead));
        return V_ERR_FILE_LENGTH;
    }
    ret = lseek(fp, fileSt.st_size - sizeof(HwSignHead), SEEK_SET);
    if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "lseek error,  %d", ret);
        return V_ERR_GET_SIGNHEAD;
    }
    HwSignHead *signHead = APPV_MALLOC(sizeof(HwSignHead));
    if (signHead == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return V_ERR_MALLOC;
    }
    int readLen = VerifyRead(fp, signHead, sizeof(HwSignHead));
    if (readLen != sizeof(HwSignHead)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "%d --- %d", readLen, (int)sizeof(HwSignHead));
        APPV_FREE(signHead);
        return V_ERR_GET_SIGNHEAD;
    }
    SignHeadN2H(signHead);
    if (memcmp("hw signed app   ", signHead->magic, HW_SH_MAGIC_LEN)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "sign head magic invalid");
        APPV_FREE(signHead);
        return V_ERR_GET_SIGNHEAD;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "sign head: size: 0x%x, blockNum:0x%x",
               signHead->size, signHead->blockNum);
    *signH = signHead;
    return V_OK;
}

static int FindBlockHead(const HwSignHead *signH, int fp, int blockType, BlockHead *block)
{
    /* find signature block */
    int signLen = signH->size;

    /* begin of sign */
    struct stat *fileSt = APPV_MALLOC(sizeof(struct stat));
    P_NULL_RETURN_WTTH_LOG(fileSt);
    (void)memset_s(fileSt, sizeof(struct stat), 0, sizeof(struct stat));
    int ret = fstat(fp, fileSt);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "fstat error: %d", ret);
        APPV_FREE(fileSt);
        return V_ERR;
    }

    lseek(fp, fileSt->st_size - signLen, SEEK_SET);
    APPV_FREE(fileSt);

    int num = signH->blockNum;
    while (num-- > 0) {
        int readLen = VerifyRead(fp, block, sizeof(BlockHead));
        if (readLen != sizeof(BlockHead)) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "find block head , read err %d --- %d",
                        readLen, (int)sizeof(BlockHead));
            return V_ERR;
        }
        if (block->type == blockType) {
            BlockHeadN2H(block);
            return V_OK;
        }
    }
    HILOG_ERROR(HILOG_MODULE_AAFWK, "get sign block by type failed, type: %d", blockType);
    return V_ERR;
}

static void FreeBlockByType(int blockType, char **buf)
{
    if (blockType == PROFILE_BLOCK_WITHSIGN_TYPE) {
        APPV_PSMEM_FREE(*buf);
        return;
    }
    APPV_FREE(*buf);
}

static char *GetSignBlockByType(HwSignHead *signH, int fp, int blockType, int *len, BlockHead *blockHead)
{
    int ret = FindBlockHead(signH, fp, blockType, blockHead);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "find block head error");
        return NULL;
    }
    struct stat *fileSt = APPV_MALLOC(sizeof(struct stat));
    P_NULL_RETURN_NULL_WTTH_LOG(fileSt);
    ret = fstat(fp, fileSt);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "fstat error: %d", ret);
        APPV_FREE(fileSt);
        return NULL;
    }
    lseek(fp, 0, SEEK_END);
    int fileLen = fileSt->st_size;
    APPV_FREE(fileSt);
    /* sign block head length always 0
        rawdata
        xx block head
        signdata
        hwsignhead
    */
    if ((unsigned int)fileLen < blockHead->offset ||
        (((unsigned int)fileLen - blockHead->offset) < sizeof(HwSignHead)) ||
        ((fileLen - blockHead->offset - sizeof(HwSignHead)) > MAX_BLOCK_LENGTH)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "file len: %d, type: %d, offset: %d",
                    fileLen, blockHead->type, blockHead->offset);
        return NULL;
    }

    /* last block */
    if (blockType == SIGNATURE_BLOCK_TYPE) {
        blockHead->length = fileLen - blockHead->offset - sizeof(HwSignHead);
    }

    if (blockHead->length > fileLen - blockHead->offset - sizeof(HwSignHead)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "file len: %d, type: %d, block offset: %d, len: %d",
                    fileLen, blockHead->type, blockHead->offset, blockHead->length);
        return NULL;
    }
    char *buf = (blockType == PROFILE_BLOCK_WITHSIGN_TYPE) ? APPV_PSMEM_MALLOC(blockHead->length) :
        APPV_MALLOC(blockHead->length);
    if (buf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return NULL;
    }
    lseek(fp, blockHead->offset, SEEK_SET);

    int readLen = VerifyRead(fp, buf, sizeof(char) * blockHead->length);
    if (readLen != blockHead->length) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "read error: %d, %d", readLen, blockHead->length);
        FreeBlockByType(blockType, &buf);
        return NULL;
    }
    *len = readLen;
    return buf;
}

static int GetHashBlockLen(int fileLen, int blockTag)
{
    if (blockTag == DEFAULT_TAG) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "error block tag");
        return V_ERR;
    }
    if (blockTag == HASH_TAG) {
        return fileLen;
    }
    return BUF_1M / (1 << ((unsigned int)blockTag - HASH_ROOT_1M_TAG));
}

static int GetHashUnitLen(int hashAlg)
{
    return mbedtls_md_get_size(mbedtls_md_info_from_type((mbedtls_md_type_t)hashAlg));
}

static int CalculateHash(const unsigned char *input, int len, int hashAlg, unsigned char *output)
{
    int ret = mbedtls_md(mbedtls_md_info_from_type((mbedtls_md_type_t)hashAlg), input, len, output);
    if (ret) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: calc digest failed");
        return ret;
    }
    return ret;
}

static void SendSectionMessage(int readLen, int stepPersent, int *countedLen, int *num)
{
    *countedLen += readLen;
    if (*countedLen >= SECTION_LEN) {
        (*num)++;
        *countedLen = 0;
        int progressPersent = FILE_CERCHAIN_VERIFY + (*num) * stepPersent;
        if (progressPersent > FILE_INTEGRITY_VERIFY) {
            progressPersent = FILE_INTEGRITY_VERIFY;
        }
        SendProgressMessage(progressPersent);
    }
}

static int CacalRootHashByBlockHash(const FileRead *fileRead, int uintLen, int alg, unsigned char *outbuf)
{
    int ret, readLen, rawBufLen, stepPersent;
    unsigned char *totalBlocks = NULL;
    size_t totalBlocksSize = 0;
    size_t blockOffset = 0;
    size_t hlen;
    int countedLen = 0;
    int num = 0;
    uint64_t start, end;
    const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type((mbedtls_md_type_t)alg);
    P_NULL_RETURN_WTTH_LOG(mdInfo);

    if (uintLen < 0 || uintLen > MAX_MALLOC_SIZE) {
        return V_ERR;
    }
    unsigned char *buf = APPV_MALLOC(uintLen);
    P_NULL_RETURN_WTTH_LOG(buf);

    ret = V_ERR;
    mbedtls_md_context_t *md_ctx = APPV_MALLOC(sizeof(mbedtls_md_context_t));
    P_NULL_GOTO_WTTH_LOG(md_ctx);

    mbedtls_md_init(md_ctx);

    ret = mbedtls_md_setup(md_ctx, mdInfo, 0);
    P_ERR_GOTO_WTTH_LOG(ret);

    hlen = mbedtls_md_get_size(mdInfo);

    size_t expectedBlocks = (fileRead->len + uintLen - 1) / uintLen;
    totalBlocks = APPV_MALLOC(expectedBlocks * hlen);
    P_NULL_GOTO_WTTH_LOG(totalBlocks);

    rawBufLen = fileRead->len;
    lseek(fileRead->fp, fileRead->offset, SEEK_SET);

    stepPersent = (FILE_INTEGRITY_VERIFY - FILE_CERCHAIN_VERIFY) / (rawBufLen / SECTION_LEN + 1);
    while (rawBufLen) {
        readLen = (rawBufLen > uintLen) ? uintLen : rawBufLen;
        int len = VerifyRead(fileRead->fp, buf, (unsigned int)(sizeof(char) * readLen));
        if (len != readLen) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "fread err: %d,  %d", len, uintLen);
            goto EXIT;
        }
        /* cacal hash */
        ret = CalculateHash(buf, len, alg, totalBlocks + blockOffset);
        P_ERR_GOTO_WTTH_LOG(ret);

        blockOffset += hlen;
        rawBufLen -= readLen;
        SendSectionMessage(readLen, stepPersent, &countedLen, &num);
    }

    ret = mbedtls_md_starts(md_ctx);  
    P_ERR_GOTO_WTTH_LOG(ret);  

    size_t processed = 0;  
    while (processed < blockOffset) {
        size_t chunk_size = (blockOffset - processed) > (size_t)uintLen  
                            ? (size_t)uintLen : (blockOffset - processed);  
        ret = mbedtls_md_update(md_ctx, totalBlocks + processed, chunk_size);  
        P_ERR_GOTO_WTTH_LOG(ret);  
        processed += chunk_size;  
    }  
    ret = mbedtls_md_finish(md_ctx, outbuf); 
EXIT:
    APPV_FREE(buf);
    APPV_FREE(totalBlocks);
    mbedtls_md_free(md_ctx);
    APPV_FREE(md_ctx);
    return ret;
}

static int GetRootHashByBlock(const FileRead *fileRead, int uintLen, int alg, unsigned char **output, int *hashLen)
{
    /* block num */
    if (uintLen == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "unit len is zero");
        return V_ERR;
    }
    int rootHashLen = GetHashUnitLen(alg);
    *output = APPV_MALLOC(rootHashLen);
    if (*output == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return V_ERR_MALLOC;
    }
    *hashLen = rootHashLen;
    int ret = CacalRootHashByBlockHash(fileRead, uintLen, alg, *output);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cal hash error: %d", ret);
        APPV_FREE(*output);
        *output = NULL;
        return  V_ERR_CALC_BLOCK_HASH;
    }
    return V_OK;
}

static int ConvertToMbedHashAlg(short algId)
{
    int hashAlg = MBEDTLS_MD_NONE;
    // Keeps in sync with signature-side hash definition
    // hapsigntool/hap_sign_tool_lib/src/main/java/com/ohos/hapsigntool/utils/HashUtils.java
    enum {
        USE_NONE = 0,
        USE_MD2,
        USE_MD4,
        USE_MD5,
        USE_SHA1,
        USE_SHA224,
        USE_SHA256,
        USE_SHA384,
        USE_SHA512,
        USE_RIPEMD160,
    };
#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
    int hashMap[][2] = {
        { USE_NONE, MBEDTLS_MD_NONE },
        { USE_MD2, MBEDTLS_MD_MD2 },
        { USE_MD4, MBEDTLS_MD_MD4 },
        { USE_MD5, MBEDTLS_MD_MD5 },
        { USE_SHA1, MBEDTLS_MD_SHA1 },
        { USE_SHA224, MBEDTLS_MD_SHA224 },
        { USE_SHA256, MBEDTLS_MD_SHA256 },
        { USE_SHA384, MBEDTLS_MD_SHA384 },
        { USE_SHA512, MBEDTLS_MD_SHA512 },
        { USE_RIPEMD160, MBEDTLS_MD_RIPEMD160 }
    };
#else
    int hashMap[][2] = {
        { USE_NONE, MBEDTLS_MD_NONE },
        { USE_MD2, MBEDTLS_MD_NONE },
        { USE_MD4, MBEDTLS_MD_NONE },
        { USE_MD5, MBEDTLS_MD_MD5 },
        { USE_SHA1, MBEDTLS_MD_SHA1 },
        { USE_SHA224, MBEDTLS_MD_SHA224 },
        { USE_SHA256, MBEDTLS_MD_SHA256 },
        { USE_SHA384, MBEDTLS_MD_SHA384 },
        { USE_SHA512, MBEDTLS_MD_SHA512 },
        { USE_RIPEMD160, MBEDTLS_MD_RIPEMD160 }
    };
#endif
    for (int i = 0; i < (int)(sizeof(hashMap) / sizeof(hashMap[0])); i++) {
        if (hashMap[i][0] == algId) {
            hashAlg = hashMap[i][1];
            break;
        }
    }
    return hashAlg;
}

static int GetRawHash(const FileRead *fileRead, const ContentInfo *content, unsigned char **output, int *hashLen)
{
    /* raw buf: head -> signBlockHead->offset */
    int blockLen = GetHashBlockLen(fileRead->len, content->hashContent.tag);
    if (blockLen <= 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "unknown content tag");
        return V_ERR_INVALID_CONTENT_TAG;
    }

    /* hash alg */
    int hashAlg = ConvertToMbedHashAlg(content->hashContent.algId);
    /* check hash alg is valid */
    if (hashAlg != HASH_ALG_SHA256 && hashAlg != HASH_ALG_SHA384 && hashAlg != HASH_ALG_SHA512) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "hash alg invalid : %d", hashAlg);
        return V_ERR_INVALID_HASH_ALG;
    }

    int ret = GetRootHashByBlock(fileRead, blockLen, hashAlg, output, hashLen);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get root hash failed, ret: %d", ret);
        return V_ERR_GET_ROOT_HASH;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "get raw hash success");
    return V_OK;
}

static int CalcCmpContHash(const Pkcs7 *pkcs7, const SignerInfo *signer,
                           mbedtls_md_type_t algType, unsigned char *hash, size_t *hashLen)
{
    int rc;
    unsigned char *input = NULL;
    size_t inputLen;

    /* calc orinal context hash */
    rc = PKCS7_GetContentData((Pkcs7 *)pkcs7, &input, &inputLen);
    P_ERR_RETURN_WTTH_LOG(rc);

    rc = mbedtls_md(mbedtls_md_info_from_type(algType), input, inputLen, hash);
    if (rc) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: calc digest failed");
        return rc;
    }
    *hashLen = mbedtls_md_get_size(mbedtls_md_info_from_type(algType));

    /* compare the calc hash with the attributes hash */
    unsigned char *digInAttr = NULL;
    size_t digInAttrLen;
    rc = PKCS7_GetDigestInSignerAuthAttr((SignerInfo *)signer, &digInAttr, &digInAttrLen);
    if (rc != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "PKCS7_GetDigestInSignerAuthAttr error: %d", rc);
        return rc;
    }
    if (digInAttrLen != *hashLen) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: content hash len is not equal with attr's hash len");
        return V_ERR;
    }
    if (memcmp(hash, digInAttr, digInAttrLen) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: content hash not equal with attr hash");
        return V_ERR;
    }
    return V_OK;
}

static int CalcDigest(const Pkcs7 *pkcs7, const SignerInfo *signer,
                      mbedtls_md_type_t algType, unsigned char *hash, size_t *hashLen)
{
    int rc;
    unsigned char *input = NULL;
    size_t inputLen;
    rc = CalcCmpContHash(pkcs7, signer, algType, hash, hashLen);
    if (rc != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: content hash not equal with attr hash");
        return rc;
    }

    /* calc the attribute hash */
    rc = PKCS7_GetSignerAuthAttr(signer, &input, &inputLen);
    if (rc != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: PKCS7_GetSignerAuthAttr failed ret: %d", rc);
        return rc;
    }
    rc = mbedtls_md(mbedtls_md_info_from_type(algType), input, inputLen, hash);
    if (rc != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error: calc digest failed ret: %d", rc);
        return rc;
    }
    *hashLen = mbedtls_md_get_size(mbedtls_md_info_from_type(algType));
    return V_OK;
}

static int VerifyRawHash(const FileRead *fileRead, const Pkcs7 *pkcs7Handle)
{
    /* parse content */
    unsigned char *input = NULL;
    size_t inputLen;
    /* calc orinal context hash */
    int ret = PKCS7_GetContentData((Pkcs7 *)pkcs7Handle, &input, &inputLen);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get content info error: %d", ret);
        return ret;
    }

    ContentInfo *content = APPV_MALLOC(sizeof(ContentInfo));
    P_NULL_RETURN_WTTH_LOG(content);

    ret = memcpy_s(content, sizeof(ContentInfo), input, inputLen);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mem cpy error, ret: %d", ret);
        APPV_FREE(content);
        return ret;
    }
    ContentN2H(content);

    unsigned char *rawHash = NULL; /* max 64, sha512 */
    int hashLen;
    ret = GetRawHash(fileRead, content, &rawHash, &hashLen);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get raw hash failed");
        APPV_FREE(content);
        return V_ERR;
    }
    /*  compare hash */
    if (memcmp(rawHash, content->hashContent.hash, hashLen)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "hash diff");
        APPV_FREE(content);
        APPV_FREE(rawHash);
        return V_ERR_GET_HASH_DIFF;
    }
    APPV_FREE(content);
    APPV_FREE(rawHash);
    return V_OK;
}

static int GetCertTypeBySourceName(const TrustAppCert *cert)
{
    if (cert == NULL) {
        return CERT_TYPE_OTHER;
    } else if (!strcmp(cert->name, "huawei app gallary")) {
        return CERT_TYPE_APPGALLARY;
    } else if (!strcmp(cert->name, "huawei system apps")) {
        return CERT_TYPE_SYETEM;
    } else if (!strcmp(cert->name, "OpenHarmony apps")) {
        return CERT_TYPE_SYETEM;
    } else {
        return CERT_TYPE_OTHER;
    }
}

static const TrustAppCert *GetProfSourceBySigningCert(const SignerResovledInfo *signer,
                                                      const TrustAppCert* whiteList, int num)
{
    for (int i = 0; i < num; i++) {
        if (!strcmp(whiteList[i].issueCA, signer->issuer)) {
            if (!strcmp(whiteList[i].profileSignCert, signer->subject) ||
                !strcmp(whiteList[i].profileDebugSignCert, signer->subject)) {
                LOG_PRINT_STR("profile source name : %s", g_trustAppList[i].name);
                return  &whiteList[i];
            }
        }
    }
    return NULL;
}

static int GetProfileCertTypeBySignInfo(SignerResovledInfo *signer, int *certType)
{
    /* only support first signer cert */
    const TrustAppCert *trustCert = GetProfSourceBySigningCert(signer, g_trustAppList,
                                                               sizeof(g_trustAppList) / sizeof(TrustAppCert));
    if (g_isDebugMode && trustCert == NULL) {
        trustCert = GetProfSourceBySigningCert(signer, g_trustAppListTest,
                                               sizeof(g_trustAppListTest) / sizeof(TrustAppCert));
    }
    /* check level */
    if (trustCert != NULL && trustCert->maxCertPath < signer->depth) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cert maxdepth error: %d", signer->depth);
        return V_ERR;
    }
    *certType = GetCertTypeBySourceName(trustCert);
    return V_OK;
}


static const TrustAppCert *GetAppSourceBySigningCert(const SignerResovledInfo *signer,
                                                     const TrustAppCert* whiteList, int num)
{
    for (int i = 0; i < num; i++) {
        if (!strcmp(whiteList[i].appSignCert, signer->subject) &&
            !strcmp(whiteList[i].issueCA, signer->issuer)) {
                return  &whiteList[i];
        }
    }
    return NULL;
}

static int GetAppCertTypeBySignInfo(SignerResovledInfo *signer, int *certType)
{
    /* only support first signer cert */
    const TrustAppCert *trustCert = GetAppSourceBySigningCert(signer, g_trustAppList,
                                                              sizeof(g_trustAppList) / sizeof(TrustAppCert));
    if (g_isDebugMode && trustCert == NULL) {
        trustCert = GetAppSourceBySigningCert(signer, g_trustAppListTest,
                                              sizeof(g_trustAppListTest) / sizeof(TrustAppCert));
    }
    /* check level */
    if (trustCert != NULL && trustCert->maxCertPath < signer->depth) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cert maxdepth error: %d %d",
                    trustCert->maxCertPath, signer->depth);
        return V_ERR;
    }
    *certType = GetCertTypeBySourceName(trustCert);
    return V_OK;
}

/* get singer cert type by white list */
static int GetAppSingerCertType(Pkcs7 *pkcs7Handle, int *certType)
{
    SignersResovedInfo *sri = PKCS7_GetAllSignersResolvedInfo(pkcs7Handle);
    if (sri == NULL || sri->nrOfSigners == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Get all signer's resolved info failed");
        return V_ERR;
    }

    int ret = GetAppCertTypeBySignInfo(&sri->signers[0], certType);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get cert type by sign info failed: %d", ret);
        PKCS7_FreeAllSignersResolvedInfo(sri);
        return V_ERR;
    }
    PKCS7_FreeAllSignersResolvedInfo(sri);
    return V_OK;
}

/* get singer cert type by white list */
static int GetProfileSingerCertType(Pkcs7 *pkcs7Handle, int *certType)
{
    SignersResovedInfo *sri = PKCS7_GetAllSignersResolvedInfo(pkcs7Handle);
    if (sri == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Get all signer's resolved info failed");
        return V_ERR;
    }
    int ret = GetProfileCertTypeBySignInfo(&sri->signers[0], certType);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get cert type by sign info failed: %d", ret);
        PKCS7_FreeAllSignersResolvedInfo(sri);
        return V_ERR;
    }
    PKCS7_FreeAllSignersResolvedInfo(sri);
    return V_OK;
}

/* verfiy profile data integrity with sign */
int VerifyProfileSignGetRaw(const char *buf, int len, char **profileContent, int *contentLen)
{
    /* verfiy */
    char *profileData = NULL;
    int certType;
    unsigned char *input = NULL;
    size_t inputLen;
    Pkcs7 *pkcs7 = APPV_MALLOC(sizeof(Pkcs7));
    P_NULL_RETURN_WTTH_LOG(pkcs7);

    int ret = PKCS7_ParseSignedData((unsigned char *)buf, (size_t)len, pkcs7);
    P_ERR_GOTO_WTTH_LOG(ret);

    HILOG_INFO(HILOG_MODULE_AAFWK, "pkcs7 parse message success");

    /* verify sign, rawdata */
    ret = PKCS7_VerifyCertsChain(pkcs7);
    P_ERR_GOTO_WTTH_LOG(ret);

    HILOG_INFO(HILOG_MODULE_AAFWK, "Verify certs success");

    ret = GetProfileSingerCertType(pkcs7, &certType);
    P_ERR_GOTO_WTTH_LOG(ret);

    if (certType == CERT_TYPE_OTHER) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "cert type invalid");
        ret = V_ERR;
        goto EXIT;
    }
    ret = PKCS7_VerifySignerSignature(pkcs7, CalcDigest);
    P_ERR_GOTO_WTTH_LOG(ret);

    /* raw profile data: content */
    ret = PKCS7_GetContentData(pkcs7, &input, &inputLen);
    P_ERR_GOTO_WTTH_LOG(ret);
    ret = V_ERR;
    if (inputLen > MAX_PROFILE_SIZE) {
        goto EXIT;
    }

    profileData = APPV_PSMEM_MALLOC(inputLen);
    if (profileData == NULL) {
        ret = V_ERR;
        goto EXIT;
    }
    ret = memcpy_s(profileData, inputLen, input, inputLen);
    P_ERR_GOTO_WTTH_LOG(ret);

    PKCS7_FreeRes(pkcs7);
    APPV_FREE(pkcs7);
    *profileContent = profileData;
    *contentLen = (int)inputLen;
    HILOG_INFO(HILOG_MODULE_AAFWK, "verify profile get raw data ok");
    return V_OK;
EXIT:
    HILOG_ERROR(HILOG_MODULE_AAFWK, "verify profile get raw data fail");
    PKCS7_FreeRes(pkcs7);
    APPV_FREE(pkcs7);
    APPV_PSMEM_FREE(profileData);
    return ret;
}
static unsigned char *GetRsaPk(const mbedtls_pk_context *pk, int *len)
{
    unsigned char *buf = APPV_MALLOC(MAX_PK_BUF);
    if (buf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return NULL;
    }
    int ret = memset_s(buf, MAX_PK_BUF, 0, MAX_PK_BUF);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "memset error");
        APPV_FREE(buf);
        return NULL;
    }
    unsigned char *c = buf + MAX_PK_BUF;
    int pkLen = mbedtls_pk_write_pubkey(&c, buf, pk);
    if (pkLen < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get pk buf error");
        APPV_FREE(buf);
        return NULL;
    }
    unsigned char *pkBuf = APPV_MALLOC(pkLen);
    if (pkBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        APPV_FREE(buf);
        return NULL;
    }
    ret = memcpy_s(pkBuf, pkLen, c, pkLen);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mem copy error: %d", ret);
        APPV_FREE(buf);
        APPV_FREE(pkBuf);
        return NULL;
    }
    *len = pkLen;
    APPV_FREE(buf);
    return pkBuf;
}

static unsigned char *GetEcPk(const mbedtls_pk_context *pk, int *len)
{
    mbedtls_ecp_keypair *ecCtx = mbedtls_pk_ec(*pk);
    if (ecCtx == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get ec pk error");
        return NULL;
    }
    unsigned char *buf = APPV_MALLOC(MBEDTLS_ECP_MAX_PT_LEN);
    if (buf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return NULL;
    }
    int ret = mbedtls_ecp_point_write_binary(&ecCtx->grp, &ecCtx->Q,
                                             MBEDTLS_ECP_PF_UNCOMPRESSED, (size_t *)len, buf, MBEDTLS_ECP_MAX_PT_LEN);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get ecc pk key error");
        APPV_FREE(buf);
        return NULL;
    }
    unsigned char *pkBuf = APPV_MALLOC(*len);
    if (pkBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        APPV_FREE(buf);
        return NULL;
    }
    ret = memcpy_s(pkBuf, *len, buf, *len);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mem copy error: %d", ret);
        APPV_FREE(buf);
        APPV_FREE(pkBuf);
        return NULL;
    }
    APPV_FREE(buf);
    return pkBuf;
}

static unsigned char *GetPkBuf(const mbedtls_pk_context *pk, int *len)
{
    unsigned char *bufA = NULL;
    if (mbedtls_pk_get_type(pk) == MBEDTLS_PK_RSA || mbedtls_pk_get_type(pk) == MBEDTLS_PK_RSASSA_PSS) {
        bufA = GetRsaPk(pk, len);
    } else if (mbedtls_pk_get_type(pk) == MBEDTLS_PK_ECDSA || mbedtls_pk_get_type(pk) == MBEDTLS_PK_ECKEY) {
        bufA = GetEcPk(pk, len);
    }
    return bufA;
}

static int ParseCertGetPk(const char *certEncoded, AppSignPk *pk)
{
    mbedtls_x509_crt *cert = APPV_MALLOC(sizeof(mbedtls_x509_crt));
    P_NULL_RETURN_WTTH_LOG(cert);

    mbedtls_x509_crt_init(cert);
    int ret = mbedtls_x509_crt_parse(cert, (unsigned char *)certEncoded, strlen(certEncoded) + 1);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "load cert failed, ret: %d", ret);
        APPV_FREE(cert);
        return V_ERR;
    }
    int len;
    unsigned char *pkBuf = GetPkBuf(&cert->pk, &len);
    if (pkBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get pk error");
        mbedtls_x509_crt_free(cert);
        APPV_FREE(cert);
        return V_ERR;
    }
    pk->pk = (char *)pkBuf;
    pk->len = len;
    mbedtls_x509_crt_free(cert);
    APPV_FREE(cert);
    return V_OK;
}

static int GetAppSignPublicKey(const ProfileProf *profile, AppSignPk *pk)
{
    int ret;
    /* release cert */
    if (profile->bundleInfo.releaseCert &&
        strlen((char *)profile->bundleInfo.releaseCert) != 0) {
        ret = ParseCertGetPk((char *)profile->bundleInfo.releaseCert, pk);
    } else {
        ret = ParseCertGetPk((char *)profile->bundleInfo.devCert, pk);
    }
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "GetSignCertpk failed, ret: %d", ret);
        return V_ERR_GET_CERT_PK;
    }
    return V_OK;
}

static void FreeAppSignPublicKey(AppSignPk *pk)
{
    if (pk->pk != NULL) {
        APPV_FREE(pk->pk);
    }
    return;
}

int GetAppid(ProfileProf *profile)
{
    AppSignPk pk;
    int ret = GetAppSignPublicKey(profile, &pk);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get sign pk failed");
        return ret;
    }
    /* base64 */
    size_t useLen;
    mbedtls_base64_encode(NULL, 0, &useLen, (unsigned char *)pk.pk, pk.len);
    int bundleNameLen = strlen(profile->bundleInfo.bundleName);
    int appidLen = bundleNameLen + useLen + 1 + 1;

    char *appid = APPV_MALLOC(appidLen);
    if (appid == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc failed");
        FreeAppSignPublicKey(&pk);
        return V_ERR_MALLOC;
    }
    appid[appidLen - 1] = '\0';
    ret = snprintf_s(appid, appidLen, bundleNameLen + 1, "%s_", profile->bundleInfo.bundleName);
    if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "snprintf error ret: %d", ret);
        APPV_FREE(appid);
        FreeAppSignPublicKey(&pk);
        return V_ERR_GET_APPID;
    }
    ret = mbedtls_base64_encode((unsigned char *)appid + bundleNameLen + 1,
                                appidLen - bundleNameLen - 1, &useLen, (unsigned char *)pk.pk, pk.len);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "base 64 encode error");
        APPV_FREE(appid);
        FreeAppSignPublicKey(&pk);
        return V_ERR_GET_APPID;
    }
    profile->appid = appid;
    LOG_PRINT_STR("%s", appid);
    FreeAppSignPublicKey(&pk);
    return V_OK;
}

static void FreeProfileBuff(int certType, char **profBuf)
{
    if (certType == CERT_TYPE_APPGALLARY) {
        APPV_FREE(*profBuf);
        return;
    }
    APPV_PSMEM_FREE(*profBuf);
}

static int VerifyProfGetContent(int fp, HwSignHead *signHead, int certType, ProfileProf *pf)
{
    char *profBuf = NULL;
    int len;
    BlockHead blockHead = {0};
    int ret;

    if (certType == CERT_TYPE_APPGALLARY) {
        profBuf = GetSignBlockByType(signHead, fp, PROFILE_BLOCK_TYPE, &len, &blockHead);
        P_NULL_RETURN_RET_WTTH_LOG(profBuf, V_ERR_GET_PROFILE_DATA);
    } else {
        int rawLen;
        char *rawBuf = GetSignBlockByType(signHead, fp, PROFILE_BLOCK_WITHSIGN_TYPE, &rawLen, &blockHead);
        P_NULL_RETURN_RET_WTTH_LOG(rawBuf, V_ERR_GET_PROFILE_DATA);
        /* verify profile */
        ret = VerifyProfileSignGetRaw(rawBuf, rawLen, &profBuf, &len);
        APPV_PSMEM_FREE(rawBuf);
        P_ERR_RETURN_WTTH_LOG(ret);
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "ParseProfile");
    ret = ParseProfile(profBuf, len, pf);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "GetSignBlock error");
        FreeProfileBuff(certType, &profBuf);
        return V_ERR_GET_PARSE_PROFILE;
    }
    FreeProfileBuff(certType, &profBuf);
    ret = VerifyProfileContent(pf);
    P_ERR_GOTO_WTTH_LOG(ret);

    ret = GetAppid(pf);
    P_ERR_GOTO_WTTH_LOG(ret);

    return V_OK;
EXIT:
    HILOG_ERROR(HILOG_MODULE_AAFWK, "verify profile content failed");
    ProfFreeData(pf);
    return ret;
}

#ifndef LITE_WEARABLE
static void PrintCertInfo(const mbedtls_x509_crt *cert)
{
    char buf[LOG_BUF_SIZE] = {0};
    int ret = mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", cert);
    if (ret < 0) {
        return;
    }
    LOG_PRINT_STR("%s", buf);
}
#endif
static int CmpCert(const mbedtls_x509_crt *certA, const CertInfo *binSignCert)
{
    P_NULL_RETURN_RET_WTTH_LOG(certA, V_ERR);
    P_NULL_RETURN_RET_WTTH_LOG(binSignCert, V_ERR);
#ifndef LITE_WEARABLE
    PrintCertInfo(certA);
#endif
    /* cmp subject and issuer */
    if (certA->subject_raw.len != binSignCert->subjectLen ||
        memcmp(certA->subject_raw.p, binSignCert->subject, certA->subject_raw.len)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cert subject diff");
        return V_ERR;
    }

    if (certA->issuer_raw.len != binSignCert->issuerLen ||
        memcmp(certA->issuer_raw.p, binSignCert->issuer, certA->issuer_raw.len)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cert issuer diff");
        return V_ERR;
    }

    /* V_OK means same */
    if (mbedtls_pk_get_type(&certA->pk) != binSignCert->pkType) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "pk type diff");
        return V_ERR;
    }
    int lenA = 0;
    unsigned char *bufA = GetPkBuf(&certA->pk, &lenA);
    P_NULL_RETURN_RET_WTTH_LOG(bufA, V_ERR);

    if (lenA != binSignCert->pkLen) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "pkA len diff %d, %d", lenA, binSignCert->pkLen);
        APPV_FREE(bufA);
        return V_ERR;
    }

    if (memcmp(bufA, binSignCert->pkBuf, lenA)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "pk content different");
        APPV_FREE(bufA);
        return V_ERR;
    }
    APPV_FREE(bufA);
    return V_OK;
}

int LoadCertAndCmpDest(const unsigned char *certBase64, const CertInfo *binSignCert)
{
    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);
    int ret = mbedtls_x509_crt_parse(&cert, certBase64, strlen((char *)certBase64) + 1);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "load release cert failed");
        LOG_PRINT_STR("%s", certBase64);
        return V_ERR;
    }
    /* cmp cert */
    if (CmpCert(&cert, binSignCert) == V_OK) {
        mbedtls_x509_crt_free(&cert);
        return V_OK;
    }
    HILOG_ERROR(HILOG_MODULE_AAFWK, "cert inconsistent");
    mbedtls_x509_crt_free(&cert);
    return V_ERR;
}

static int CheckReleaseAppSign(const CertInfo *binSignCert, const ProfileProf *pf)
{
    /* if distribution type is app_gallery , return error */
    if (!strcmp(pf->appDistType, "app_gallery")) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "release, distribution type is app_gallery, return error");
        return V_ERR;
    }

    if (strlen((char *)pf->bundleInfo.releaseCert) == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "release app, release Cert null");
        return V_ERR;
    }
    int ret = LoadCertAndCmpDest(pf->bundleInfo.releaseCert, binSignCert);
    if (ret == V_OK) {
        return V_OK;
    }
    HILOG_ERROR(HILOG_MODULE_AAFWK, "app sign cert not consistent with profile cert");
    return V_ERR;
}

static int CheckDebugAppSign(CertInfo *binSignCert, const ProfileProf *pf)
{
    if (strlen((char *)pf->bundleInfo.devCert) == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "debug app, devCert null");
        return V_ERR;
    }
    int ret = LoadCertAndCmpDest(pf->bundleInfo.devCert, binSignCert);
    if (ret == V_OK) {
        return V_OK;
    }
    if (strlen((char *)pf->bundleInfo.releaseCert) != 0) {
        ret = LoadCertAndCmpDest(pf->bundleInfo.releaseCert, binSignCert);
        if (ret == V_OK) {
            HILOG_INFO(HILOG_MODULE_AAFWK, "release cert consistent");
            return V_OK;
        }
    }
    HILOG_ERROR(HILOG_MODULE_AAFWK, "app sign cert not consistent with profile cert");
    return V_ERR;
}

static int CheckAppSignCertWithProfile(int appCertType, CertInfo *binSignCert, ProfileProf *pf)
{
    /* cert type appgallary or system , not check */
    if (appCertType == CERT_TYPE_APPGALLARY || appCertType == CERT_TYPE_SYETEM) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "app type : %d, return OK", appCertType);
        return V_OK;
    }

    int ret = V_ERR;
    /* debug app, app cert consistent with profile dev or release cert */
    if (!strcmp("debug", (char *)pf->type)) {
        /* */
        ret = CheckDebugAppSign(binSignCert, pf);
    } else if (!strcmp("release", pf->type)) {
        ret = CheckReleaseAppSign(binSignCert, pf);
    }

    HILOG_INFO(HILOG_MODULE_AAFWK, "check app sign cert ret : %d", ret);
    return ret;
}

static int CertInfoInit(CertInfo *certInfo)
{
    int ret = memset_s(certInfo, sizeof(CertInfo), 0, sizeof(CertInfo));
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "memset error");
    }
    return ret;
}

void FreeCertInfo(CertInfo *certInfo)
{
    if (certInfo == NULL) {
        return;
    }
    if (certInfo->issuer != NULL) {
        APPV_FREE(certInfo->issuer);
        certInfo->issuerLen = 0;
    }

    if (certInfo->subject != NULL) {
        APPV_FREE(certInfo->subject);
        certInfo->subjectLen = 0;
    }

    if (certInfo->pkBuf != NULL) {
        APPV_FREE(certInfo->pkBuf);
        certInfo->pkLen = 0;
    }
    return;
}

int GetCertInfo(const mbedtls_x509_crt *ctr, CertInfo **binSignCert)
{
    CertInfo *certInfo = APPV_MALLOC(sizeof(CertInfo));
    P_NULL_RETURN_RET_WTTH_LOG(certInfo, V_ERR_MALLOC);

    int ret = CertInfoInit(certInfo);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cert info init");
        ret = V_ERR_MEMSET;
        goto EXIT;
    }
    certInfo->issuerLen = ctr->issuer_raw.len;
    certInfo->issuer = APPV_MALLOC(certInfo->issuerLen);
    if (certInfo->issuer == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        ret = V_ERR_MALLOC;
        goto EXIT;
    }

    ret = memcpy_s(certInfo->issuer, certInfo->issuerLen, ctr->issuer_raw.p, ctr->issuer_raw.len);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        ret = V_ERR_MEMCPY;
        goto EXIT;
    }

    certInfo->subjectLen = ctr->subject_raw.len;
    certInfo->subject = APPV_MALLOC(certInfo->subjectLen);
    if (certInfo->subject == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        ret = V_ERR_MALLOC;
        goto EXIT;
    }

    ret = memcpy_s(certInfo->subject, certInfo->subjectLen, ctr->subject_raw.p, ctr->subject_raw.len);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        ret = V_ERR_MEMCPY;
        goto EXIT;
    }

    certInfo->pkType = mbedtls_pk_get_type(&ctr->pk);
    certInfo->pkBuf = (char *)GetPkBuf(&ctr->pk, &certInfo->pkLen);

    if (certInfo->pkBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get pk error");
        ret = V_ERR;
        goto EXIT;
    }
    *binSignCert = certInfo;
    return V_OK;
EXIT:
    FreeCertInfo(certInfo);
    APPV_FREE(certInfo);
    return ret;
}

static int VerfiyAppSourceGetProfile(int fp, HwSignHead *signHead,
                                     int certType, CertInfo *binSignCert, ProfileProf *pf)
{
    int ret = VerifyProfGetContent(fp, signHead, certType, pf);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "VerifyProfGetContent error: %d", ret);
        return ret;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "verify prof get content success");

    /* verfiy profile cert and app sign cert */
    ret = CheckAppSignCertWithProfile(certType, binSignCert, pf);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "CheckAppSignCertWithProfile error: %d", ret);
        ProfFreeData(pf);
        return V_ERR_VERFIY_PROF_CERT;
    }

    /* free cert */
    FREE_IF_NOT_NULL(pf->bundleInfo.devCert);
    FREE_IF_NOT_NULL(pf->bundleInfo.releaseCert);

    HILOG_INFO(HILOG_MODULE_AAFWK, "verfiy app source success");
    return V_OK;
}

static int VerifyAppSignPkcsData(FileRead *fileRead,
                                 HwSignHead *signHead, Pkcs7 *pkcs7Handle)
{
    /*  verify sign, rawdata */
    int ret = PKCS7_VerifyCertsChain(pkcs7Handle);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Verify certs failed, ret: %d", ret);
        return V_ERR_VERIFY_CERT_CHAIN;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "Verify certs success");
    SendProgressMessage(FILE_CERCHAIN_VERIFY);
    ret = VerifyRawHash(fileRead, pkcs7Handle);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "VerifyRawHash failed : %d", ret);
        return ret;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "VerifyRawHash success");

    ret = PKCS7_VerifySignerSignature(pkcs7Handle, CalcDigest);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "pkcs7 verify signer signature failed : %d", ret);
        return V_ERR_VERIFY_SIGNATURE;
    }

    return V_OK;
}

static Pkcs7 *GetBinSignPkcs(const char *signBuf, int len)
{
    Pkcs7 *pkcs7 = APPV_MALLOC(sizeof(Pkcs7));
    if (pkcs7 == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return NULL;
    }
    int ret = PKCS7_ParseSignedData((unsigned char *)signBuf, (size_t)len, pkcs7);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "pkcs7parse message failed, ret: %d", ret);
        APPV_FREE(pkcs7);
        return NULL;
    }
    return pkcs7;
}

static FileRead *GetFileRead(int fp, int offset, int size)
{
    /* raw buf len = sign block head offset */
    FileRead *fileRead = APPV_MALLOC(sizeof(FileRead));
    if (fileRead == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        return NULL;
    }
    fileRead->fp = fp;
    fileRead->offset = offset;
    fileRead->len = size;
    return fileRead;
}
static int VerifyBinSign(HwSignHead *signH, int fp, CertInfo **signCert, int *certType)
{
    int blockLen;
    BlockHead blockHead = {0};
    FileRead *fileRead = NULL;
    int ret;

    char *signBuf = GetSignBlockByType(signH, fp, SIGNATURE_BLOCK_TYPE, &blockLen, &blockHead);
    P_NULL_RETURN_RET_WTTH_LOG(signBuf, V_ERR_GET_SIGN_BLOCK);

    Pkcs7 *pkcs7 = GetBinSignPkcs(signBuf, (size_t)blockLen);
    if (pkcs7 == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "GetBinSignPkcs failed");
        APPV_FREE(signBuf);
        return V_ERR_PARSE_PKC7_DATA;
    }
    /* pkcs7 handle the content of signBuf, do not free signBuf */
    HILOG_INFO(HILOG_MODULE_AAFWK, "pkcs7 parse message success");

    /* raw buf len = sign block head offset */
    fileRead = GetFileRead(fp, 0, blockHead.offset);
    if (fileRead == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc error");
        ret = V_ERR_MALLOC;
        goto EXIT;
    }

    // haomini go here
    ret = VerifyAppSignPkcsData(fileRead, signH, pkcs7);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "intergrity failed");
        ret = V_ERR_VERIFY_CERT_CHAIN;
        goto EXIT;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "pkcs7 verify signer signature success");

    ret = GetAppSingerCertType(pkcs7, certType);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "cert source invalid: %d", ret);
        ret = V_ERR_GET_CERT_TYPE;
        goto EXIT;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "get cert Type : %d", *certType);

    ret = GetCertInfo(pkcs7->signedData.signers.certPath.crt, signCert);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get bin cert info  error: %d", ret);
        ret = V_ERR_GET_CERT_INFO;
        goto EXIT;
    }

EXIT:
    /* free sign */
    APPV_FREE(signBuf);
    /* free pkcs7Handle */
    PKCS7_FreeRes(pkcs7);
    APPV_FREE(pkcs7);
    APPV_FREE(fileRead);
    return ret;
}

static int VerifyIntegrity(HwSignHead *signH, int fp, ProfileProf *pf)
{
    CertInfo *binSignCert = NULL;
    int certType;

    // haomini go here
    int ret = VerifyBinSign(signH, fp, &binSignCert, &certType);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "verify bin sign error");
        return ret;
    }
    SendProgressMessage(FILE_INTEGRITY_VERIFY);
    ret = VerfiyAppSourceGetProfile(fp, signH, certType, binSignCert, pf);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "verify app source failed : %d", ret);
        FreeCertInfo(binSignCert);
        APPV_FREE(binSignCert);
        return ret;
    }
    SendProgressMessage(PROFILE_CERCHAIN_VERIFY);
    FreeCertInfo(binSignCert);
    APPV_FREE(binSignCert);
    return V_OK;
}

static int InitMessageEnv(const char *filePath, MessageFunc func)
{
    g_messageFunc = NULL;
    if (filePath == NULL) {
        return V_ERR;
    }
    if (g_verifyFileName != NULL) {
        LOG_PRINT_STR("multi files verifying %s", filePath);
        return V_ERR;
    }
    char *name = strrchr(filePath, '/');
    if (name == NULL) {
        return V_ERR;
    }
    name++;
    if (strlen(name) == 0) {
        return V_ERR;
    }
    g_verifyFileName = APPV_MALLOC(strlen(name) + 1);
    P_NULL_RETURN_WTTH_LOG(g_verifyFileName);
    if (memcpy_s(g_verifyFileName, strlen(name) + 1, name, strlen(name) + 1) != V_OK) {
        APPV_FREE(g_verifyFileName);
        return V_ERR;
    }
    g_messageFunc = func;
    return V_OK;
}

static void FreeHeadAndMessageEnv(HwSignHead **signHead)
{
    APPV_FREE(*signHead);
    APPV_FREE(g_verifyFileName);
    g_messageFunc = NULL;
}

static int GetUnsignedFileLength(int handle, unsigned int signSize)
{
    struct stat *fileSt = APPV_MALLOC(sizeof(struct stat));
    if (fileSt == NULL) {
        LOG_PRINT_STR("malloc error");
        return V_ERR;
    }
    int ret = fstat(handle, fileSt);
    if (ret != V_OK) {
        APPV_FREE(fileSt);
        return V_ERR;
    }
    if (fileSt->st_size <= signSize) {
        APPV_FREE(fileSt);
        return V_ERR;
    }
    int unsignedFileLength = fileSt->st_size - signSize;
    HILOG_INFO(HILOG_MODULE_AAFWK, "file len: %d, unsigned len %d",
               fileSt->st_size, unsignedFileLength);
    APPV_FREE(fileSt);
    return unsignedFileLength;
}

/* ***********verify data ***********************************
 *     -----------------------------
 *     rawdata                 |
 *                             |
 *     -----------------       |          ---------------
 *     sign block head 1       |                   |
 *     type_1                 offset_1             |
 *     tag_1                   |                   |
 *     length_1                |                   |
 *     offset_1                |                   |
 *     -----------------       |                   |
 *     sign block head 2       |                   |
 *     type_2                  |                   |
 *     tag_2                   |                   |
 *     length2                 |                   |
 *     offset_2                |                   |
 *     -----------------      ---  ---             |
 *     signblock 1                  |              |
 *                                  |             length
 *                                 length_1        |
 *                                  |              |
 *                                  |              |
 *     -----------------           ---             |
 *     signblock 2                                 |
 *                                                 |
 *                                                 |
 *                                                 |
 *     -----------------                           |
 *     hwsign head                                 |
 *     length                                      | 
 *     -----------------                 ----------------
 * note: MessageFunc could be null, some app do not need progressing message
 */
int APPVERI_AppVerify(const char *filePath, VerifyResult *verifyRst)
{
    if (filePath == NULL || verifyRst == NULL) {
        return V_ERR_FILE_OPEN;
    }
    int handle = open(filePath, O_RDONLY, 0);
    if (handle < 0) {
        LOG_PRINT_STR("file open error %s", filePath);
        APPV_FREE(g_verifyFileName);
        g_messageFunc = NULL;
        return V_ERR_FILE_OPEN;
    }
    HwSignHead *signHead = NULL;
    int ret = GetSignHead(handle, &signHead);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get sign head error");
        close(handle);
        FreeHeadAndMessageEnv(&signHead);
        return ret;
    }
    (void)memset_s(&verifyRst->profile, sizeof(verifyRst->profile), 0, sizeof(verifyRst->profile));
    // haomini go here
    ret = VerifyIntegrity(signHead, handle, &verifyRst->profile);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "verify integrity failed");
        close(handle);
        FreeHeadAndMessageEnv(&signHead);
        return ret;
    }
    int unsignedFileLength = GetUnsignedFileLength(handle, signHead->size);
    if (unsignedFileLength == V_ERR) {
        ret = V_ERR_FILE_STAT;
    }
    verifyRst->appFileStart = 0;
    verifyRst->appFileLen = unsignedFileLength;
    close(handle);
    FreeHeadAndMessageEnv(&signHead);
    return ret;
}

/* set debug mode */
int APPVERI_SetDebugMode(bool mode)
{
    HILOG_INFO(HILOG_MODULE_AAFWK, "set debug mode: %d", mode);
    if (g_isDebugMode == mode) {
        return V_OK;
    }
    int ret = PKCS7_EnableDebugMode(mode);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "enable pcks7 debug mode failed");
        return ret;
    }
    g_isDebugMode = mode;
    return V_OK;
}

void APPVERI_FreeVerifyRst(VerifyResult *verifyRst)
{
    HILOG_INFO(HILOG_MODULE_AAFWK, "free verify rst data");
    ProfFreeData(&verifyRst->profile);
    return;
}

int APPVERI_GetUnsignedFileLength(const char *filePath)
{
    if (filePath == NULL) {
        return V_ERR;
    }
    int handle = open(filePath, O_RDONLY, 0);
    if (handle < 0) {
        LOG_PRINT_STR("file open error %s", filePath);
        return V_ERR;
    }
    HwSignHead *signHead = NULL;
    int ret = GetSignHead(handle, &signHead);
    if (ret != V_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "get sign head error");
        close(handle);
        APPV_FREE(signHead);
        return V_ERR;
    }
    int unsignedFileLength = GetUnsignedFileLength(handle, signHead->size);
    close(handle);
    APPV_FREE(signHead);
    return unsignedFileLength;
}
