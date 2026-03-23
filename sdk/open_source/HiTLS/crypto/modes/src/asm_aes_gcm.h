/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file asm_aes_gcm.h
 * Description: aes_gcm 汇编优化头文件
 * Author: haoruixiang
 * Create: 2022-06-27
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-06-27  haoruixiang        aes_gcm 汇编优化头文件
 */
#ifndef ASM_AES_GCM_H
#define ASM_AES_GCM_H
 
#include "crypt_modes_gcm.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void AES_GCM_EncryptBlockAsm(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, void *key);
void AES_GCM_DecryptBlockAsm(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, void *key);
void AES_GCM_Encrypt16BlockAsm(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, void *key);
void AES_GCM_Decrypt16BlockAsm(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, void *key);
void AES_GCM_ClearAsm(void);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif