/**
 * @defgroup    crypt_eal_pkey  crypt_eal_pkey.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       消息非对称算法对外接口文件
 */

#ifndef CRYPT_EAL_PKEY_H
#define CRYPT_EAL_PKEY_H

#include <stdbool.h>
#include <stdint.h>
#include "crypt_algid.h"
#include "crypt_types.h"
#include "crypt_eal_pkey.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @ingroup crypt_eal_pkey
 *
 * EAL公钥结构体
 */
typedef struct {
    CRYPT_PKEY_AlgId id; /**< 公钥算法ID */
    union {
        CRYPT_RsaPub rsaPub; /**< RSA 公钥结构体 */
        CRYPT_DsaPub dsaPub; /**< DSA 公钥结构体 */
        CRYPT_DhPub dhPub;   /**< DH 公钥结构体 */
        CRYPT_EccPub eccPub; /**< ECC 公钥结构体 */
        CRYPT_Curve25519Pub curve25519Pub; /**< ed25519/x25519 公钥结构体 */
        CRYPT_Curve448Pub curve448Pub; /**< ed448/x448 公钥结构体 */
        CRYPT_Sm9Pub sm9Pub; /**< sm9 公钥结构体 */
    } key;                           /**< 全部算法的公钥联合体 */
} CRYPT_EAL_PkeyPub;

/**
 * @ingroup crypt_eal_pkey
 *
 * EAL私钥结构体
 */
typedef struct {
    CRYPT_PKEY_AlgId id; /**< 私钥算法ID */
    union {
        CRYPT_RsaPrv rsaPrv; /**< RSA 私钥结构体 */
        CRYPT_DsaPrv dsaPrv; /**< DSA 私钥结构体 */
        CRYPT_DhPrv  dhPrv;  /**< DH 私钥结构体 */
        CRYPT_EccPrv eccPrv; /**< ECC 私钥结构体 */
        CRYPT_Curve25519Prv curve25519Prv; /**< ed25519/x25519 私钥结构体 */
        CRYPT_Curve448Prv curve448Prv; /**< ed448/x448 私钥结构体 */
        CRYPT_Sm9Prv sm9Prv; /**< sm9 私钥结构体 */
    } key;                           /**< 全部算法的私钥联合体 */
} CRYPT_EAL_PkeyPrv;

/**
 * @ingroup crypt_eal_pkey
 *
 * 用于非对称算法Para使用的结构体，包括算法id和对应算法的Para联合体
 */
typedef struct {
    CRYPT_PKEY_AlgId id; /**< 非对称算法ID */
    union {
        CRYPT_RsaPara rsaPara; /**< RSA Para结构体 */
        CRYPT_DsaPara dsaPara; /**< DSA Para结构体 */
        CRYPT_DhPara  dhPara;  /**< DH Para结构体 */
    } para;                            /**< 全部算法的Para联合体 */
} CRYPT_EAL_PkeyPara;

/**
* @ingroup  crypt_eal_pkey
*
* pkey的会话结构体
*/
typedef struct EAL_PkeyCtx CRYPT_EAL_PkeyCtx;

/**
 * @ingroup crypt_eal_pkey
 * @brief   密钥配对一致性测试
 *
 * @param   pkey     [IN] 秘钥对结构体
 *
 * @return  true 测试成功
 *  false 测试失败
 */
typedef bool (*CRYPT_EAL_Pct)(CRYPT_EAL_PkeyCtx *pkey);

/**
 * @ingroup crypt_eal_pkey
 * @brief   密钥配对一致性测试回调注册接口，EAL生成非对称密钥对后会调用该回调，回调失败会导致EAL接口失败
 *
 * @param   func     [IN] 密钥配对一致性测试回调
 *
 * @return  无
 */
void CRYPT_EAL_RegPct(CRYPT_EAL_Pct func);

/**
 * @ingroup crypt_eal_pkey
 *
 * 非对称合规判断数据
 */
typedef struct {
    const CRYPT_EAL_PkeyPara *para; /**< CRYPT_EAL_PkeyPara结构体 */
    const CRYPT_EAL_PkeyPub *pub; /**< CRYPT_EAL_PkeyPub结构体 */
    const CRYPT_EAL_PkeyPrv *prv; /**< CRYPT_EAL_PkeyPrv结构体 */
    CRYPT_MD_AlgId mdId; /**< 哈希算法ID */
    CRYPT_PKEY_ParaId paraId; /**< 非对称算法参数ID */
    CRYPT_EVENT_TYPE oper; /**< 当前执行的操作 */
    const CRYPT_RSA_PkcsV15Para *pkcsv15; /**< CRYPT_RSA_PkcsV15Para结构体 */
    const CRYPT_RSA_PssPara *pss; /**< CRYPT_RSA_PssPara结构体 */
    const CRYPT_RSA_OaepPara *oaep; /**< CRYPT_RSA_OaepPara结构体 */
} EAL_PkeyC2Data;

/**
  * @ingroup crypt_eal_pkey
  * @brief 非对称合规判断
  *
  * @param id [IN] 非对称算法id
  * @param data [IN] 数据
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_PkeyC2)(CRYPT_PKEY_AlgId id, const EAL_PkeyC2Data *data);

/**
 * @ingroup crypt_eal_pkey
 * @brief   非对称合规判断回调注册接口，用于判断算法参数是否合规和模块状态是否可用
 *            EAL的非对称接口在执行操作前会调用该回调，回调失败会导致EAL接口失败
 *          非核准模式下可以使用任意算法
 *          ISO19790模式下只允许CRYPT_PKEY_DSA/CRYPT_PKEY_X25519/CRYPT_PKEY_RSA/CRYPT_PKEY_DH/
 *            CRYPT_PKEY_ECDSA/CRYPT_PKEY_ECDH算法
 *          RSA密钥长度至少为2048bits
 *          DSA算法p至少为2048bits，q至少为224bits
 *          ECDSA算法p至少为224bits，曲线只允许P-224/P-256/P-384/P-521
 *          DH算法p至少为2048bits，q至少为256bits
 *          ECDH算法曲线只允许P-224/P-256/P-384/P-521
 *          签名支持CRYPT_MD_SHA224/CRYPT_MD_SHA256/CRYPT_MD_SHA384/CRYPT_MD_SHA512
 *          验签支持CRYPT_MD_SHA1/CRYPT_MD_SHA224/CRYPT_MD_SHA256/CRYPT_MD_SHA384/CRYPT_MD_SHA512
 *
 * @param   func     [IN] 非对称合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegPkeyC2(CRYPT_EAL_PkeyC2 func);


/* Pkey对外接口 */
/**
 * @ingroup crypt_eal_pkey
 * @brief   创建一个非对称秘钥对结构体
 *
 * @param   id [IN] 算法ID
 *
 * @return  CRYPT_EAL_PkeyCtx 指针
 *          若失败返回空
 */
CRYPT_EAL_PkeyCtx *CRYPT_EAL_PkeyNewCtx(CRYPT_PKEY_AlgId id);

/**
 * @ingroup crypt_eal_pkey
 * @brief   释放非对称秘钥对结构体
 *
 * @param   pkey [IN] pkey上下文, pkey由调用方置空
 *
 * @return  无
 */
void CRYPT_EAL_PkeyFreeCtx(CRYPT_EAL_PkeyCtx *pkey);

/**
 * @ingroup crypt_eal_pkey
 * @brief   设置秘钥参数，其中para由调用者申请和释放内存
 *
 * @param   pkey [IN/OUT] 待设置秘钥对结构体
 * @param   para [IN] 参数
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeySetPara(CRYPT_EAL_PkeyCtx *pkey, const CRYPT_EAL_PkeyPara *para);

/**
 * @ingroup crypt_eal_pkey
 * @brief   设置秘钥参数
 *
 * @param   pkey [IN/OUT] 待设置秘钥对结构体
 * @param   id [IN] 参数id
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeySetParaById(CRYPT_EAL_PkeyCtx *pkey, CRYPT_PKEY_ParaId id);

/**
 * @ingroup crypt_eal_pkey
 * @brief   获取秘钥参数，其中para由调用者申请和释放内存
 *
 * @param   pkey [IN] 待设置秘钥对结构体
 * @param   para [OUT] 参数
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyGetPara(const CRYPT_EAL_PkeyCtx *pkey, CRYPT_EAL_PkeyPara *para);

/**
 * @ingroup crypt_eal_pkey
 * @brief   生成秘钥数据
 *
 * @param   pkey [IN/OUT] 接收秘钥数据的秘钥对结构体
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyGen(CRYPT_EAL_PkeyCtx *pkey);

/**
 * @ingroup crypt_eal_pkey
 * @brief   公钥设置，其中key由调用者申请和释放内存。
 *
 * @param   pkey [OUT] 接收秘钥数据的秘钥对结构体
 * @param   key  [IN] 公钥数据
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeySetPub(CRYPT_EAL_PkeyCtx *pkey, const CRYPT_EAL_PkeyPub *key);

/**
 * @ingroup crypt_eal_pkey
 * @brief   私钥设置，其中key由调用者申请和释放内存。
 *
 * @param   pkey [OUT] 接收秘钥数据的秘钥对结构体
 * @param   key  [IN] 私钥数据
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeySetPrv(CRYPT_EAL_PkeyCtx *pkey, const CRYPT_EAL_PkeyPrv *key);

/**
 * @ingroup crypt_eal_pkey
 * @brief   公钥获取，其中key由调用者申请和释放内存。
 *
 * @param   pkey [IN] 秘钥会话
 * @param   key  [OUT] 公钥数据
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyGetPub(const CRYPT_EAL_PkeyCtx *pkey, CRYPT_EAL_PkeyPub *key);

/**
 * @ingroup crypt_eal_pkey
 * @brief   私钥获取，其中key由调用者申请和释放内存。
 *
 * @param   pkey [IN] 秘钥会话
 * @param   key  [OUT] 私钥数据
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyGetPrv(const CRYPT_EAL_PkeyCtx *pkey, CRYPT_EAL_PkeyPrv *key);

/**
 * @ingroup crypt_eal_pkey
 * @brief   签名接口
 *
 * @param   pkey     [IN] 秘钥会话
 * @param   id       [IN] 哈希算法ID
 * @param   data     [IN] 明文数据
 * @param   dataLen  [IN] 明文长度,最大长度限制为[0,0xffffffff]
 * @param   sign     [OUT] 签名数据
 * @param   signLen  [OUT/IN] 签名数据长度。可通过接口 CRYPT_EAL_PkeyGetSignLen 获取该值。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeySign(const CRYPT_EAL_PkeyCtx *pkey, CRYPT_MD_AlgId id, const uint8_t *data,
    uint32_t dataLen, uint8_t *sign, uint32_t *signLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   验签接口
 *
 * @param   pkey      [IN] 秘钥会话
 * @param   id        [IN] 哈希算法ID
 * @param   data      [IN] 明文数据
 * @param   dataLen   [IN] 明文长度,最大长度限制为[0,0xffffffff]
 * @param   sign      [IN] 签名数据
 * @param   signLen   [IN] 签名数据长度
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyVerify(const CRYPT_EAL_PkeyCtx *pkey, CRYPT_MD_AlgId id, const uint8_t *data,
    uint32_t dataLen, const uint8_t *sign, uint32_t signLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   对哈希数据进行签名的接口
 *
 * @param   pkey      [IN] 秘钥会话
 * @param   hash      [IN] 哈希数据
 * @param   hashLen   [IN] 哈希长度。
 *                         当使用RSA用于签名时，哈希长度应该与填充方法所依赖的哈希算法的摘要长度对应
 * @param   sign      [OUT] 签名数据
 * @param   signLen   [OUT/IN] 签名数据长度。可通过接口 CRYPT_EAL_PkeyGetSignLen 获取该值。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeySignData(const CRYPT_EAL_PkeyCtx *pkey, const uint8_t *hash, uint32_t hashLen,
    uint8_t *sign, uint32_t *signLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   对哈希数据进行验签的接口
 *
 * @param   pkey     [IN] 秘钥会话
 * @param   hash     [IN] 哈希数据
 * @param   hashLen   [IN] 哈希长度。
 *                         当使用RSA用于签名时，哈希长度应该与填充方法所依赖的哈希算法的摘要长度对应
 * @param   sign     [IN] 签名数据
 * @param   signLen  [IN] 签名数据长度
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyVerifyData(const CRYPT_EAL_PkeyCtx *pkey, const uint8_t *hash, uint32_t hashLen,
    const uint8_t *sign, uint32_t signLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   对数据进行加密
 *
 * @param   pkey      [IN] 秘钥会话
 * @param   data      [IN] 输入明文数据
 * @param   dataLen   [IN] 数据长度。
 *
 * @param   out      [OUT] 加密后的数据，RSA加密时用于保存加密数据的内存buff长度必须大于等于秘钥模长
 * @param   outLen   [OUT/IN] 加密数据长度。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyEncrypt(const CRYPT_EAL_PkeyCtx *pkey, const uint8_t *data, uint32_t dataLen,
    uint8_t *out, uint32_t *outLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   对数据进行解密
 *
 * @param   pkey      [IN] 秘钥会话
 * @param   data      [IN] 输入密文数据
 * @param   dataLen   [IN] 数据长度。
 *
 * @param   out      [OUT] 解密后的数据
 * @param   outLen   [OUT/IN] 解密数据长度。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyDecrypt(const CRYPT_EAL_PkeyCtx *pkey, const uint8_t *data, uint32_t dataLen,
    uint8_t *out, uint32_t *outLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   计算共享秘钥
 *
 * @param   pkey         [IN] 秘钥会话
 * @param   pubKey       [IN] 公钥会话
 * @param   share        [OUT] 共享秘钥
 * @param   shareLen     [IN/OUT] 入参为share空间长度，出参为share有效长度，所需空间可通过接口CRYPT_EAL_PkeyGetKeyLen获取
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyComputeShareKey(const CRYPT_EAL_PkeyCtx *pkey, const CRYPT_EAL_PkeyCtx *pubKey,
    uint8_t *share, uint32_t *shareLen);

/**
 * @ingroup crypt_eal_pkey
 * @brief   获取秘钥长度的字节数
 *
 * @param   pkey [IN] 秘钥会话
 *
 * @return  成功返回秘钥长度
 *          失败返回0
 */
uint32_t CRYPT_EAL_PkeyGetKeyLen(const CRYPT_EAL_PkeyCtx *pkey);

/**
 * @ingroup crypt_eal_pkey
 * @brief   获取秘钥进行签名的签名长度，只支持可用于签名的算法
 *
 * @param   pkey [IN] 秘钥会话
 *
 * @return  成功返回签名长度
 *          失败返回0
 */
uint32_t CRYPT_EAL_PkeyGetSignLen(const CRYPT_EAL_PkeyCtx *pkey);

/**
 * @ingroup crypt_eal_pkey
 * @brief   对秘钥进行特定的设置/获取动作，支持动作可参考CRYPT_PkeyCtrl枚举
 *
 * @param   pkey [IN] 秘钥会话
 * @param   opt [IN] 选项信息
 * @param   val [IN/OUT] 进行设置/获取的数据
 * @param   len [IN] val数据的长度
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyCtrl(CRYPT_EAL_PkeyCtx *pkey, CRYPT_PkeyCtrl opt, void *val, uint32_t len);

/**
 * @ingroup crypt_eal_pkey
 * @brief   获取密钥算法类型
 *
 * @param   pkey [IN] 秘钥会话
 *
 * @return  密钥算法类型
 */
CRYPT_PKEY_AlgId CRYPT_EAL_PkeyGetId(const CRYPT_EAL_PkeyCtx *pkey);

/**
 * @ingroup crypt_eal_pkey
 * @brief   检查密钥对一致性
 *
 * @param   pkey [IN] 秘钥会话
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_PkeyCheck(const CRYPT_EAL_PkeyCtx *pkey);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CRYPT_EAL_PKEY_H
