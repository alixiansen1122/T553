/**
 * @defgroup    modes_xts crypt_modes_xts.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       modes句柄类型
 */

#ifndef CRYPT_MODES_XTS_H
#define CRYPT_MODES_XTS_H

#ifndef HITLS_CRYPTO_NO_XTS
#include "crypt_modes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/**
 * @brief 模块初始化，将加解密算法的method注册到模块中，并且开辟算法的上下文
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param method [IN] 对称加解密的方法
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_XTS_InitCtx(MODE_CipherCtx *ctx, EAL_CipherMethod *method);
/**
 * @brief XTS模式设置加密密钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 加密密钥
 * @param len [IN] 加密密钥长度，只支持32(256bit) 和 64(512bit)
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_XTS_SetEncryptKey(MODE_CipherCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief XTS模式设置解密密钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 解密密钥
 * @param len [IN] 解密密钥长度，只支持32(256bit) 和 64(512bit)
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_XTS_SetDecryptKey(MODE_CipherCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief XTS模式加密, 当len不是16的整数倍时，不能再调用MODE_XTS_Encrypt进行新数据的加密
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_XTS_Encrypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief XTS模式解密，当len不是16的整数倍时，本轮解密结束
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param in [IN] 待加密数据
 * @param out [OUT] 已加密数据
 * @param len [IN] 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_XTS_Decrypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 对xts mode进行参数操作
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param opt [IN] 操作
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_XTS_Ctrl(MODE_CipherCtx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif