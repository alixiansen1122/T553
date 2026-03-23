/**
 * @defgroup    crypt_check_macro crypt_check_macro.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       检查宏之间的依赖关系
 */

#ifndef CRYPT_CHECK_MACRO_H
#define CRYPT_CHECK_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if !defined(HITLS_CRYPTO_NO_PKEY) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_NO_PKEY not defined, but HITLS_CRYPTO_NO_MD defined."
#endif

#if defined(HITLS_CRYPTO_HMAC) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_HMAC defined, but HITLS_CRYPTO_NO_MD also defined."
#endif

#if !defined(HITLS_CRYPTO_NO_RAND) && !defined(HITLS_CRYPTO_NO_DRBG_HASH) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "Hash Func in eal-rand should be controlled by HITLS_CRYPTO_NO_DRBG_HASH, not by HITLS_CRYPTO_NO_MD."
#endif

#if defined(HITLS_CRYPTO_SHA1) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_SHA1 defined, but HITLS_CRYPTO_NO_MD also defined."
#endif

#if defined(HITLS_CRYPTO_SHA2) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_SHA2 defined, but HITLS_CRYPTO_NO_MD also defined."
#endif

#if defined(HITLS_CRYPTO_SHA3) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_SHA3 defined, but HITLS_CRYPTO_NO_MD also defined."
#endif

#if defined(HITLS_CRYPTO_MD5) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_MD5 defined, but HITLS_CRYPTO_NO_MD also defined."
#endif

#if defined(HITLS_CRYPTO_SM3) && \
    defined(HITLS_CRYPTO_NO_MD)
#error "HITLS_CRYPTO_SM3 defined, but HITLS_CRYPTO_NO_MD also defined."
#endif

#if !defined(HITLS_CRYPTO_NO_SHA224) && \
    defined(HITLS_CRYPTO_NO_SHA256)
#error "HITLS_CRYPTO_NO_SHA224 not defined, but HITLS_CRYPTO_NO_SHA256 also defined."
#endif

#if !defined(HITLS_CRYPTO_NO_SHA384) && \
    defined(HITLS_CRYPTO_NO_SHA512)
#error "HITLS_CRYPTO_NO_SHA384 not defined, but HITLS_CRYPTO_NO_SHA512 also defined."
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // CRYPT_CHECK_MACRO_H
