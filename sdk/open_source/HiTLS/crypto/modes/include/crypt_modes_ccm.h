/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_modes_ccm.h
 * Description: ccm模式
 * Author: linkaixin
 * Create: 2022-05-17
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-05-17  linkaixin        ccm模式头文件
 */

#ifndef CRYPT_MODES_CCM_H
#define CRYPT_MODES_CCM_H

#ifndef HITLS_CRYPTO_NO_CCM
#include <stdint.h>
#include "crypt_local_types.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define CCM_BLOCKSIZE 16

typedef struct {
    void *ciphCtx;  /* 每种算法自已定义的key  */
    const EAL_CipherMethod *ciphMeth;  /* 对应底层的加解密, 操作keyctx */
    uint8_t tagLen;    // tag长度 默认为16 在每次设置秘钥时进行重置

    uint8_t nonce[CCM_BLOCKSIZE];  // nonce数据，ctr加密数据
    uint8_t tag[CCM_BLOCKSIZE];    // tag数据，cbc加密中间数据
    uint8_t last[CCM_BLOCKSIZE];   // ctr模式下上一块数据
    uint64_t msgLen;    // 消息长度
    uint8_t lastLen;    // ctr模式下上一块数据未使用数据长度
    uint8_t tagInit;    // 标识tag是否做了初始化
} MODES_CCM_Ctx;

int32_t MODES_CCM_InitCtx(MODES_CCM_Ctx *ctx, const struct EAL_CipherMethodLocal *m);

void MODES_CCM_DeinitCtx(MODES_CCM_Ctx *ctx);

void MODES_CCM_Clean(MODES_CCM_Ctx *ctx);

int32_t MODES_CCM_Ctrl(MODES_CCM_Ctx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

int32_t MODES_CCM_SetKey(MODES_CCM_Ctx *ctx, const uint8_t *key, uint32_t len);

int32_t MODES_CCM_Encrypt(MODES_CCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

int32_t MODES_CCM_Decrypt(MODES_CCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
#endif
