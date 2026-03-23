/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file asm_sm4.h
 * Description: 汇编SM4算法头文件
 * Author: panxinrong
 * Create: 2022-08-16
 */

#ifndef ASM_SM4_H
#define ASM_SM4_H

#include <stdint.h>
#include "crypt_types.h"
#include "crypt_modes_cbc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

/**
 * @brief SM4 cbc设置加解密秘钥
 *
 * @param [IN] ctx sm4上下文
 * @param [IN] key 秘钥
 * @param [IN] keyLen 秘钥长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t CRYPT_SM4_ASM_SetKey(CRYPT_SM4_ASM_Ctx *ctx, const uint8_t *key, uint32_t keyLen);

/**
 * @brief SM4上下文清零
 *
 * @param [IN] ctx sm4上下文
 */
void CRYPT_SM4_ASM_Clean(CRYPT_SM4_ASM_Ctx *ctx);

/**
 * @brief sm4 CBC模式加密，使用前需要用Ctrl接口设置iv，数据长度必须是16整数倍
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] length 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t CRYPT_SM4_CbcDecrypt(CRYPT_SM4_ASM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t length);

/**
 * @brief sm4 CBC模式解密，使用前需要用Ctrl接口设置iv，数据长度必须是16整数倍
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] in 待解密数据
 * @param [OUT] out 已解密数据
 * @param [IN] length 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t CRYPT_SM4_CbcEncrypt(CRYPT_SM4_ASM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t length);

/**
 * @brief sm4 CBC模式设置获取iv接口
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] opt 模式，支持CRYPT_CTRL_SET_IV和CRYPT_CTRL_GET_IV
 * @param [IN/OUT] 设置的iv/获取的iv
 * @param [IN] len iv长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t CRYPT_SM4_Ctrl(CRYPT_SM4_ASM_Ctx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

/**
 * @brief sm4 ECB模式加密，数据长度必须是16整数倍
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] length 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t CRYPT_SM4_EcbEncrypt(CRYPT_SM4_ASM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t length);

/**
 * @brief sm4 ECB模式解密，数据长度必须是16整数倍
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] length 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t CRYPT_SM4_EcbDecrypt(CRYPT_SM4_ASM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t length);

void Sm4CbcDecrypt_80(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_96(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_112(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_128(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_144(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_160(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_176(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_192(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecrypt_64(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcDecryptConsTime_64(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcEncrypt(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4CbcEncryptConsTime(uint32_t *key, uint8_t *iv, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4SetKey(const uint8_t *key, uint32_t rk[32]);

void Sm4Decrypt(uint32_t *key, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4DecryptConsTime(uint32_t *key, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4Encrypt(uint32_t *key, const uint8_t *in, uint8_t *out, uint32_t len);

void Sm4EncryptConsTime(uint32_t *key, const uint8_t *in, uint8_t *out, uint32_t len);


#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif