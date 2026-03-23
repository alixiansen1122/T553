/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_modes_cfb.h
 * Description: cfb模式
 * Author: chenrenhao
 * Create: 2022-10-08
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-10-08  chenrenhao       cfb模式头文件
 */

#ifndef CRYPT_MODES_CFB_H
#define CRYPT_MODES_CFB_H

#ifndef HITLS_CRYPTO_NO_CFB
#include "crypt_modes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    MODE_CipherCtx *modeCtx;    /* 存放指向MODE_CipherCtx的指针 */
    uint8_t feedbackBits;  /* 保存FeedBack长度 */
    uint8_t cipherCache[3][DES_BLOCK_BYTE_NUM];     /* 3IV的TDES用，缓存前3块密文用于反馈 */
    uint8_t cacheIndex;   /* 3IV的TDES用，当前使用哪一块缓存 */
} MODE_CFB_Ctx;

/**
 * @brief 模块初始化，将加解密算法的method注册到模块中，并且开辟算法的上下文
 *
 * @param ctx [IN] 模式句柄
 * @param method [IN] 对称加解密的方法
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CFB_InitCtx(MODE_CFB_Ctx *ctx, const EAL_CipherMethod *method);

/**
 * @brief CFB模式加密，可对任意字节进行加密，包含1-bit/8-bit/64-bit/128-bit CFB
 *
 * @param [IN] ctx 模式句柄
 * @param [IN] in 待加密数据
 * @param [OUT] out 已加密数据
 * @param [IN] len 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CFB_Encrypt(MODE_CFB_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief CFB模式解密，可对任意字节进行解密，包含1-bit/8-bit/64-bit/128-bit CFB
 *
 * @param ctx [IN] 模式句柄
 * @param in [IN] 待解密数据
 * @param out [OUT] 已解密数据
 * @param len [IN] 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CFB_Decrypt(MODE_CFB_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

/**
 * @brief 对mode进行参数操作
 *
 * @param ctx [IN] 模式句柄
 * @param opt [IN] 操作 (Set/Get IV; Set/Get FeedbackSize)
 * @param val [IN/OUT] 参数，可以是入参也可以是出参
 * @param len [IN] 参数的长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CFB_Ctrl(MODE_CFB_Ctx *ctx, CRYPT_CipherCtrl opt, void *val, uint32_t len);

/**
 * @brief CFB模式内容清理，清除敏感数据，保留算法模块的内存以及方法
 *
 * @param ctx [IN] 模式句柄
 * @return 无
 */
void MODE_CFB_Clean(MODE_CFB_Ctx *ctx);

/**
 * @brief CFB模式设置加密密钥
 *
 * @param ctx [IN/OUT] 模式句柄
 * @param key [IN] 加密密钥
 * @param len [IN] 加密密钥长度
 * @return 成功返回：CRYPT_SUCCESS
 *         失败返回：其他错误码
 */
int32_t MODE_CFB_SetEncryptKey(MODE_CFB_Ctx *ctx, const uint8_t *key, uint32_t len);

/**
 * @brief 模块反初始化，解除与算法模块的关系，并且释放算法的上下文
 *
 * @param ctx [IN] 模式句柄
 * @param method [IN] 对称加解密的方法
 */
void MODE_CFB_DeInitCtx(MODE_CFB_Ctx *ctx);

/**
 * @brief CFB模式对bit进行加解密的接口（内部接口，仅测试用）
 */
int32_t MODE_CFB_BitCrypt(MODE_CFB_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, bool enc);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif