/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file gcm.c
 * Description: gcm模式
 * Author: linkaixin
 * Create: 2022-05-13
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-05-13  linkaixin        gcm模式
 */
#ifndef HITLS_CRYPTO_NO_GCM
#include <stdint.h>
#include "securec.h"
#include "crypt_utils.h"
#include "crypt_errno.h"
#include "sal_mem.h"
#include "bsl_err_internal.h"
#include "modes_local.h"
#include "crypt_modes_gcm.h"

/**
 * NIST_800-38D-5.2
 * len(P) ≤ 2^39-256 (bit)
 * 等价于 len(P) ≤ 2^36 - 32 (byte)
 */
#define GCM_MAX_COMBINED_LENGTH     (((uint64_t)1 << 36) - 32)
/**
 * NIST_800-38D-8.3
 * The total number of invocations of the authenticated encryption function shall not exceed
 * 2^32, including all IV lengths and all instances of the authenticated encryption function with
 * the given ciphCtx
 */
#define GCM_MAX_INVOCATIONS_TIMES   ((uint32_t)(-1))

#define GCM_BLOCK_MASK (0xfffffff0)

int32_t MODES_GCM_InitCtx(MODES_GCM_Ctx *ctx, const struct EAL_CipherMethodLocal *m)
{
    if (ctx == NULL || m == NULL) {
        return CRYPT_NULL_INPUT;
    }
    (void)memset_s(ctx, sizeof(MODES_GCM_Ctx), 0, sizeof(MODES_GCM_Ctx));
    ctx->ciphMeth = m;
    ctx->ciphCtx = SAL_MALLOC(m->ctxSize);
    if (ctx->ciphCtx == NULL) {
        return CRYPT_MEM_ALLOC_FAIL;
    }
    return CRYPT_SUCCESS;
}

void MODES_GCM_DeinitCtx(MODES_GCM_Ctx *ctx)
{
    if (ctx == NULL) {
        return;
    }
    (void)SAL_CleanseData(ctx->ciphCtx, ctx->ciphMeth->ctxSize);
    SAL_FREE(ctx->ciphCtx);
    (void)SAL_CleanseData(ctx, sizeof(MODES_GCM_Ctx));
}

void MODES_GCM_Clean(MODES_GCM_Ctx *ctx)
{
    if (ctx == NULL) {
        return;
    }
    void *ciphCtx = ctx->ciphCtx;
    const EAL_CipherMethod *ciphMeth = ctx->ciphMeth;
    SAL_CleanseData((void *)(ciphCtx), ciphMeth->ctxSize);
    SAL_CleanseData((void *)(ctx), sizeof(MODES_GCM_Ctx));
    ctx->ciphCtx = ciphCtx;
    ctx->ciphMeth = ciphMeth;
}

int32_t MODES_GCM_SetKey(MODES_GCM_Ctx *ctx, const uint8_t *ciphCtx, uint32_t len)
{
    if (ctx == NULL) {
        return CRYPT_NULL_INPUT;
    }
    uint8_t gcmKey[GCM_BLOCKSIZE] = { 0 };
    MODES_GCM_Clean(ctx);
    int32_t ret = ctx->ciphMeth->setEncryptKey(ctx->ciphCtx, ciphCtx, len);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    ret = ctx->ciphMeth->encrypt(ctx->ciphCtx, gcmKey, gcmKey, GCM_BLOCKSIZE);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    GcmTableGen4bit(gcmKey, ctx->hTable);
    ctx->tagLen = 16; // 默认tag长度为128bit 即16字节
    SAL_CleanseData(gcmKey, sizeof(gcmKey));
    return CRYPT_SUCCESS;
}

// 更新使用次数
static int32_t CheckUseCnt(const MODES_GCM_Ctx *ctx)
{
    // 128, 120, 112, 104, or 96 即 12 byte - 16 byte
    if (ctx->cryptCnt == GCM_MAX_INVOCATIONS_TIMES) {
        return CRYPT_MODES_KEYUSE_TOOMANY_TIME;
    }
    return CRYPT_SUCCESS;
}

/**
 * NIST_800-38D-5.2
 * 1 ≤ len(IV) ≤ 2^64 - 1 (bit)
 * 当前限制其不超过 2^32 - 1 byte
 */
static int32_t SetIv(MODES_GCM_Ctx *ctx, const uint8_t *iv, uint32_t ivLen)
{
    if (iv == NULL || ivLen == 0) {
        return CRYPT_NULL_INPUT;
    }
    int32_t ret = CheckUseCnt(ctx); // 检查使用次数
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    uint32_t i;
    uint64_t len = (uint64_t)ivLen;
    // when ivLen == 0, do reinit, no need to refersh iv
    if (len == 12) { // len(IV ) = 96bit = 12byte
        const uint8_t ivPad[4] = {0x00, 0x00, 0x00, 0x01};
        /* Y0 = IV || 0^31 || 1  if len(IV ) = 96 = 12byte */
        (void)memcpy_s(ctx->iv, GCM_BLOCKSIZE, iv, 12);
        (void)memcpy_s(ctx->iv + 12, GCM_BLOCKSIZE - 12, ivPad, sizeof(ivPad)); // 补充后4bit(base = 12)
    } else {
        /* Y0 = GHASH(H, {}, IV ) otherwise */
        (void)memset_s(ctx->iv, GCM_BLOCKSIZE, 0, GCM_BLOCKSIZE);
        const uint8_t *off = iv;
        uint32_t blockLen = ivLen & GCM_BLOCK_MASK;
        uint32_t lastLen = ivLen - blockLen;
        uint8_t tmp[GCM_BLOCKSIZE] = {0};
        if (blockLen > 0) {
            GcmHashMultiBlock(ctx->iv, ctx->hTable, off, blockLen);
            off += blockLen;
        }
        if (lastLen > 0) {
            for (i = 0; i < lastLen; i++) {
                tmp[i] = off[i];
            }
            GcmHashMultiBlock(ctx->iv, ctx->hTable, tmp, GCM_BLOCKSIZE);
        }
        len = (uint64_t)ivLen << 3; // bitLen = byteLen << 3
        (void)SAL_CleanseData(tmp, GCM_BLOCKSIZE);
        Uint64ToBeBytes(len, tmp + 8); // 后8字节存放iv长度
        GcmHashMultiBlock(ctx->iv, ctx->hTable, tmp, GCM_BLOCKSIZE);
    }
    /**
     * NIST_800-38D-7.1
     * GCTR(J0)
     */
    ctx->ciphMeth->encrypt(ctx->ciphCtx, ctx->iv, ctx->ek0, GCM_BLOCKSIZE);

    /**
     * NIST_800-38D-7.1
     * INC32
     * the 32-bit incrementing function is applied to the pre-counter block
     * to produce the initial counter block for an invocation of the GCTR
     * function on the plaintext
     */
    uint32_t ctr = GET_UINT32_BE(ctx->iv, 12); // 偏移12字节，取最后四字节
    ctr++;
    PUT_UINT32_BE(ctr, ctx->iv, 12); // 偏移12字节写回

    // 重置信息
    (void)memset_s(ctx->ghash, GCM_BLOCKSIZE, 0, GCM_BLOCKSIZE);
    ctx->aadLen = 0;
    ctx->lastLen = 0;
    ctx->plaintextLen = 0;

    // 敏感信息清除
    SAL_CleanseData(&ctr, sizeof(uint32_t));
    return CRYPT_SUCCESS;
}

/**
 * NIST_800-38D-5.2
 * len(AAD) ≤ 2^64 - 1 (bit)
 * 当前限制其不超过 2^32 - 1 byte
 */
static int32_t SetAad(MODES_GCM_Ctx *ctx, const uint8_t *aad, uint32_t aadLen)
{
    if (aad == NULL && aadLen != 0) {
        return CRYPT_NULL_INPUT;
    }
    const uint8_t *off = aad;
    uint32_t i;
    if (ctx->aadLen != 0) { // aad已设置
        BSL_ERR_PUSH_ERROR(CRYPT_MODES_AAD_REPEAT_SET_ERROR);
        return CRYPT_MODES_AAD_REPEAT_SET_ERROR;
    }
    uint32_t blockLen = aadLen & GCM_BLOCK_MASK;
    uint32_t lastLen = aadLen - blockLen;
    if (blockLen > 0) {
        GcmHashMultiBlock(ctx->ghash, ctx->hTable, off, blockLen);
        off += blockLen;
    }
    if (lastLen > 0) {
        uint8_t temp[GCM_BLOCKSIZE] = {0};
        for (i = 0; i < lastLen; i++) {
            temp[i] = off[i];
        }
        GcmHashMultiBlock(ctx->ghash, ctx->hTable, temp, GCM_BLOCKSIZE);
    }
    ctx->aadLen = aadLen;
    return CRYPT_SUCCESS;
}

// 判断加密长度时候溢出，并刷新已加密长度信息
int32_t CryptLenCheckAndRefresh(MODES_GCM_Ctx *ctx, uint32_t len)
{
    // len只有32bit 结合该长度限制，该计算不会产生溢出
    uint64_t plaintextLen = ctx->plaintextLen + len;
    if (plaintextLen > GCM_MAX_COMBINED_LENGTH) {
        BSL_ERR_PUSH_ERROR(CRYPT_MODES_CRYPTLEN_OVERFLOW);
        return CRYPT_MODES_CRYPTLEN_OVERFLOW;
    }
    ctx->plaintextLen = plaintextLen;
    return CRYPT_SUCCESS;
}

typedef struct {
    const uint8_t *in;
    uint8_t *out;
    const uint8_t *ctr;
    uint8_t *ct;
} XorCryptData;

static void XorInEncrypt(XorCryptData *data, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++) {
        data->out[i] = data->in[i] ^ data->ctr[i];
        data->ct[i] = data->out[i];
    }
}

static void XorInDecrypt(XorCryptData *data, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++) {
        data->ct[i] = data->in[i];
        data->out[i] = data->in[i] ^ data->ctr[i];
    }
}

// 处理上一次update剩余数据
uint32_t LastHandle(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, bool enc)
{
    uint32_t lastLen = 0;
    if (ctx->lastLen > 0) {
        XorCryptData data;
        lastLen = (ctx->lastLen < len) ? ctx->lastLen : len;
        data.in = in;
        data.out = out;
        data.ctr = &(ctx->last[GCM_BLOCKSIZE - ctx->lastLen]);
        data.ct = &(ctx->remCt[GCM_BLOCKSIZE - ctx->lastLen]);
        if (enc) { // ctx->lastLen 必然小于 GCM_BLOCKSIZE
            XorInEncrypt(&data, lastLen);
        } else {
            XorInDecrypt(&data, lastLen);
        }
        // 刷新剩余长度
        ctx->lastLen -= lastLen;
        if (ctx->lastLen == 0) {
            GcmHashMultiBlock(ctx->ghash, ctx->hTable, ctx->remCt, GCM_BLOCKSIZE);
        }
    }
    return lastLen;
}

static void GcmMultiBlockCrypt(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, bool enc)
{
    uint32_t blockLen = len;
    const uint8_t *dataIn = in;
    uint8_t *dataOut = out;
    uint32_t ctr = GET_UINT32_BE(ctx->iv, 12); // 计数信息，iv的最后32位，需偏移12字节(16 - 4 = 12)
    if (enc == false) {
        GcmHashMultiBlock(ctx->ghash, ctx->hTable, in, len);
    }
    while (blockLen > 0) {
        ctx->ciphMeth->encrypt(ctx->ciphCtx, ctx->iv, ctx->last, GCM_BLOCKSIZE);
        DATA64_XOR(dataIn, ctx->last, dataOut, GCM_BLOCKSIZE);
        /**
        * NIST_800-38D-7.1
        * INC32
        */
        ctr++;
        PUT_UINT32_BE(ctr, ctx->iv, 12); // 偏移12字节，取最后四字节
        // 刷新剩余长度
        blockLen -= GCM_BLOCKSIZE;
        // 偏移
        dataIn += GCM_BLOCKSIZE;
        dataOut += GCM_BLOCKSIZE;
    }
    if (enc) {
        GcmHashMultiBlock(ctx->ghash, ctx->hTable, out, len);
    }
    // 敏感信息清除
    SAL_CleanseData(&ctr, sizeof(uint32_t));
}

// enc标识为true时为加密操作，false为解密操作
static int32_t MODES_GCM_Crypt(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, bool enc)
{
    if (ctx == NULL || in == NULL || out == NULL || len == 0) {
        return CRYPT_NULL_INPUT;
    }
    int32_t ret = CryptLenCheckAndRefresh(ctx, len);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    uint32_t lastLen = LastHandle(ctx, in, out, len, enc);
    // 数据处理完毕，提前退出
    if (lastLen == len) {
        return CRYPT_SUCCESS;
    }

    XorCryptData data;
    data.in = in + lastLen;
    data.out = out + lastLen;
    data.ctr = ctx->last;
    data.ct = ctx->remCt;

    uint32_t multiBlockLen = (len - lastLen) & GCM_BLOCK_MASK;
    if (multiBlockLen > 0) {
        GcmMultiBlockCrypt(ctx, data.in, data.out, multiBlockLen, enc);
        data.in += multiBlockLen;
        data.out += multiBlockLen;
    }
    uint32_t remLen = len - lastLen - multiBlockLen;
    if (remLen > 0) {
        uint32_t ctr = GET_UINT32_BE(ctx->iv, 12); // 计数信息，iv的最后32位，需偏移12字节(16 - 4 = 12)
        (void)ctx->ciphMeth->encrypt(ctx->ciphCtx, ctx->iv, ctx->last, GCM_BLOCKSIZE);
        if (enc) {
            XorInEncrypt(&data, remLen);
        } else {
            XorInDecrypt(&data, remLen);
        }
        /**
         * NIST_800-38D-7.1
         * INC32
         */
        ctr++;
        PUT_UINT32_BE(ctr, ctx->iv, 12); // 偏移12字节，取最后四字节
        // 敏感信息清除
        SAL_CleanseData(&ctr, sizeof(uint32_t));
    }
    ctx->lastLen = (remLen > 0) ? (GCM_BLOCKSIZE - remLen) : 0;

    return CRYPT_SUCCESS;
}

static void GcmPad(MODES_GCM_Ctx *ctx)
{
    // S = GHASHH (A || 0v || C || 0u || [len(A)]64 || [len(C)]64).
    if (ctx->lastLen != 0) {
        uint32_t offset = GCM_BLOCKSIZE - ctx->lastLen;
        (void)memset_s(ctx->remCt + offset, GCM_BLOCKSIZE - offset, 0, ctx->lastLen);
        GcmHashMultiBlock(ctx->ghash, ctx->hTable, ctx->remCt, GCM_BLOCKSIZE);
    }
    uint64_t aadLen = (uint64_t)(ctx->aadLen) << 3; // bitLen = byteLen << 3
    uint64_t plaintextLen = ctx->plaintextLen << 3; // bitLen = byteLen << 3
    uint8_t padBuf[GCM_BLOCKSIZE];
    Uint64ToBeBytes(aadLen, padBuf);
    Uint64ToBeBytes(plaintextLen, padBuf + 8); // 后64bit(8byte)为密文长度

    GcmHashMultiBlock(ctx->ghash, ctx->hTable, padBuf, GCM_BLOCKSIZE);
}

static int32_t SetTagLen(MODES_GCM_Ctx *ctx, const uint8_t *val, uint32_t len)
{
    if (val == NULL) {
        return CRYPT_NULL_INPUT;
    }
    if (len != sizeof(uint32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_MODES_CTRL_TAGLEN_ERROR);
        return CRYPT_MODES_CTRL_TAGLEN_ERROR;
    }
    /**
     * NIST_800-38D-5.2.1.2
     * The bit length of the tag, denoted t, is a security parameter, as discussed in Appendix B.
     * In general, t may be any one of the following five values: 128, 120, 112, 104, or 96. For certain
     * applications, t may be 64 or 32; guidance for the use of these two tag lengths, including
     * requirements on the length of the input data and the lifetime of the ciphCtx in these cases,
     * is given in Appendix C
     */
    uint32_t tagLen = *((const uint32_t *)val);
    // 32bit 即 4 byte 64bit 即 8 byte 128, 120, 112, 104, or 96 即 12 byte - 16 byte
    if (tagLen == 4 || tagLen == 8 || (tagLen >= 12 && tagLen <= 16)) {
        ctx->tagLen = (uint8_t)tagLen;
        return CRYPT_SUCCESS;
    }
    BSL_ERR_PUSH_ERROR(CRYPT_MODES_CTRL_TAGLEN_ERROR);
    return CRYPT_MODES_CTRL_TAGLEN_ERROR;
}

static int32_t GetTag(MODES_GCM_Ctx *ctx, uint8_t *val, uint32_t len)
{
    if (val == NULL) {
        return CRYPT_NULL_INPUT;
    }
    if (len != ctx->tagLen) {
        BSL_ERR_PUSH_ERROR(CRYPT_MODES_TAGLEN_ERROR);
        return CRYPT_MODES_TAGLEN_ERROR;
    }
    ctx->cryptCnt++; // 加解密流程结束 秘钥使用次数+1
    GcmPad(ctx);
    uint32_t i;
    for (i = 0; i < len; i++) {
        val[i] = ctx->ghash[i] ^ ctx->ek0[i];
    }
    return CRYPT_SUCCESS;
}

int32_t MODES_GCM_Encrypt(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len)
{
    return MODES_GCM_Crypt(ctx, in, out, len, true);
}

int32_t MODES_GCM_Decrypt(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len)
{
    return MODES_GCM_Crypt(ctx, in, out, len, false);
}

int32_t MODES_GCM_Ctrl(MODES_GCM_Ctx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len)
{
    if (ctx == NULL) {
        return CRYPT_NULL_INPUT;
    }
    switch (opt) {
        case CRYPT_CTRL_SET_IV:
            return SetIv(ctx, val, len);
        case CRYPT_CTRL_SET_TAGLEN:
            return SetTagLen(ctx, val, len);
        case CRYPT_CTRL_SET_AAD:
            return SetAad(ctx, val, len);
        case CRYPT_CTRL_GET_TAG:
            return GetTag(ctx, val, len);
        default:
            return CRYPT_MODES_CTRL_TYPE_ERROR;
    }
}
#endif