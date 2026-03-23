/**
 * @defgroup    crypt_mpa_aead    crypt_mpa_aead.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       MPA多路并行计算对外接口文件
 */
#ifndef CRYPT_MPA_AEAD_H
#define CRYPT_MPA_AEAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup crypt_mpa_aead
 *
 * 加密认证计算顺序
 */
typedef enum {
    MPA_MTE, /**< hmac->aes */
    MPA_ETM, /**< aes->hmac */
} CRYPT_MPA_AeadOrder;

/**
 * @ingroup crypt_mpa_aead
 *
 * aes加密解密模式
 */
typedef enum {
    MPA_ENC, /**< 加密 */
    MPA_DEC, /**< 解密 */
} CRYPT_MPA_EncMode;

/**
 * @ingroup crypt_mpa_aead
 *
 * hmac 算法类型
 */
typedef enum {
    MPA_HASH_NONE, /**< 不使用hash 算法 */
    MPA_MD5,       /**< md5 算法(预留) */
    MPA_SHA1,      /**< sha1 算法(预留) */
    MPA_SHA224,    /**< sha224 算法(预留) */
    MPA_SHA256,    /**< sha256 算法 */
    MPA_SHA384,    /**< sha384 算法(预留) */
    MPA_SHA512     /**< sha512 算法 */
} CRYPT_MPA_HashAlg;

/**
 * @ingroup crypt_mpa_aead
 *
 * aes 算法类型
 */
typedef enum {
    MPA_AES_NONE,   /**< 不使用 aes 算法 */
    MPA_AES128_CBC, /**< aes128cbc 算法 */
    MPA_AES192_CBC, /**< aes192cbc 算法 */
    MPA_AES256_CBC  /**< aes256cbc 算法 */
} CRYPT_MPA_AesMode;

/**
 * @ingroup crypt_mpa_aead
 *
 * 基于流的、对外数据结构。设置单条流的具体信息。
 */
typedef struct {
    /* common */
    uint32_t jobId;             /**< job标记 */
    uint32_t aeadOrder;         /**< 认证加密模式：MPA_MTE、MPA_ETM */

    /* aes alg resource */
    const uint8_t *msgInput;    /**< aes进行计算的数据输入, 明文或密文 */
    uint8_t *msgOutput;         /**< aes计算结果, 密文或明文 */
    uint32_t aesMsgLen;         /**< aes计算的数据长度，长度为16字节的整数倍，不支持aes padding功能 */
    uint8_t aesMode;            /**< aes模式:MPA_AES_NONE, MPA_AES128_CBC, MPA_AES192_CBC, MPA_AES256_CBC */
    uint8_t encryptDecrypt;     /**< aes加密或解密: MPA_ENC, MPA_DEC */
    uint8_t res1[2];            /**< 预留 */
    const uint8_t *iv;          /**< aes初始向量输入 */
    const uint8_t *aesKey;      /**< aes密钥输入 */
    uint32_t keyLen;            /**< aes密钥长度，aes128:16字节；aes192:24字节；aes256:32字节 */
    uint32_t ivLen;             /**< aes初始向量长度，16字节 */
    uint8_t *expandKeyEnc;      /**< aes加密扩展密钥输入，aes128:11 * 16字节；aes192:13 * 16字节；aes256:15 * 16字节 */
    uint8_t *expandKeyDec;      /**< aes解密扩展密钥输入，aes128:11 * 16字节；aes192:13 * 16字节；aes256:15 * 16字节 */

    /* hmac-sha resource */
    const uint8_t *hmacKey;     /**< hmac密钥输入 */
    const uint8_t *msg;         /**< hmac计算，数据输入 */
    uint32_t hmacKeyLen;        /**< hmac密钥长度 */
    uint32_t msgLen;            /**< hmac计算的数据长度 */
    uint8_t *hashResult;        /**< hmac计算结果mac输出 */
    uint8_t hashAlg;            /**< 哈希算法选择：MPA_HASH_NONE, MPA_SHA256，MPA_SHA512 */
    uint8_t res2[3];            /**< 预留 */
    uint32_t hashResultLen;     /**< 输入参数，决定返回的mac数据长度；sha256：长度不大于32字节；sha512：长度不大于64字节 */
    const uint8_t *ipadKeyHash; /**< hmac ipadKey数据哈希值，没有提前计算ipadkey哈希值，赋值NULL */
    const uint8_t *opadKeyHash; /**< hmac opadKey数据哈希值，没有提前计算opadkey哈希值，赋值NULL */
    
    void *userData;             /**< 用户自定义使用 */
} CRYPT_MPA_Job;

typedef struct MGR_ CRYPT_MPA_Mgr;

/**
 * @ingroup crypt_mpa_aead
 * @brief 从mgr池子里面获取一个job
 * @attention 调用者需要保证入参mgr不为空
 * @param mgr [IN] 多流并行计算管理结构体，包含job池子
 * @return 成功返回job；失败返回NULL
 */
void *CRYPT_MPA_MallocJob(CRYPT_MPA_Mgr *mgr);

/**
 * @ingroup crypt_mpa_aead
 * @brief 提交job到调度层
 * @attention 调用者需要保证入参mgr和job不为空
 * @param mgr [IN] 多流并行计算管理结构体，包含job池子
 * @param job [IN] 多流并行调度单元(当前支持算法：hmac，aes，aes-hmac组合)
 * @return 有job计算完成，返回计算完成的job，用户需要自行释放job；无job计算完成则返回NULL
 */
void *CRYPT_MPA_CommitJob(CRYPT_MPA_Mgr *mgr, CRYPT_MPA_Job *job);

/**
 * @ingroup crypt_mpa_aead
 * @brief 初始化多流并行计算模块
 * @return 初始化成功返回0；初始化失败返回其它值
 */
uint32_t CRYPT_MPA_Init(void);

/**
 * @ingroup crypt_mpa_aead
 * @brief init mgr resource
 * @param mgr [IN] 多流并行计算管理结构体，包含job池子
 * @param order [IN] 指定加解密和认证的顺序
 * @return 初始化成功返回0；初始化失败返回其它值
 */
uint32_t CRYPT_MPA_InitMgr(CRYPT_MPA_Mgr *mgr, CRYPT_MPA_AeadOrder order);

/**
 * @ingroup crypt_mpa_aead
 * @brief 申请mgr资源
 * @return 成功返回mgr；失败返回NULL
 */
CRYPT_MPA_Mgr *CRYPT_MPA_MallocMgr(void);

/**
 * @ingroup crypt_mpa_aead
 * @brief 释放mgr资源，与CRYPT_MPA_MallocMgr接口配对使用
 * @param mgr [IN] 多流并行计算管理结构体，包含job池子
 * @return NONE
 */
void CRYPT_MPA_FreeMgr(CRYPT_MPA_Mgr *mgr);

/**
 * @ingroup crypt_mpa_aead
 * @brief 释放job资源. 如果CRYPT_MPA_CommitJob 和 CRYPT_MPA_FetchJob接口返回job资源，用户需要调用本接口释放job资源
 * @attention 调用者需要保证入参mgr和job不为空
 * @param mgr [IN] 多流并行计算管理结构体，包含job池子
 * @param job [IN] 多流并行调度单元(当前支持算法：hmac，aes，aes-hmac组合)
 * @return NONE
 */
void CRYPT_MPA_FreeJob(CRYPT_MPA_Mgr *mgr, CRYPT_MPA_Job *job);

/**
 * @ingroup crypt_mpa_aead
 * @brief 强制处理job，并且返回处理完的job，每次返回一个job，直到所有的job处理完毕
 * @attention 调用者需要保证入参mgr不为空
 * @param mgr [IN] 多流并行计算管理结构体，包含job池子
 * @return 用户提交的job如果还有没处理完的，返回处理完的job；所有job都处理完了，则返回NULL。用户需要负责释放返回的job
 */
void *CRYPT_MPA_FetchJob(CRYPT_MPA_Mgr *mgr);

/**
 * @ingroup crypt_mpa_aead
 * @brief AES扩展密钥算法
 * @param aesMode [IN] 指定AES模式
 * @param key [IN] AES原始密钥. aes128: 16 bytes; aes192: 24 bytes; aes256: 32 bytes.
 * @param encExpKey [OUT] 加密扩展密钥起始地址. 用户必须保证16字节对齐
 * @param decExpKey [OUT] 解密扩展密钥起始地址. 用户必须保证16字节对齐
 * @return 成功返回0；失败返回其它值
 */
uint32_t CRYPT_MPA_AesExpsionKey(CRYPT_MPA_AesMode aesMode, const uint8_t *key, uint8_t *encExpKey, uint8_t *decExpKey);

/**
 * @ingroup crypt_mpa_aead
 * @brief hmac-sha2 算法 opadkey ipadkey hash计算，本接口不做填充. 在使用该接口前，需要调用CRYPT_MPA_Init初始化
 * @param hashAlg [IN] 指定hash模式
 * @param key [IN] hmac 密钥
 * @param keyLen [IN] hmac 密钥长度
 * @param opadKeyHash [OUT] opay key hash计算结果. SHA256要求32字节空间;SHA512算法要求64字节空间
 * @param opadKeyHashLen [IN] 指定opadKeyHash长度
 * @param ipadKeyHash [OUT] ipay key hash计算结果. SHA256要求32字节空间;SHA512算法要求64字节空间
 * @param ipadKeyHashLen [IN] 指定ipadKeyHash长度
 * @return 成功返回0；失败返回其它值
 */
uint32_t CRYPT_MPA_PadKeyHashNoPadding(CRYPT_MPA_HashAlg hashAlg, const uint8_t *key, uint32_t keyLen,
    uint8_t *opadKeyHash, uint32_t opadKeyHashLen, uint8_t *ipadKeyHash, uint32_t ipadKeyHashLen);

#ifdef __cplusplus
}
#endif

#endif