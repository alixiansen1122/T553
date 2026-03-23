/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file hmac.c
 * Description: hmac原语实现
 * Author: liushaoming
 * Create: 2022-04-18
 */
#include <stdlib.h>
#include <stdbool.h>
#include "securec.h"
#include "sal_mem.h"
#include "crypt_errno.h"
#include "bsl_err_internal.h"
#include "crypt_hmac.h"


#define GOTO_ERROR_IF(condition, ret) do {   \
    if (condition) {                         \
        BSL_ERR_PUSH_ERROR((ret));           \
        goto ERR;                            \
    }                                        \
} while (0)

uint32_t CRYPT_HMAC_GetMacLen(const CRYPT_HMAC_Ctx *ctx)
{
    if (ctx == NULL || ctx->method == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return 0;
    }
    return ctx->method->mdSize;
}

int32_t CRYPT_HMAC_InitCtx(CRYPT_HMAC_Ctx *ctx, const EAL_MdMethod *m)
{
    if (ctx == NULL || m == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    void *mdCtx = (void *)SAL_MALLOC(m->ctxSize * 3); // 3个ctx 包括mdCtx iCtx oCtx
    if (mdCtx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }
    // 3个ctx清零 包括mdCtx iCtx oCtx
    (void)memset_s(mdCtx, m->ctxSize * 3, 0, m->ctxSize * 3);

    ctx->mdCtx = mdCtx;
    ctx->iCtx = (void *)((uint8_t *)mdCtx + m->ctxSize);
    // 偏移2个ctxSize (mdCtx,iCtx)
    ctx->oCtx = (void *)((uint8_t *)mdCtx + m->ctxSize * 2);
    ctx->method = m;

    return CRYPT_SUCCESS;
}

void CRYPT_HMAC_DeinitCtx(CRYPT_HMAC_Ctx *ctx)
{
    if (ctx == NULL || ctx->method == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return;
    }
    const EAL_MdMethod *method = ctx->method;
    // 3个ctx清零 包括mdCtx iCtx oCtx
    SAL_CleanseData((void *)(ctx->mdCtx), method->ctxSize * 3);
    SAL_FREE(ctx->mdCtx);
}

int32_t CRYPT_HMAC_Init(CRYPT_HMAC_Ctx *ctx, const uint8_t *key, uint32_t len)
{
    if (ctx == NULL || ctx->method == NULL || (key == NULL && len != 0)) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    const EAL_MdMethod *method = ctx->method;
    uint32_t blockSize = method->blockSize;
    uint8_t tmp[HMAC_MAXBLOCKSIZE];
    uint32_t tmpLen = HMAC_MAXBLOCKSIZE;
    const uint8_t *keyTmp = key;
    uint32_t i, keyLen = len;
    uint8_t ipad[HMAC_MAXBLOCKSIZE];
    uint8_t opad[HMAC_MAXBLOCKSIZE];
    int32_t ret;

    if (keyLen > blockSize) {
        keyTmp = tmp;
        ret = method->init(ctx->mdCtx);
        GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
        ret = method->update(ctx->mdCtx, key, keyLen);
        GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
        ret = method->final(ctx->mdCtx, tmp, &tmpLen);
        GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
        keyLen = method->mdSize;
    }
    for (i = 0; i < keyLen; i++) {
        ipad[i] = 0x36 ^ keyTmp[i];
        opad[i] = 0x5c ^ keyTmp[i];
    }
    for (i = keyLen; i < blockSize; i++) {
        ipad[i] = 0x36;
        opad[i] = 0x5c;
    }
    ret = method->init(ctx->iCtx);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    ret = method->update(ctx->iCtx, ipad, method->blockSize);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    ret = method->init(ctx->oCtx);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    ret = method->update(ctx->oCtx, opad, method->blockSize);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    ret = method->copyCtx(ctx->mdCtx, ctx->iCtx);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);

    (void)memset_s(tmp, HMAC_MAXBLOCKSIZE, 0, HMAC_MAXBLOCKSIZE);
    return CRYPT_SUCCESS;

ERR:
    method->deinit(ctx->mdCtx);
    method->deinit(ctx->iCtx);
    method->deinit(ctx->oCtx);
    return ret;
}

int32_t CRYPT_HMAC_Update(CRYPT_HMAC_Ctx *ctx, const uint8_t *in, uint32_t len)
{
    if (ctx == NULL || ctx->method == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    return ctx->method->update(ctx->mdCtx, in, len);
}

int32_t CRYPT_HMAC_Final(CRYPT_HMAC_Ctx *ctx, uint8_t *out, uint32_t *len)
{
    if (ctx == NULL || ctx->method == NULL || out == NULL || len == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    const EAL_MdMethod *method = ctx->method;
    if (*len < method->mdSize) {
        BSL_ERR_PUSH_ERROR(CRYPT_HMAC_OUT_BUFF_LEN_NOT_ENOUGH);
        return CRYPT_HMAC_OUT_BUFF_LEN_NOT_ENOUGH;
    }
    *len = method->mdSize;
    uint8_t tmp[HMAC_MAXOUTSIZE];
    uint32_t tmpLen = sizeof(tmp);
    int32_t ret;
    ret = method->final(ctx->mdCtx, tmp, &tmpLen);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    ret = method->copyCtx(ctx->mdCtx, ctx->oCtx);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    ret = method->update(ctx->mdCtx, tmp, tmpLen);
    GOTO_ERROR_IF(ret != CRYPT_SUCCESS, ret);
    return method->final(ctx->mdCtx, out, len);
ERR:
    return ret;
}

void CRYPT_HMAC_Reinit(CRYPT_HMAC_Ctx *ctx)
{
    if (ctx == NULL || ctx->method == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return;
    }
    const EAL_MdMethod *method = ctx->method;
    method->copyCtx(ctx->mdCtx, ctx->iCtx);
}

void CRYPT_HMAC_Deinit(CRYPT_HMAC_Ctx *ctx)
{
    if (ctx == NULL || ctx->method == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return;
    }
    const EAL_MdMethod *method = ctx->method;
    (void)method->deinit(ctx->mdCtx);
    (void)method->deinit(ctx->iCtx);
    (void)method->deinit(ctx->oCtx);
}
