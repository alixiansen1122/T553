/**
 * @defgroup    crypt_aes  crypt_aes.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       AES加解密接口
 */
#ifndef CRYPT_AES_H
#define CRYPT_AES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define CRYPT_AES_MAX_ROUNDS  14
#define CRYPT_AES_MAX_KEYLEN  (4 * (CRYPT_AES_MAX_ROUNDS + 1))

/**
 * @ingroup CRYPT_AES_Key
 *
 * aes 密钥结构体
 */
typedef struct {
    uint32_t key[CRYPT_AES_MAX_KEYLEN];
    uint32_t rounds;
} CRYPT_AES_Key;

/**
 * @ingroup aes
 * @brief 设置AES加密秘钥。
 *
 * @param ctx [IN]  AES 句柄
 * @param key [IN]  加密密钥
 * @param len [IN]  秘钥长度，为128。
*/
int32_t CRYPT_AES_SetEncryptKey128(CRYPT_AES_Key *ctx, const uint8_t *key, uint32_t len);
/**
 * @ingroup aes
 * @brief 设置AES加密秘钥。
 *
 * @param ctx [IN]  AES 句柄
 * @param key [IN]  加密密钥
 * @param len [IN]  秘钥长度，为192。
*/
int32_t CRYPT_AES_SetEncryptKey192(CRYPT_AES_Key *ctx, const uint8_t *key, uint32_t len);
/**
 * @ingroup aes
 * @brief 设置AES加密秘钥。
 *
 * @param ctx [IN]  AES 句柄
 * @param key [IN]  加密密钥
 * @param len [IN]  秘钥长度，为256。
*/
int32_t CRYPT_AES_SetEncryptKey256(CRYPT_AES_Key *ctx, const uint8_t *key, uint32_t len);
/**
 * @ingroup aes
 * @brief 设置AES解密秘钥。
 *
 * @param ctx [IN]  AES 句柄
 * @param key [IN] 解密密钥
 * @param len [IN]  秘钥长度，为128
*/
int32_t CRYPT_AES_SetDecryptKey128(CRYPT_AES_Key *ctx, const uint8_t *key, uint32_t len);
/**
 * @ingroup aes
 * @brief 设置AES解密秘钥。
 *
 * @param ctx [IN]  AES 句柄
 * @param key [IN] 解密密钥
 * @param len [IN]  秘钥长度，为192
*/
int32_t CRYPT_AES_SetDecryptKey192(CRYPT_AES_Key *ctx, const uint8_t *key, uint32_t len);
/**
 * @ingroup aes
 * @brief 设置AES解密秘钥。
 *
 * @param ctx [IN]  AES 句柄
 * @param key [IN] 解密密钥
 * @param len [IN]  秘钥长度，可以为256。
*/
int32_t CRYPT_AES_SetDecryptKey256(CRYPT_AES_Key *ctx, const uint8_t *key, uint32_t len);
/**
 * @ingroup aes
 * @brief AES加密。
 *
 * @param ctx [IN]  AES句柄，存有密钥
 * @param in  [IN]  输入明文数据，需为16字节。
 * @param out [OUT] 输出密文数据，长度为16字节。
 * @param len [IN]  块长度。
*/
int32_t CRYPT_AES_Encrypt(const CRYPT_AES_Key *ctx, const uint8_t *in, uint8_t *out, uint32_t len);
/**
 * @ingroup aes
 * @brief AES解密。
 *
 * @param ctx [IN]  AES句柄，存有密钥
 * @param in  [IN]  输入密文数据，需为16字节。
 * @param out [OUT] 输出明文数据，长度为16字节。
 * @param len [IN]  块长度，长度为16。
*/
int32_t CRYPT_AES_Decrypt(const CRYPT_AES_Key *ctx, const uint8_t *in, uint8_t *out, uint32_t len);
/**
 * @ingroup aes
 * @brief 清除aes密钥信息
 *
 * @param ctx [IN]  AES句柄，存有密钥
 * @return void
*/
void CRYPT_AES_Clean(CRYPT_AES_Key *ctx);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif