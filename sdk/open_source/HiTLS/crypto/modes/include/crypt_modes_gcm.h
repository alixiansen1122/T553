/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_modes_gcm.h
 * Description: gcm模式
 * Author: linkaixin
 * Create: 2022-05-13
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-05-13  linkaixin        gcm模式头文件
 */

#ifndef CRYPT_MODES_GCM_H
#define CRYPT_MODES_GCM_H

#ifndef HITLS_CRYPTO_NO_GCM
#include <stdint.h>
#include "crypt_local_types.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define GCM_BLOCKSIZE 16

typedef struct {
    uint64_t h;
    uint64_t l;
} MODES_GCM_GF128;

typedef struct {
    // 该信息可一次设置，多次使用
    uint8_t iv[GCM_BLOCKSIZE];      // 经过处理后的iv信息，长度为16字节
    uint8_t ghash[GCM_BLOCKSIZE];   // tag计算的中间数据
    MODES_GCM_GF128 hTable[16]; // 窗口使用4bit 需要预计算2^4(16)个表项
    void *ciphCtx; // 方法用的句柄
    const EAL_CipherMethod *ciphMeth; // 算法方法
    /**
     * tagLen may be any one of the following five values: 128, 120, 112, 104, or 96
     * For certain applications, tagLen may be 64 or 32
     */
    uint8_t tagLen;
    uint32_t cryptCnt; // 秘钥设置可用的加密次数

    // 加解密中间信息，生命周期为一次加密操作，在每次加解密操作需要重置
    uint8_t last[GCM_BLOCKSIZE];    // ctr mode last
    uint8_t remCt[GCM_BLOCKSIZE];     // 剩余密文
    uint8_t ek0[GCM_BLOCKSIZE];     // ek0
    uint64_t plaintextLen;  // use for calc tag
    uint32_t aadLen;        // use for calc tag
    uint32_t lastLen;       // ctr mode lastLen
} MODES_GCM_Ctx;

int32_t MODES_GCM_InitCtx(MODES_GCM_Ctx *ctx, const struct EAL_CipherMethodLocal *m);

void MODES_GCM_DeinitCtx(MODES_GCM_Ctx *ctx);

void MODES_GCM_Clean(MODES_GCM_Ctx *ctx);

int32_t MODES_GCM_Ctrl(MODES_GCM_Ctx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

int32_t MODES_GCM_SetKey(MODES_GCM_Ctx *ctx, const uint8_t *ciphCtx, uint32_t len);

int32_t MODES_GCM_Encrypt(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

int32_t MODES_GCM_Decrypt(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

int32_t AES_GCM_EncryptBlock(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

int32_t AES_GCM_DecryptBlock(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
#endif
