/**
 * @defgroup    modes_ctr crypt_modes_ctr.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       modes句柄类型
 */

#ifndef CRYPT_MODES_CTR_H
#define CRYPT_MODES_CTR_H

#ifndef HITLS_CRYPTO_NO_CTR
#include "crypt_modes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief CTR模式加密
 *
 * @param [IN/OUT] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CTR_Crypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif