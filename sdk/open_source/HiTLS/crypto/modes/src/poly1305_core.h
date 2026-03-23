/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file poly1305_core.h
 * Description: poly1305内部头文件
 * Author: pandeguang
 * Create: 2022-06-27
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-06-27  pandeguang      poly1305内部算法头文件
 */
#ifndef POLY1305_CORE_H
#define POLY1305_CORE_H

#include "crypt_modes_chacha20poly1305.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define POLY1305_BLOCKSIZE 16
#define POLY1305_TAGSIZE   16
#define POLY1305_KEYSIZE   32

void Poly1305InitForAsm(Poly1305Ctx *ctx);
uint32_t Poly1305Block(Poly1305Ctx *ctx, const uint8_t *data, uint32_t dataLen, uint32_t padbit);
void Poly1305Last(Poly1305Ctx *ctx, uint8_t mac[POLY1305_TAGSIZE]);
void Poly1305CleanRegister(void);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif