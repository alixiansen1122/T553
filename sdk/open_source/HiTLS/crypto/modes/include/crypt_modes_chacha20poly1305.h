/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_modes_chacha20poly1305.h
 * Description: chacha20_poly1305模式
 * Author: linkaixin
 * Create: 2022-05-05
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-05-05  linkaixin        chacha20_poly1305模式头文件
 */

#ifndef CRYPT_MODES_CHACHA20POLY1305_H
#define CRYPT_MODES_CHACHA20POLY1305_H

#ifndef HITLS_CRYPTO_NO_CHACHA20POLY1305
#include <stdint.h>
#include "crypt_local_types.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    uint32_t acc[6];  // acc中间数据 需大于130bit
    uint32_t r[4];    // 秘钥信息r 16字节 即4 * sizeof(uint32_t)
    uint32_t s[4];    // 秘钥信息s 16字节 即4 * sizeof(uint32_t)
    uint32_t table[36]; // 用于汇编计算加速的表
    uint8_t last[16]; // 上一次未处理数据缓存 一个block16字节
    uint32_t lastLen; // 上一次数据还有剩下的长度 字节长度
    uint32_t flag;    // 用于保存汇编状态信息
} Poly1305Ctx;

typedef struct {
    void *key; // 方法用的句柄
    const EAL_CipherMethod *method; // 算法方法
    Poly1305Ctx polyCtx;
    uint64_t aadLen; // 状态，标识数据是否设置
    uint64_t cipherTextLen; // 状态，标识数据是否设置
} MODES_CHACHA20POLY1305_Ctx;

int32_t MODES_CHACHA20POLY1305_InitCtx(MODES_CHACHA20POLY1305_Ctx *ctx, const struct EAL_CipherMethodLocal *m);

void MODES_CHACHA20POLY1305_DeinitCtx(MODES_CHACHA20POLY1305_Ctx *ctx);

void MODES_CHACHA20POLY1305_Clean(MODES_CHACHA20POLY1305_Ctx *ctx);

int32_t MODES_CHACHA20POLY1305_Ctrl(MODES_CHACHA20POLY1305_Ctx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

int32_t MODES_CHACHA20POLY1305_SetEncryptKey(MODES_CHACHA20POLY1305_Ctx *ctx, const uint8_t *key, uint32_t len);

int32_t MODES_CHACHA20POLY1305_SetDecryptKey(MODES_CHACHA20POLY1305_Ctx *ctx, const uint8_t *key, uint32_t len);

int32_t MODES_CHACHA20POLY1305_Encrypt(MODES_CHACHA20POLY1305_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

int32_t MODES_CHACHA20POLY1305_Decrypt(MODES_CHACHA20POLY1305_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif
