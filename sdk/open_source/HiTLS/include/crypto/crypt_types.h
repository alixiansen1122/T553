/**
 * @defgroup    crypt_types crypt_types.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       算法ID对外接口
 */

#ifndef CRYPT_TYPES_H
#define CRYPT_TYPES_H

#include <stdint.h>
#include "crypt_algid.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @ingroup crypt_types
 *
 * 数据结构体
 */
typedef struct {
    uint8_t *data; /**< 数据内容 */
    uint32_t len;    /**< 数据长度 */
} CRYPT_Data;

/**
 * @ingroup crypt_types
 *
 * 数据范围
 */
typedef struct {
    uint32_t min; /**< 最小值 */
    uint32_t max; /**< 最大值 */
} CRYPT_Range;

/**
 * @ingroup crypt_types
 *
 * RSA用于签名时，Pkcsv15填充方式的结构体
 */
typedef struct {
    CRYPT_MD_AlgId mdId; /**< pkcsv15 填充时时候的哈希算法ID*/
} CRYPT_RSA_PkcsV15Para;

/**
 * @ingroup crypt_types
 *
 * RSA用于签名时，PSS填充方式的结构体
 */
typedef struct {
    int32_t saltLen;      /**< pss 盐值长度，-1表示hashLen，-2表示MaxLen */
    CRYPT_MD_AlgId mdId;  /**< pss 填充时的mdid */
    CRYPT_MD_AlgId mgfId; /**< pss 填充时的mgfid */
} CRYPT_RSA_PssPara;

typedef struct {
    CRYPT_MD_AlgId mdId;  /**< pss 填充时的mdid */
    CRYPT_MD_AlgId mgfId; /**< pss 填充时的mgfid */
} CRYPT_RSA_OaepPara;

typedef enum {
    CRYPT_RSA_BLINDING = 0x00000001,            /**< 开启RSA blinding功能进行签名
                                                    此时需要设置公钥信息，或者在设置私钥的时候设置p q参数 */
    CRYPT_RSA_MAXFLAG
} CRYPT_RSA_Flag;

/**
 * @ingroup crypt_types
 *
 * RSA私钥参数结构体
 */
typedef struct {
    uint8_t *d;  /**< RSA 私钥参数d */
    uint8_t *n;  /**< RSA 私钥参数n */
    uint8_t *p;  /**< RSA 私钥参数p */
    uint8_t *q;  /**< RSA 私钥参数q */
    uint32_t dLen; /**< RSA 私钥参数d的长度 */
    uint32_t nLen; /**< RSA 私钥参数n的长度 */
    uint32_t pLen; /**< RSA 私钥参数p的长度 */
    uint32_t qLen; /**< RSA 私钥参数q的长度 */
} CRYPT_RsaPrv;

/**
 * @ingroup crypt_types
 *
 * DSA私钥参数结构体
 */
typedef CRYPT_Data CRYPT_DsaPrv;

/**
 * @ingroup crypt_types
 *
 * ECC私钥参数结构体
 */
typedef CRYPT_Data CRYPT_EccPrv;

/**
 * @ingroup crypt_types
 *
 * ECDSA私钥参数结构体
 */
typedef CRYPT_Data CRYPT_EcdsaPrv;

/**
 * @ingroup crypt_types
 *
 * SM2私钥参数结构体
 */
typedef CRYPT_Data CRYPT_Sm2Prv;

/**
 * @ingroup crypt_types
 *
 * DH私钥参数结构体
 */
typedef CRYPT_Data CRYPT_DhPrv;

/**
 * @ingroup crypt_types
 *
 * ECDH私钥参数结构体
 */
typedef CRYPT_Data CRYPT_EcdhPrv;

/**
 * @ingroup crypt_types
 *
 * ed25519/x25519私钥参数结构体
 */
typedef CRYPT_Data CRYPT_Curve25519Prv;

/**
 * @ingroup crypt_types
 *
 * ed448/x448私钥参数结构体
 */
typedef CRYPT_Data CRYPT_Curve448Prv;

/**
 * @ingroup crypt_types
 *
 * RSA公钥参数结构体
 */
typedef struct {
    uint8_t *e;  /**< RSA 公钥参数e */
    uint8_t *n;  /**< RSA 公钥参数n */
    uint32_t eLen; /**< RSA 公钥参数e的长度 */
    uint32_t nLen; /**< RSA 公钥参数e的长度 */
} CRYPT_RsaPub;

/**
 * @ingroup crypt_types
 *
 * DSA公钥参数结构体
 */
typedef CRYPT_Data CRYPT_DsaPub;

/**
 * @ingroup crypt_types
 *
 * ECC公钥参数结构体
 */
typedef CRYPT_Data CRYPT_EccPub;

/**
 * @ingroup crypt_types
 *
 * ECDSA公钥参数结构体
 */
typedef CRYPT_Data CRYPT_EcdsaPub;

/**
 * @ingroup crypt_types
 *
 * SM2公钥参数结构体
 */
typedef CRYPT_Data CRYPT_Sm2Pub;

/**
 * @ingroup crypt_types
 *
 * DH公钥参数结构体
 */
typedef CRYPT_Data CRYPT_DhPub;

/**
 * @ingroup crypt_types
 *
 * ECDH公钥参数结构体
 */
typedef CRYPT_Data CRYPT_EcdhPub;

/**
 * @ingroup crypt_types
 *
 * ed25519/x25519公钥参数结构体
 */
typedef CRYPT_Data CRYPT_Curve25519Pub;

/**
 * @ingroup crypt_types
 *
 * ed448/x448公钥参数结构体
 */
typedef CRYPT_Data CRYPT_Curve448Pub;


/**
 * @ingroup crypt_types
 *
 * RSA算法的Para结构体
 */
typedef struct {   // 该参数全不可为空, 由底层结构判断
    uint8_t *e;    /**< para 参数e */
    uint32_t eLen; /**< para 参数e的长度 */
    uint32_t bits; /**< para 的bits */
} CRYPT_RsaPara;

/**
 * @ingroup crypt_types
 *
 * DSA算法的Para结构体
 */
typedef struct {   // 该参数全不可为空, 由底层结构判断
    uint8_t *p;  /**< 参数p */
    uint8_t *q;  /**< 参数q */
    uint8_t *g;  /**< 参数g */
    uint32_t pLen; /**< 参数p的长度 */
    uint32_t qLen; /**< 参数q的长度 */
    uint32_t gLen; /**< 参数g的长度 */
} CRYPT_DsaPara;

/**
 * @ingroup crypt_types
 *
 * DH算法的Para结构体
 */
typedef struct {
    uint8_t *p;  /**< 参数p */
    uint8_t *q;  /**< 参数q 该参数可为空L */
    uint8_t *g;  /**< 参数g */
    uint32_t pLen; /**< 参数p的长度 */
    uint32_t qLen; /**< 参数q的长度 */
    uint32_t gLen; /**< 参数g的长度 */
} CRYPT_DhPara;

/**
 * @ingroup crypt_types
 *
 * RAND注册接口的metohd结构体，包括熵源获取和清理接口，随机数获取和清理接口。
 */
typedef struct {
    /**
     * @ingroup crypt_types
     * @brief 获取熵源，该接口必须注册，其输出的数据需要满足长度等要求，HiTLS不会对熵源进行检查，需要由熵源提供者实现
     *
     * ctx      [IN] 调用者所使用的上下文
     * entropy  [OUT] 获取到的熵源数据，熵源数据长度需要满足：lenRange->min <= len <= lenRange->max
     * strength [IN] 熵源强度
     * lenRange [IN] 熵源长度范围
     * @return 0表示成功，其他表示失败
     */
    int32_t (*getEntropy)(void *ctx, CRYPT_Data *entropy, uint32_t strength, CRYPT_Range *lenRange);

    /**
     * @ingroup crypt_types
     * @brief 熵源内存清理，非必须注册接口
     * ctx     [IN] 调用者所使用的上下文
     * entropy [OUT] 熵源数据
     * @return  void
     */
    void (*cleanEntropy)(void *ctx, CRYPT_Data *entropy);

    /**
     * @ingroup crypt_types
     * @brief 获取随机数，该接口不是必须注册的
              如果注册，其输出的数据需要满足长度等要求,HiTLS不会对熵源进行检查，需要由提供则实现
     *
     * ctx      [IN] 调用者所使用的上下文
     * nonce    [OUT] 获取到的随机数，随机数长度需要满足：lenRange->min <= len <= lenRange->max
     * strength [IN] 随机数强度
     * lenRange [IN] 随机数长度范围
     * @return 0表示成功，其他表示失败
     */
    int32_t (*getNonce)(void *ctx, CRYPT_Data *nonce, uint32_t strength, CRYPT_Range *lenRange);

    /**
     * @ingroup crypt_types
     * @brief 随机数内存清理,非必须注册接口
     * ctx   [IN] 调用者所使用的上下文
     * nonce [OUT] 随机数
     * @return  void
     */
    void (*cleanNonce)(void *ctx, CRYPT_Data *nonce);
} CRYPT_RandSeedMethod;

/**
 * @ingroup crypt_ctrl_param
 *
 * 设置、获取内部modes的参数
 */
typedef enum {
    CRYPT_CTRL_SET_IV = 0,        /**< 设置iv 数据为uint8_t 类型*/
    CRYPT_CTRL_GET_IV,            /**< 获取iv 数据为uint8_t* 类型 */
    CRYPT_CTRL_GET_BLOCKSIZE,     /**< 获取块大小 数据为uint8_t类型 */
    CRYPT_CTRL_SET_COUNT,         /**< 设置计数器信息,输入为四字节小端排布的字节流,需要的算法为chacha20 */
    CRYPT_CTRL_SET_AAD,           /**< AEAD加解密模式下设置aad信息 */
    CRYPT_CTRL_GET_TAG,           /**< AEAD加密或解密，在结束时获取tag */
    CRYPT_CTRL_SET_TAGLEN,        /**< AEAD加解密，设置tag长度，在加解密开始之前设置，设置类型为uint32_t */
    CRYPT_CTRL_SET_MSGLEN,        /**< CMM模式下需要加密消息长度作为输入用于计算，需要在SET_AAD之前设置，输入数据为一个uint64_t类型 */
    CRYPT_CTRL_SET_FEEDBACKSIZE,  /**< CFB模式下设置密文反馈长度 */
    CRYPT_CTRL_GET_FEEDBACKSIZE,  /**< CFB模式下获取密文反馈长度 */
    CRYPT_CTRL_SET_SM4_CONSTTIME, /**< SM4选择侧信道安全实现，降低性能。仅在启用ARM汇编实现时有效 */
    CRYPT_CTRL_MAX
} CRYPT_CipherCtrl;

/**
 * @ingroup crypt_ctrl_param
 *
 * pkey设置、获取内部的参数
 */
typedef enum {
    CRYPT_CTRL_SET_ED25519_HASH_METHOD, /**< ED25519设置哈希方法 */
    CRYPT_CTRL_SET_RSA_EMSA_PKCSV15,    /**< RSA设置签名填充方式为EMSA_PKCSV15 */
    CRYPT_CTRL_SET_RSA_EMSA_PSS,        /**< RSA设置签名填充方式为EMSA_PSS */
    CRYPT_CTRL_SET_RSA_SALT,            /**< RSA算法进行PSS签名时指定盐数据，使用时将用户数据地址直接保存到秘钥
                                             并在下一次签名时使用，必须保证设置后的下一次签名在盐数据的生命周期内
                                             调用，该选项不推荐用户使用，仅用于KAT测试、自校验时使用 */
    CRYPT_CTRL_SET_ECC_POINT_FORMAT,    /**< ECC PKEY设置点格式，点格式参照CRYPT_PKEY_PointFormat */
    CRYPT_CTRL_SET_ECC_USE_COFACTOR_MODE, /**< ECC PKEY设置是否使用cofactor模式，使用后可防止中间人篡改
                                               公钥，使用置为1，不使用置为0  */
    CRYPT_CTRL_SET_RSA_RSAES_OAEP,            /**< RSA设置加解密填充方式为RSAES_OAEP */
    CRYPT_CTRL_SET_RSA_OAEP_LABEL,            /**< RSA oaep充填设置label, 用于生成hash值 */
    CRYPT_CTRL_SET_RSA_FLAG,                  /**< RSA 设置flag */
    CRYPT_CTRL_CLR_RSA_FLAG,                  /**< RSA 清除flag */
    CRYPT_CTRL_SET_RSA_RSAES_PKCSV15,         /**< RSA设置加解密填充方式为RSAES_PKCSV15 */

    CRYPT_CTRL_SET_SM9_HASH_METHOD,     /**< SM9设置哈希方法 */

    CRYPT_CTRL_SET_ED448_HASH_METHOD,   /**< ed448设置哈希方法 */
    CRYPT_CTRL_SET_ED448_CONTEXT,       /**< ed448设置context */
    CRYPT_CTRL_SET_ED448_PREHASH,       /**< ed448设置prehash模式 */
    CRYPT_CTRL_SET_SM2_USER_ID,         /**< SM2设置用户ID */
    CRYPT_CTRL_SET_SM2_HASH_METHOD,     /**< SM2设置使用SM3计算杂凑值 */
    CRYPT_CTRL_SET_SM2_SERVER,          /**< SM2设置用户状态 */
    CRYPT_CTRL_GENE_SM2_R,              /**< SM2获取R值 */
    CRYPT_CTRL_SET_SM2_R,               /**< SM2设置R值 */
    CRYPT_CTRL_SM2_GET_SEND_CHECK,      /**< SM2获取本端发送给对端的校验值 */
    CRYPT_CTRL_SM2_DO_CHECK,            /**< SM2校验共享密钥 */
    CRYPT_CTRL_SET_NO_PADDING,          /**< RSA设置加解密填充方式为NO_PADDING */
} CRYPT_PkeyCtrl;

/**
 * @ingroup crypt_padding_type
 *
 * 填充方式枚举类型
 */
typedef enum {
    CRYPT_PADDING_NONE = 0,         /**< Never pad (full blocks only).   */
    CRYPT_PADDING_ZEROS,            /**< Zero padding (not reversible).  */
    CRYPT_PADDING_ISO7816,          /**< ISO/IEC 7816-4 padding.         */
    CRYPT_PADDING_X923,             /**< ANSI X.923 padding.            */
    CRYPT_PADDING_PKCS5,            /**< PKCS5 padding.                  */
    CRYPT_PADDING_PKCS7,            /**< PKCS7 padding.                  */
    CRYPT_PADDING_MAX_COUNT
} CRYPT_PaddingType;

/**
 * @ingroup  crypt_types
 * 操作类型
 */
typedef enum {
    CRYPT_EVENT_ENC,          /**< 加密*/
    CRYPT_EVENT_DEC,          /**< 解密*/
    CRYPT_EVENT_GEN,          /**< 生成密钥*/
    CRYPT_EVENT_SIGN,         /**< 签名*/
    CRYPT_EVENT_VERIFY,       /**< 验签*/
    CRYPT_EVENT_MD,           /**< 哈希*/
    CRYPT_EVENT_MAC,          /**< MAC*/
    CRYPT_EVENT_KEYAGGREMENT, /**< 密钥协商*/
    CRYPT_EVENT_KEYDERIVE,    /**< 派生密钥*/
    CRYPT_EVENT_RANDGEN,      /**< 生成随机数*/
    CRYPT_EVENT_ZERO,         /**< 敏感信息置零*/
    CRYPT_EVENT_ERR,          /**< 发生错误*/
    CRYPT_EVENT_SETSSP,       /**< 添加、修改密码数据和SSP*/
    CRYPT_EVENT_GETSSP,       /**< 访问密码数据和SSP*/
    CRYPT_EVENT_MAX
} CRYPT_EVENT_TYPE;

/**
 * @ingroup  crypt_types
 * 算法类型
 */
typedef enum {
    CRYPT_ALGO_CIPHER = 0,
    CRYPT_ALGO_PKEY,
    CRYPT_ALGO_MD,
    CRYPT_ALGO_MAC,
    CRYPT_ALGO_KDF,
    CRYPT_ALGO_RAND
} CRYPT_ALGO_TYPE;

/**
 * @ingroup crypt_types
 * @brief 事件上报
 *
 * @param   oper [IN] 操作类型
 * @param   type [IN] 算法类型
 * @param   id [IN] 算法id
 * @param   err [IN] 错误码，成功为CRYPT_SUCCESS，其他错误码参考crypt_errno.h
 *
 * @return 无
 */
typedef void (*EventReport)(CRYPT_EVENT_TYPE oper, CRYPT_ALGO_TYPE type, int32_t id, int32_t err);

/**
 * @ingroup crypt_types
 * @brief
 * 事件上报回调注册接口，EAL执行完服务及报错时会上报事件。不支持多线程
 *
 * @param   func     [IN] 事件上报处理回调
 *
 * @return  无
 */
void CRYPT_EAL_RegEventReport(EventReport func);

/* SM9算法秘钥类型枚举 */
typedef enum {
    SM9_NOKEY = 0x00,               // 不存在秘钥信息
    SM9_SIGN_KEY = 0x01,            // 签名
    SM9_VERIFY_KEY = 0x02,          // 验签
    SM9_COMPUTE_SHARE_KEY = 0x04,   // 共享秘钥计算
    SM9_ENC_KEY = 0x08,             // 加密
    SM9_DEC_KEY = 0x10,             // 解密
    SM9_KEY_GEN = 0x20,             // 秘钥生成
} CRYPT_SM9_KeyType;

/* SM9算法用户设置/获取秘钥使用的公钥结构体 */
typedef struct {
    CRYPT_Data pub;     // 主公钥 1、用于签名验签 Fp2；2、用于加解密 Fp1
    CRYPT_Data idA;    // 标识ID
    CRYPT_SM9_KeyType keyType;  // 秘钥类型，用于签名验签、共享秘钥计算、加解密
    uint8_t hid;    // 标识符
} CRYPT_Sm9Pub;

/* SM9算法用户设置/获取秘钥使用的私钥结构体 */
typedef struct {
    CRYPT_Data pub;     // 主公钥 1、用于签名验签 Fp2；2、用于加解密、共享秘钥计算 Fp1
    CRYPT_Data prv;     // 主私钥 1、进行秘钥生成时使用
    CRYPT_Data d;       // 用户私钥 1、用于签名验签 Fp1 2、用于加解密、共享秘钥计算 Fp2
    CRYPT_Data idA;    // 标识ID 签名可设置为NULL
    CRYPT_SM9_KeyType keyType; // 秘钥类型
    uint8_t hid;        // 标识符 签名时不使用该值
} CRYPT_Sm9Prv;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CRYPT_TYPES_H
