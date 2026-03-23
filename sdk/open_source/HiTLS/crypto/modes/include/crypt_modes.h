/**
 * @defgroup    modes crypt_modes.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       modes句柄类型
 */
#ifndef CRYPT_MODES_H
#define CRYPT_MODES_H

#include <stdint.h>
#include "crypt_eal_cipher.h"
#include "crypt_types.h"
#include "crypt_algid.h"
#include "crypt_local_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MODES_MAX_IV_LENGTH 24
#define MODES_MAX_BUF_LENGTH 24
#define DES_BLOCK_BYTE_NUM 8
/**
 * @ingroup crypt_mode_cipherctx
 *  模式句柄
 */
typedef struct {
    void *ciphCtx;  /* 每种算法自已定义的key  */
    const EAL_CipherMethod *ciphMeth;  /* 对应底层的加解密, 操作keyctx */
    uint8_t iv[MODES_MAX_IV_LENGTH]; /* 长度为blocksize */
    uint8_t buf[MODES_MAX_BUF_LENGTH]; /* 缓存上一块的信息 */
    uint8_t blockSize;     /* 保存块大小长度 */
    uint8_t offset;   /* ctr、ofb模式下使用，若offset>0，iv的[0, offset-1]表示已使用过的数据，
                         [offset, blockSize-1]表示未使用过的数据 */
    uint8_t flag3Iv;       /* 是否为3个IV的标识 */
    uint32_t ivIndex;       /* 表示使用第几块IV，TDES可能会有3个IV */
    CRYPT_SYM_AlgId algId;  /* 对称算法ID */
} MODE_CipherCtx;

/**
 * @brief 模块初始化，将加解密算法的method注册到模块中，并且开辟算法的上下文
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param method [IN] 对称加解密的方法
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_InitCtx(MODE_CipherCtx *ctx, const EAL_CipherMethod *method);

/**
 * @brief 模块反初始化，解除与算法模块的关系，并且释放算法的上下文
 *
 * @param ctx [IN] 模式句柄
 * @param method [IN] 对称加解密的方法
 */
void MODE_DeInitCtx(MODE_CipherCtx *ctx);

/**
 * @brief 设置加密密钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 加密密钥
 * @param len [IN] 加密密钥长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_SetEncryptKey(MODE_CipherCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief 设置解密密钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 解密密钥
 * @param len [IN] 解密密钥长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_SetDecryptKey(MODE_CipherCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief 对mode进行参数操作
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param opt [IN] 操作
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_Ctrl(MODE_CipherCtx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

/**
 * @brief 获取分组大小
 *
 * @param ctx [IN] 模式句柄
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t GetBlockSize(MODE_CipherCtx *ctx, uint32_t *val, uint32_t len);

/**
 * @brief 模式内容清理，清除敏感数据，保留算法模块的内存以及方法
 *
 * @param ctx [IN] 模式句柄
 * @return 无
 */
void MODE_Clean(MODE_CipherCtx *ctx);

int32_t MODE_SetIv(MODE_CipherCtx *ctx, uint8_t *val, uint32_t len);
int32_t MODE_GetIv(MODE_CipherCtx *ctx, uint8_t *val, uint32_t len);

/**
 * @brief SM4设置秘钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 秘钥
 * @param len [IN] 秘钥的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t SM4_SetKey(MODE_CipherCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief SM4 deinit模式上下文
 *
 * @param ctx [IN/OUT] 模式句柄
 */
void SM4_DeInitModeCtx(MODE_CipherCtx *ctx);

// 从counter开始， 长度为counterLen的计数器
static inline void IncCounter(uint8_t *counter, uint32_t counterLen)
{
    uint32_t i = counterLen;
    uint16_t carry = 1;

    while (i > 0) {
        i--;
        carry += counter[i];
        counter[i] = carry & (0xFFu);
        carry >>= 8;  // 取高8位
    }
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif