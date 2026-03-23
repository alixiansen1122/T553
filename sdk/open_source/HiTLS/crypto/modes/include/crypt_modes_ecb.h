/**
 * @defgroup    modes_ecb crypt_modes_ecb.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       modes句柄类型
 */

#ifndef CRYPT_MODES_ECB_H
#define CRYPT_MODES_ECB_H

#ifndef HITLS_CRYPTO_NO_ECB
#include "crypt_modes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief ECB模式加密
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_ECB_Encrypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief ECB模式解密
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param in [IN] 待加密数据
 * @param out [OUT] 已加密数据
 * @param len [IN] 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_ECB_Decrypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief ECB模式下，对mode进行参数操作
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param opt [IN] 操作
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_ECB_Ctrl(MODE_CipherCtx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

/**
 * @brief 原语SM4 ECB模式加密块
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t SM4_ECB_EncryptBlock(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 原语SM4 ECB模式解密块
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t SM4_ECB_DecryptBlock(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 使用memset_s清理信息，仅供非认证算法使用
 *
 * @param [IN/OUT] ctx 模式句柄
 */
void MODES_SM4_ECB_Clean(MODE_CipherCtx *ctx);

/**
 * @brief SM4 ECB模式下，对mode进行参数操作
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param opt [IN] 操作
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_SM4_ECB_Ctrl(MODE_CipherCtx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif