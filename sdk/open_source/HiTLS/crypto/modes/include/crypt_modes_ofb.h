/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_modes_ofb.h
 * Description: ofb模式
 * Author: chenrenhao
 * Create: 2022-10-08
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-10-08  chenrenhao       ofb模式头文件
 */

#ifndef CRYPT_MODES_OFB_H
#define CRYPT_MODES_OFB_H

#ifndef HITLS_CRYPTO_NO_OFB
#include "crypt_modes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**
 * @brief OFB模式加/解密，可对任意字节长度进行加/解密
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_OFB_Crypt(MODE_CipherCtx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif