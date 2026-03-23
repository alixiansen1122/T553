/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_local_types.h
 * Description: 算法操作语义抽象
 * Author: zhangqiankun
 * Create: 2022-2-14
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-2-14  zhangqiankun        创建算法操作语义抽象文件
 *  2022-2-16  zhangqiankun        修改MD抽象层内容
 */

#ifndef CRYPT_LOCAL_TYPES_H
#define CRYPT_LOCAL_TYPES_H

#include <stdint.h>
#include <stdlib.h>
#include "crypt_algid.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* MD算法操作接口原型 */
typedef int32_t (*MdInit)(void *data);
typedef int32_t (*MdUpdate)(void *data, const uint8_t *input, uint32_t len);
typedef int32_t (*MdFinal)(void *data, uint8_t *out, uint32_t *len);
typedef void (*MdDeinit)(void *data);
typedef int32_t (*MdCopyCtx)(void *dst, void *src);

typedef struct {
    uint16_t blockSize;            // HASH算法单次处理的blockSize，配合其他算法使用
    uint16_t mdSize;               // HASH算法的输出长度
    uint16_t ctxSize;              // HASH的上下文的size
    MdInit init;                   // 初始化MD上下文的密钥
    MdUpdate update;               // 添加MD计算的分块数据
    MdFinal final;                 // 完成MD计算，得到MD结果
    MdDeinit deinit;               // 清理MD上下文密钥信息
    MdCopyCtx copyCtx;             // 拷贝MD上下文
} EAL_MdMethod;

typedef struct {
    uint16_t hashSize;               // Siphash算法的输出长度
    uint16_t compressionRounds;     // 压缩轮数
    uint16_t finalizationRounds;    // 收尾轮数
} EAL_SiphashMethod;

/* 非对称原语方法提供 */
typedef void *(*PkeyNew)(void);
typedef void (*PkeyFree)(void *key);
typedef void *(*PkeyNewParaById)(int32_t id);
typedef void (*PkeyFreePara)(void *para);
typedef int32_t (*PkeySetPara)(void *key, const void *para);
typedef int32_t (*PkeyGetPara)(const void *key, void *para);
typedef int32_t (*PkeyGen)(void *key);
typedef uint32_t (*PkeyBits)(void *key);
typedef uint32_t (*PkeyGetSignLen)(void *key);
typedef int32_t (*PkeyCtrl)(void *key, CRYPT_PkeyCtrl opt, void *val, uint32_t len);
typedef int32_t (*PkeySetPrv)(void *key, const void *prv);
typedef int32_t (*PkeySetPub)(void *key, const void *pub);
typedef int32_t (*PkeyGetPrv)(const void *key, void *prv);
typedef int32_t (*PkeyGetPub)(const void *key, void *pub);
typedef void *(*PkeyNewPara)(const void *para);
typedef int32_t (*PkeySign)(const void *key, const uint8_t *data, uint32_t dataLen,
    uint8_t *sign, uint32_t *signLen);
typedef int32_t (*PkeyVerify)(const void *key, const uint8_t *data, uint32_t dataLen,
    const uint8_t *sign, uint32_t signLen);
typedef int32_t (*PkeyComputeShareKey)(const void *key, const void *pub,
    uint8_t *share, uint32_t *shareLen);
typedef int32_t (*PkeyCrypt)(const void *key, const uint8_t *data, uint32_t dataLen,
    uint8_t *out, uint32_t *outLen);
typedef int32_t (*PkeyCheck)(const void *key);


/**
* @ingroup  EAL
*
* eal的method结构体
*/
typedef struct {
    PkeyNew newCtx;                         // 申请秘钥对结构体资源
    PkeyFree freeCtx;                       // 秘钥结构体释放
    PkeySetPara setPara;                    // 在秘钥对结构体中设置参数
    PkeyGetPara getPara;                    // 在秘钥对结构体中获取参数
    PkeyGen gen;                            // 生成秘钥对
    PkeyBits bits;                          // 获取秘钥长
    PkeyGetSignLen signLen;                 // 获取签名数据长度
    PkeyCtrl ctrl;                          // 控制参数
    PkeyNewParaById newParaById;            // 通过参数ID生成参数
    PkeyFreePara freePara;                  // 释放秘钥参数
    PkeyNewPara newPara;                    // 生成秘钥参数
    PkeySetPub setPub;                      // 设置公钥
    PkeySetPrv setPrv;                      // 设置私钥
    PkeyGetPub getPub;                      // 获取公钥
    PkeyGetPrv getPrv;                      // 获取私钥
    PkeySign sign;                          // 签名
    PkeyVerify verify;                      // 验签
    PkeyComputeShareKey computeShareKey;    // 计算共享秘钥
    PkeyCrypt encrypt;                        // 加密
    PkeyCrypt decrypt;                      // 解密
    PkeyCheck check;                        // 检查密钥对一致性
} EAL_PkeyMethod;

/**
 * @ingroup  sym_algid
 * 对称加解密算法ID
 */
typedef enum {
    CRYPT_SYM_AES128 = 0,
    CRYPT_SYM_AES192,
    CRYPT_SYM_AES256,
    CRYPT_SYM_CHACHA20,
    CRYPT_SYM_SM4,
    CRYPT_SYM_TDES,
    CRYPT_SYM_MAX
} CRYPT_SYM_AlgId;

typedef struct EAL_CipherMethodLocal {
    /**
     * @ingroup crypt_type
     * @brief 初始化句柄,注册其他模块
     * @return 0表示成功，其他表示失败
     */
    int32_t (*initCtx)(void *ctx, const struct EAL_CipherMethodLocal *m);
    /**
     * @ingroup crypt_type
     * @brief 反初始化句柄，回到调用init后的状态， 包括释放内存
     * @return void
     */
    void (*deinitCtx)(void *ctx);
    /**
     * @ingroup crypt_type
     * @brief 清除密钥，敏感信息，但不释放内存，回到initCtx后的状态
     * @return void
     */
    void (*clean)(void *ctx);
    /**
     * @ingroup crypt_type
     * @brief 设置加密密钥和密钥长度
     * @return 0表示成功，其他表示失败
     */
    int32_t (*setEncryptKey)(void *ctx, const uint8_t *key, uint32_t len);
    /**
     * @ingroup crypt_type
     * @brief 设置解密密钥和密钥长度
     * @return 0表示成功，其他表示失败
     */
    int32_t (*setDecryptKey)(void *ctx, const uint8_t *key, uint32_t len);
    /**
     * @ingroup crypt_type
     * @brief 对输入数据进行加密，加解密的数据长度一致
     * @return 0表示成功，其他表示失败
     */
    int32_t (*encrypt)(void *ctx, const uint8_t *in, uint8_t *out, uint32_t len);
    /**
     * @ingroup crypt_type
     * @brief 对输入数据进行加密，加解密的数据长度一致
     * @return 0表示成功，其他表示失败
     */
    int32_t (*decrypt)(void *ctx, const uint8_t *in, uint8_t *out, uint32_t len);
    /**
     * @ingroup crypt_type
     * @brief 对ctx进行参数设置
     * @return 0表示成功，其他表示失败
     */
    int32_t (*ctrl)(void *ctx, uint32_t opt, void *val, uint32_t len);

    uint8_t blockSize;   /**< 块大小的字节数 */
    uint16_t ctxSize;   /**< ctx句柄大小，最大支持到65535字节 */
    CRYPT_SYM_AlgId algId;      /**< 算法ID */
} EAL_CipherMethod;

/* MAC算法操作接口原型 */
// 初始化内存，并设置方法
typedef int32_t (*MacInitCtx)(void *ctx, const void *method);
// 完成key初始化
typedef int32_t (*MacInit)(void *ctx, const uint8_t *key, uint32_t len);
typedef int32_t (*MacUpdate)(void *ctx, const uint8_t *in, uint32_t len);
typedef int32_t (*MacFinal)(void *ctx, const uint8_t *out, uint32_t *len);
typedef void    (*MacDeinit)(void *ctx);
// 与initCtx接口动作相反。清理敏感数据
typedef void    (*MacDeinitCtx)(void *ctx);
typedef void    (*MacReinit)(void *ctx);
typedef uint32_t (*MacGetMacLen)(void *ctx);

/* MAC算法操作方法集合 */
typedef struct {
    MacInitCtx initCtx;       // 申请内存，初始化并设置方法
    MacInit init;             // 初始化MAC上下文的密钥
    MacUpdate update;         // 添加MAC计算的分块数据
    MacFinal final;           // 完成MAC计算，得到MAC结果
    MacDeinit deinit;         // 清理MAC上下文密钥信息
    MacDeinitCtx deinitCtx;   // 清理敏感数据并释放内存
    MacReinit reinit;         // 重新初始化MAC上下文中的密钥，用于多次MAC计算密钥相同的场景
    MacGetMacLen getLen;      // 获取MAC计算结果数据长度
    uint16_t ctxSize;         // MAC 算法上下文大小
} EAL_MacMethod;

typedef struct {
    union {
        const EAL_MacMethod *macMethod;
        const EAL_CipherMethod *modeMethod; // gmac 依赖对称算法的方法
        const void *masMeth;    // 主算法的方法指针
    };
    union {
        const EAL_MdMethod *md;        // hmac 依赖md算法
        const EAL_CipherMethod *ciph;  // cmac 依赖aes接口
        const EAL_SiphashMethod *sip;  // siphash method
        const void *depMeth;           // 依赖算法指针，预留用于拓展
    };
} EAL_MacMethLookup;

/**
 * @ingroup  mode_algid
 * 对称加解密模式ID
 */
typedef enum {
    CRYPT_MODE_CBC = 0,
    CRYPT_MODE_CTR,
    CRYPT_MODE_ECB,
    CRYPT_MODE_XTS,
    CRYPT_MODE_CCM,
    CRYPT_MODE_GCM,
    CRYPT_MODE_CHACHA20_POLY1305,
    CRYPT_MODE_CFB,
    CRYPT_MODE_OFB,
    CRYPT_MODE_MAX
} CRYPT_MODE_AlgId;

/**
 * @ingroup crypt_eal_pkey
 *
 * RSA用于签名时，PSS填充方式的结构体
 */
typedef struct {
    int32_t saltLen;        /**< pss 盐值长度，-1表示hashLen，-2表示MaxLen */
    const EAL_MdMethod *mdMeth;    /**< pss 填充时的mdid */
    const EAL_MdMethod *mgfMeth;   /**< pss 填充时的mgfid */
} RSA_PadingPara;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EAL_LOCAL_TYPES_H
