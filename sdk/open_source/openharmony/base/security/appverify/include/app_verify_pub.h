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

#ifndef SECURITY_APP_VERIFY_PUB_H
#define SECURITY_APP_VERIFY_PUB_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    V_OK = 0,

    /* begin app sign parse */
    V_ERR_GET_CERT_INFO = 0xef000002,
    V_ERR_UNTRUSTED_CERT = 0xef000003,
    V_ERR_INTEGRITY = 0xef000004,
    V_ERR_GET_SIGNHEAD = 0xef000005,
    V_ERR_GET_SIGN_BLOCK = 0xef000006,
    V_ERR_GET_HASH_DIFF = 0xef000007,
    V_ERR_INVALID_CONTENT_TAG = 0xef000008,
    V_ERR_INVALID_HASH_ALG = 0xef000009,
    V_ERR_GET_ROOT_HASH = 0xef00000a,
    V_ERR_CALC_BLOCK_HASH = 0xef00000c,
    V_ERR_PARSE_PKC7_DATA = 0xef00000d,
    V_ERR_VERIFY_CERT_CHAIN = 0xef00000e,
    V_ERR_VERIFY_SIGNATURE = 0xef00000f,
    V_ERR_GET_CERT_TYPE = 0xef000010,

    /* begin profile signparse */
    V_ERR_GET_PROFILE_DATA = 0xef000011,
    V_ERR_GET_PARSE_PROFILE = 0xef000012,
    V_ERR_PROF_CONTENT_INVALID = 0xef000013,
    V_ERR_VERFIY_PROF_CERT = 0xef000014,
    V_ERR_GET_CERT_PK = 0xef000015,
    V_ERR_GET_APPID = 0xef000016,
    V_ERR_INVALID_DISP_TYPE = 0xef000017,
    V_ERR_INVALID_APP_BUNDLE = 0xef000018,
    V_ERR_INVALID_DATE = 0xef000019,
    V_ERR_INVALID_DEVID = 0xef00001a,

    /* begin file operation */
    V_ERR_FILE_OPEN = 0xef00001b,
    V_ERR_FILE_STAT = 0xef00001c,
    V_ERR_FILE_LENGTH = 0xef00001d,

    /* begin memory operation */
    V_ERR_MEMSET = 0xef00001e,
    V_ERR_MEMCPY = 0xef00001f,
    V_ERR_MALLOC = 0xef000020,

    /* default error */
    V_ERR = 0xffffffff,
} AppVErrCode;

typedef struct {
    int notBefore;
    int notAfter;
} ProfValidity;

typedef struct {
    char *developerId; /* developer-id */
    unsigned char *devCert;     /* development-certificate */
    unsigned char *releaseCert; /* distribution-certificate */
    char *bundleName;  /* bundle-name */
    char *appFeature;  /* app-feature : hos_system_app/hos_normal_app */
} ProfBundleInfo;

typedef struct {
    int restricNum;
    char **restricPermission;
    int permissionNum;
    char **permission;
} ProfPermission;

typedef struct {
    char *devIdType;
    int devidNum;
    char **deviceId;
} ProfDebugInfo;

typedef struct {
    int versionCode;  /* version */
    char *versionName;  /* version description */
    char *uuid;  /* uuid */
    char *type;  /* debug/release */
    char *appDistType;  /* app-distribution-type */
    ProfValidity validity; /* validity */
    ProfBundleInfo bundleInfo; /* bundle-info */
    ProfPermission permission; /* permissions */
    ProfDebugInfo  debugInfo;  /* debug-info */
    char *issuer;     /* issuer */
    char *appid;           /* bundle_name_pk(base64) */
} ProfileProf;

typedef struct {
    char *pk;
    int len;
} AppSignPk;

#ifdef OH_MARKET_ENABLE
#define NOISE_SIZE 32
#define SALT_INFO_POOL_ROWS 3
#define PUBLIC_KEY_FILE_LEN (SALT_INFO_POOL_ROWS * NOISE_SIZE * 2 + 4 + 1)
#define CLIENT_NOISE_LEN (PUBLIC_KEY_FILE_LEN - 4)
#define FRONT_FIXED_SIZE 204
#define FIXED_BYTES_LEN 6
#define AES_KEY_LEN 16
#define ECP_PUBKEY_LEN 65
#define ENC_TYPE_ECC 0x22
static const unsigned char FIXED_BYTES[FIXED_BYTES_LEN] = {0xa5, 0x01, 0x02, 0x03, 0x04, 0x5a};

#pragma pack(push, 4)
typedef struct {
    uint32_t originalLen;
    uint32_t encryptedLen;
    uint32_t sigLen;
    uint32_t sigOffset;
    uint32_t appCertLen;
    uint32_t intermediateCertLen;
    uint8_t iv1[AES_KEY_LEN];
    uint8_t iv2[AES_KEY_LEN];
    uint8_t password[AES_KEY_LEN];
    uint8_t pubKeyType;
    uint8_t padLen;
    uint8_t padData[AES_KEY_LEN];
    uint8_t dialViewCount;
    uint8_t reserve[5];
} MetadataStruct;
#pragma pack(pop)
#endif

struct VfyRst;
typedef int (*GetSignPk)(struct VfyRst *verifyRst, AppSignPk *pk);
typedef void (*FreeSignPK)(AppSignPk *pk);
typedef int (*MessageFunc)(unsigned char operationResult, const char *bundleName, unsigned char errCode);

typedef struct VfyRst {
    ProfileProf profile;   /* profile */
    size_t appFileStart;   /* raw bin file start */
    size_t appFileLen;     /* raw bin file len */
} VerifyResult;

/* verify app integrity, return 0 if OK, otherwise errcode AppVErrCode */
//int APPVERI_AppVerify(const char *filePath, MessageFunc func, VerifyResult *verifyRst);
int APPVERI_AppVerify(const char *filePath, VerifyResult *verifyRst);

/* free the verify result of APPVERI_AppVerify returned */
void APPVERI_FreeVerifyRst(VerifyResult *verifyRst);

/* set debugmode */
int APPVERI_SetDebugMode(bool mode);

/* get unsigned file len without verify, return V_ERR if not OK */
int APPVERI_GetUnsignedFileLength(const char *filePath);

#ifdef OH_MARKET_ENABLE
/*******************************************************************************
 * Function    : PKCS7_GenerateECCKeypair
 * Description : Generate ECC brainpool256R1 key pair and write to PEM files
 *
 * Return      : int 0 on success, non-zero error code on failure
 *******************************************************************************/
int32_t PKCS7_GenerateECCKeypair(void);

/*******************************************************************************
 * Function    : PKCS7_VerifyAndDecrypt
 * Description : Decrypt the signature verification fee dial data and return the
                 dial data that has passed integrity verification and decryption
 * Input       : fp     - Buffer for storing decrypted dial data
 *               firstLoad       - Is this the first time loading this dial
 *               meta            - Encrypt the metadata data structure content in the dial
 *               restoredBufLen  - The length of decrypted dials header and types
 * Output      : restoredBuf       - Data buffer for storing decrypted dials header and types
 * Return      : 0 on success, others on fail
 *******************************************************************************/
int32_t PKCS7_VerifyAndDecrypt(FILE *fp, bool firstLoad, MetadataStruct *meta, unsigned char *restoredBuf, uint32_t restoredBufLen);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
