/**
 * @defgroup    crypt_eal_kdf    crypt_eal_kdf.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       密钥派生(KDF)算法头文件
 */

#ifndef CRYPT_EAL_KDF_H
#define CRYPT_EAL_KDF_H

#ifndef HITLS_CRYPTO_NO_KDF

#include <stdbool.h>
#include <stdint.h>
#include "crypt_algid.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @ingroup crypt_eal_kdf
 * @brief   scrypt 基于密码的密钥派生函数
 *
 * @param   key [IN] 密码，用户输入的字符串
 * @param   keyLen [IN] 密码长度，任意长度，包括0长度
 * @param   salt [IN] 盐值，用户输入的字符串
 * @param   saltLen [IN] 盐值长度，任意长度，包括0长度
 * @param   n [IN] CPU和内存消耗参数，必须为2的幂次，大于1，小于2 ^ (128 * r / 8)
 * @param   r [IN] 块大小参数，不为0的任意正整数，其中 r * p < 2 ^ 30
 * @param   p [IN] 并行化参数，不为0的任意正整数，p <= (2 ^ 32 - 1) * 32 / (128 * r)
 * @param   out [OUT] 派生密钥，不能为空
 * @param   len [IN] 派生密钥长度，取值范围为 (0, 0xFFFFFFFF]
 *
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_Scrypt(const uint8_t *key, uint32_t keyLen, const uint8_t *salt, uint32_t saltLen, uint32_t n,
    uint32_t r, uint32_t p, uint8_t *out, uint32_t len);

/**
 * @ingroup crypt_eal_kdf
 * @brief   PBKDF 基于密码的密钥派生函数
 *
 * @param   id [IN] HMAC算法ID(仅支持CRYPT_MAC_HMAC_SHA1、CRYPT_MAC_HMAC_SHA224、
 *                  CRYPT_MAC_HMAC_SHA256、CRYPT_MAC_HMAC_SHA384、CRYPT_MAC_HMAC_SHA512)
 * @param   key [IN] 密码，用户输入的字符串
 * @param   keyLen [IN] 密码长度，任意长度，包括0长度。
 * @param   salt [IN] 盐值，用户输入的字符串
 * @param   saltLen [IN] 盐值长度，任意长度，包括0长度。
 * @param   it [IN] 迭代次数，可以为任意不为0的正整数，其中性能特殊场景可为1000，默认场景推荐10000，
 *                  性能不敏感或者高安全性要求场景推荐使用10000000。
 * @param   out [OUT] 派生密钥
 * @param   len [IN] 派生密钥长度，长度取值范围为 [1, 0xFFFFFFFF]
 *
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_Pbkdf2(CRYPT_MAC_AlgId id, const uint8_t *key, uint32_t keyLen, const uint8_t *salt,
    uint32_t saltLen, uint32_t it, uint8_t *out, uint32_t len);

/**
 * @ingroup crypt_eal_kdf
 * @brief   PKCS5 PBKDF 本接口为接口CRYPT_EAL_Pbkdf2的封装，默认使用算法CRYPT_MAC_HMAC_SHA256进行计算。
 *
 * @param   key [IN] 密码，用户输入的字符串
 * @param   keyLen [IN] 密码长度，不为0的任意正整数。
 * @param   salt [IN] 盐值，用户输入的字符串
 * @param   saltLen [IN] 盐值长度，不为0的任意正整数。
 * @param   it [IN] 迭代次数，可以为任意不为0的正整数，其中性能特殊场景可为1000，默认场景推荐10000，
 *                  性能不敏感或者高安全性要求场景推荐使用10000000。
 * @param   out [OUT] 派生密钥
 * @param   len [IN] 派生密钥长度，长度取值范围为 [1, 0xFFFFFFFF]
 *
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
#define CRYPT_EAL_PKCS5_PBKDF2(key, keyLen, salt, saltLen, it, out, len)  \
        CRYPT_EAL_Pbkdf2(CRYPT_MAC_HMAC_SHA256, key, keyLen, salt, saltLen, it, out, len)

/**
 * @ingroup crypt_eal_kdf
 * @brief   HKDF
 *
 * @param   id [IN] MAC算法ID(仅支持CRYPT_MAC_HMAC_SHA1、CRYPT_MAC_HMAC_SHA224、
 *                  CRYPT_MAC_HMAC_SHA256、CRYPT_MAC_HMAC_SHA384、CRYPT_MAC_HMAC_SHA512)
 * @param   key [IN] 密码，用户输入的字符串
 * @param   keyLen [IN] 密钥长度，任意长度
 * @param   salt [IN] 盐值，用户输入的字符串
 * @param   saltLen [IN] 盐值长度，任意长度
 * @param   info [IN] 用户可选的附加信息，可选信息。
 * @param   infoLen [IN] 附加信息长度，可以为0长度。[0,0xFFFFFFFF]
 * @param   out [OUT] 派生密钥
 * @param   len [IN] 派生密钥长度，不为0的任意整数，其中最大值：
 *                      CRYPT_MAC_HMAC_SHA1对应最大值为5100、
 *                      CRYPT_MAC_HMAC_SHA224对应最大值为7140、
 *                      CRYPT_MAC_HMAC_SHA256对应最大值为8160、
 *                      CRYPT_MAC_HMAC_SHA384对应最大值为12240、
 *                      CRYPT_MAC_HMAC_SHA512对应最大值为16320、
 *
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_Hkdf(CRYPT_MAC_AlgId id, const uint8_t *key, uint32_t keyLen, const uint8_t *salt, uint32_t saltLen,
    const uint8_t *info, uint32_t infoLen, uint8_t *out, uint32_t len);

/**
 * @ingroup crypt_eal_kdf
 * @brief   KDF-TLS1.2
 *
 * @param   id [IN] MAC算法ID(仅支持部分HMAC算法ID，
 *                  其中包括 CRYPT_MAC_HMAC_SHA256、CRYPT_MAC_HMAC_SHA384、CRYPT_MAC_HMAC_SHA512)
 * @param   key [IN] 密钥，用户输入的字符串
 * @param   keyLen [IN] 密钥长度，任意长度
 * @param   label [IN] 标签，用户输入的字符串。和seed拼接作为PRF的输入数据
 * @param   labelLen [IN] 标签长度，任意长度
 * @param   seed [IN] 种子，用户输入的字符串，用于作为PRF的输入数据。
 * @param   seedLen [IN] 种子长度，任意长度
 * @param   out [OUT] 派生密钥
 * @param   len [IN] 派生密钥长度，取值范围为 [1, 0xFFFFFFFF]
 *
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_KdfTls12(CRYPT_MAC_AlgId id, const uint8_t *key, uint32_t keyLen, const uint8_t *label,
    uint32_t labelLen, const uint8_t *seed, uint32_t seedLen,  uint8_t *out, uint32_t len);

/**
  * @ingroup crypt_eal_kdf
  * @brief scrypt合规判断
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_ScryptC2)(void);

/**
 * @ingroup crypt_eal_kdf
 * @brief   scrypt合规判断回调注册接口，用于判断模块状态是否可用
 *          CRYPT_EAL_Scrypt接口在执行操作前会调用该回调，回调失败会导致CRYPT_EAL_Scrypt失败
 *
 * @param   func     [IN] scrypt合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegScryptC2(CRYPT_EAL_ScryptC2 func);

/**
  * @ingroup crypt_eal_kdf
  * @brief pbkdf2合规判断
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_Pbkdf2C2)(uint32_t saltLen, uint32_t it, uint32_t len);

/**
 * @ingroup crypt_eal_kdf
 * @brief   pbkdf2合规判断回调注册接口，用于判断模块状态是否可用
 *          CRYPT_EAL_Pbkdf2接口在执行操作前会调用该回调，回调失败会导致CRYPT_EAL_Pbkdf2失败
 *
 * @param   func     [IN] pbkdf2合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegPbkdf2C2(CRYPT_EAL_Pbkdf2C2 func);

/**
  * @ingroup crypt_eal_kdf
  * @brief hkdf合规判断
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_HkdfC2)(void);

/**
 * @ingroup crypt_eal_kdf
 * @brief   hkdf合规判断回调注册接口，用于判断模块状态是否可用
 *          CRYPT_EAL_Hkdf接口在执行操作前会调用该回调，回调失败会导致CRYPT_EAL_Hkdf失败
 *
 * @param   func     [IN] hkdf合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegHkdfC2(CRYPT_EAL_HkdfC2 func);

/**
  * @ingroup crypt_eal_kdf
  * @brief KDF-TLS1.2合规判断
  *
  * @param   id [IN] MAC算法ID
  *                 非核准模式下可以使用任意算法
  *                 ISO19790模式下只允许CRYPT_MAC_HMAC_SHA256/CRYPT_MAC_HMAC_SHA384/CRYPT_MAC_HMAC_SHA512
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_KdfTls12C2)(CRYPT_MAC_AlgId id);

/**
 * @ingroup crypt_eal_kdf
 * @brief   KDF-TLS1.2合规判断回调注册接口，用于判断算法参数是否合规和模块状态是否可用
 *          CRYPT_EAL_KdfTls12接口在执行操作前会调用该回调，回调失败会导致CRYPT_EAL_KdfTls12失败
 *
 * @param   func     [IN] KDF-TLS1.2合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegKdfTls12C2(CRYPT_EAL_KdfTls12C2 func);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif
