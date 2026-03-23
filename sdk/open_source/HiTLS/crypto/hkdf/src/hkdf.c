/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file hkdf.c
 * Description: hkdf算法原语实现
 * Author: liushaoming
 * Create: 2022-05-11
 */
#include <stdint.h>
#include "securec.h"
#include "bsl_err_internal.h"
#include "sal_mem.h"
#include "crypt_local_types.h"
#include "crypt_errno.h"
#include "crypt_utils.h"
#include "crypt_hkdf.h"

#define HKDF_MAX_HMACSIZE 64

int32_t CRYPT_HKDF_Extract(const EAL_MacMethod *macMeth, const EAL_MdMethod *mdMeth, const uint8_t *key,
    uint32_t keyLen, const uint8_t *salt, uint32_t saltLen, uint8_t *prk, uint32_t *prkLen)
{
    int32_t ret = CRYPT_SUCCESS;
    void *macCtx = SAL_MALLOC(macMeth->ctxSize);
    if (macCtx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }
    (void)memset_s(macCtx, macMeth->ctxSize, 0, macMeth->ctxSize);

    ret = macMeth->initCtx(macCtx, mdMeth);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->init(macCtx, salt, saltLen);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->update(macCtx, key, keyLen);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->final(macCtx, prk, prkLen);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);

EXIT:
    macMeth->deinit(macCtx);
    macMeth->deinitCtx(macCtx);
    SAL_FREE(macCtx);
    return ret;
}

int32_t CRYPT_HKDF_Expand(const EAL_MacMethod *macMeth, const EAL_MdMethod *mdMeth, const uint8_t *prk, uint32_t prkLen,
    const uint8_t *info, uint32_t infoLen, uint8_t *out, uint32_t outLen)
{
    int32_t ret = CRYPT_SUCCESS;
    uint8_t hash[HKDF_MAX_HMACSIZE];
    uint32_t hashLen = HKDF_MAX_HMACSIZE;
    uint8_t counter = 1;
    uint32_t totalLen = 0;
    uint32_t n;
    void *macCtx = SAL_MALLOC(macMeth->ctxSize);
    if (macCtx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }

    ret = macMeth->initCtx(macCtx, mdMeth);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->init(macCtx, prk, prkLen);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->update(macCtx, info, infoLen);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->update(macCtx, &counter, 1);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    ret = macMeth->final(macCtx, hash, &hashLen);
    GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
    if (hashLen >= outLen) {
        (void)memcpy_s(out, outLen, hash, outLen);
        goto EXIT;
    }
    (void)memcpy_s(out, outLen, hash, hashLen);
    totalLen += hashLen;
    /* ceil(a / b) = (a + b - 1) / b */
    n = (outLen + hashLen - 1) / hashLen;
    // 2 表示计数2，上面代码已经使用了计数1
    counter = 2;
    for (uint32_t i = 2; i <= n; i++, counter++) {
        macMeth->reinit(macCtx);
        ret = macMeth->update(macCtx, hash, hashLen);
        GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
        ret = macMeth->update(macCtx, info, infoLen);
        GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
        ret = macMeth->update(macCtx, &counter, 1);
        GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
        ret = macMeth->final(macCtx, hash, &hashLen);
        GOTO_EXIT_IF(ret != CRYPT_SUCCESS);
        hashLen = hashLen > (outLen - totalLen) ? (outLen - totalLen) : hashLen;
        (void)memcpy_s(out + totalLen, outLen - totalLen, hash, hashLen);
        totalLen += hashLen;
    }

EXIT:
    macMeth->deinit(macCtx);
    macMeth->deinitCtx(macCtx);
    SAL_FREE(macCtx);
    return ret;
}

int32_t CRYPT_HKDF(const EAL_MacMethod *macMeth, const EAL_MdMethod *mdMeth, const uint8_t *key, uint32_t keyLen,
    const uint8_t *salt, uint32_t saltLen, const uint8_t *info, uint32_t infoLen, uint8_t *out, uint32_t len)
{
    if (macMeth == NULL || mdMeth == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (key == NULL && keyLen > 0) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (salt == NULL && saltLen > 0) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (info == NULL && infoLen > 0) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if ((out == NULL) || (len == 0)) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    int ret;
    uint32_t hashLen = mdMeth->mdSize;
    /* len cannot be larger than 255 * hashLen */
    if (len > hashLen * 255) {
        return CRYPT_HKDF_DKLEN_OVERFLOW;
    }

    uint8_t prk[HKDF_MAX_HMACSIZE];
    uint32_t prkLen = HKDF_MAX_HMACSIZE;
    ret = CRYPT_HKDF_Extract(macMeth, mdMeth, key, keyLen, salt, saltLen, prk, &prkLen);
    if (ret != CRYPT_SUCCESS) {
        return ret;
    }
    ret = CRYPT_HKDF_Expand(macMeth, mdMeth, prk, prkLen, info, infoLen, out, len);
    if (ret != CRYPT_SUCCESS) {
        return ret;
    }
    return CRYPT_SUCCESS;
}
