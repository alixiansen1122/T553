/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_hkdf.h
 * Description: hkdf算法原语实现
 * Author: liushaoming
 * Create: 2022-05-11
 */

#ifndef CRYPT_HKDF_H
#define CRYPT_HKDF_H

#include <stdint.h>
#include "crypt_local_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief hkdf密钥派生算法
 *
 * @param macMeth [in] hmac算法method，仅支持HMAC方法
 * @param mdMeth [in] md算法method
 * @param key [IN] 密码，用户输入的字符串
 * @param keyLen [IN] 密码长度，任意长度，包括0长度。
 * @param salt [IN] 盐值，用户输入的字符串
 * @param saltLen [IN] 盐值长度，任意长度，包括0长度。
 * @param info [in] 额外信息
 * @param infoLen [in] 额外信息长度，任意长度，包括0长度。
 * @param out [OUT] 派生密钥
 * @param outLen [IN] 派生密钥长度，不为0的任意整数，其中最大值：
 *                      CRYPT_MAC_HMAC_SHA1对应最大值为5100、
 *                      CRYPT_MAC_HMAC_SHA224对应最大值为7140、
 *                      CRYPT_MAC_HMAC_SHA256对应最大值为8160、
 *                      CRYPT_MAC_HMAC_SHA384对应最大值为12240、
 *                      CRYPT_MAC_HMAC_SHA512对应最大值为16320、
 *
 * @return  CRYPT_OK 成功
 *          其他错误码参考 ht_error.h
 */

int32_t CRYPT_HKDF(const EAL_MacMethod *macMeth, const EAL_MdMethod *mdMeth, const uint8_t *key, uint32_t keyLen,
    const uint8_t *salt, uint32_t saltLen, const uint8_t *info, uint32_t infoLen, uint8_t *out, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
