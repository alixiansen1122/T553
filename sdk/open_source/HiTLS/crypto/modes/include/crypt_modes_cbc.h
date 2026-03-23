/**
 * @defgroup    modes_cbc crypt_modes_cbc.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       modes句柄类型
 */

#ifndef CRYPT_MODES_CBC_H
#define CRYPT_MODES_CBC_H

#ifndef HITLS_CRYPTO_NO_CBC
#include <stdbool.h>
#include "crypt_modes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define CRYPT_SM4_BLOCKSIZE    16

typedef struct {
    uint8_t iv[CRYPT_SM4_BLOCKSIZE];
    uint32_t rk[32]; // sm4 秘钥扩展后为32长度的uint32
    bool safeMode; // 侧信道安全选项
} CRYPT_SM4_ASM_Ctx;

/**
 * @brief CBC模式设置加密密钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 加密密钥
 * @param len [IN] 加密密钥长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CBC_SetEncryptKey(MODE_CipherCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief CBC模式加密
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CBC_Encrypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief CBC模式解密
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param in [IN] 待加密数据
 * @param out [OUT] 已加密数据
 * @param len [IN] 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CBC_Decrypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 原语SM4 CBC模式加密块
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t SM4_CBC_EncryptBlock(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 原语SM4 CBC模式解密块
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t SM4_CBC_DecryptBlock(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 使用memset_s清理信息，仅供非认证算法使用
 *
 * @param [IN/OUT] ctx 模式句柄
 */
void MODES_SM4_CBC_Clean(MODE_CipherCtx *ctx);

/**
 * @brief SM4 CBC模式下，对mode进行参数操作
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param opt [IN] 操作
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_SM4_CBC_Ctrl(MODE_CipherCtx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif