/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_encode.h
 * Description: dsa中der-asn1相关的编解码接口
 * Author: linkaixin
 * Create: 2022-3-21
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-3-21   linkaixin        dsa中der-asn1相关的编解码接口
 */
#ifndef CRYPT_ENCODE_H
#define CRYPT_ENCODE_H
#if defined(HITLS_CRYPTO_DSA) || defined(HITLS_CRYPTO_SM2) \
    || defined(HITLS_CRYPTO_SM9) || defined(HITLS_CRYPTO_ECDSA)
#include "crypt_bn.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#define DSA_MIN_PBITS 1024 // DSA最小规格1024bit
#define DSA_MAX_PBITS 3072 // DSA最大规格3072bit
#define DSA_MIN_QBITS 160  // DSA参数q最小规格

typedef struct {
    BN_BigNum *r;
    BN_BigNum *s;
} DSA_Sign;
// 签名数据编码
int32_t SignDataEncode(const DSA_Sign *s, uint8_t *sign, uint32_t *signLen);

// 签名数据解码
int32_t SignDataDecode(DSA_Sign *s, const uint8_t *sign, uint32_t signLen);

// 获取签名数据所需长度
uint32_t SignEnCodeLen(uint32_t rLen, uint32_t sLen);

// 大数进行编码时的流长度
uint32_t SignStringLenOfBn(const BN_BigNum *num);

#ifdef HITLS_CRYPTO_SM9
int32_t SignDataEncodeByString(uint8_t *sign, uint32_t *signLen,
    const uint8_t *h, uint32_t hLen, const uint8_t *s, uint32_t sLen);

int32_t SignDataDecode2String(const uint8_t *sign, uint32_t signLen,
    uint8_t *h, uint32_t *hLen, uint8_t *s, uint32_t *sLen);
#endif

#ifdef __cplusplus
}
#endif
#endif
#endif