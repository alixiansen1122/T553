/**
 * @defgroup    crypt_errno    crypt_errno.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       crypto模块返回码
 */

/** @defgroup   crypto  crypto */

#ifndef CRYPT_ERRNO_H
#define CRYPT_ERRNO_H

#include "crypt_check_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup crypt_errno
 * @brief   返回成功
 */
#define CRYPT_SUCCESS 0

/**
 * @ingroup crypt_errno
 *
 * CRYPTO模块的返回值
 */
enum CRYPT_ERROR {
    CRYPT_NULL_INPUT = 0x01010001,      /**< 错误的空指针输入, bufferLen为0 */
    CRYPT_SECUREC_FAIL,                 /**< 安全函数返回错误 */
    CRYPT_MEM_ALLOC_FAIL,               /**< 内存资源申请失败 */
    CRYPT_NO_REGIST_RAND,               /**< 未注册全局随机数 */
    CRYPT_ERR_ALGID,                    /**< 错误的算法ID */
    CRYPT_INVALID_ARG,                  /**< 非法入参 */

    CRYPT_BN_BUFF_LEN_NOT_ENOUGH = 0x01020001, /**< 缓冲区长度不足 */
    CRYPT_BN_SPACE_NOT_ENOUGH,          /**< 大数空间不足 */
    CRYPT_BN_BITS_TOO_MAX,              /**< 超出大数最大bits限制 */
    CRYPT_BN_RAND_GEN_FAIL,             /**< 随机数生成失败 */
    CRYPT_BN_OPTIMIZER_STACK_FULL,      /**< 优化器栈满了失败 */
    CRYPT_BN_NO_NEGATIVE_ZERO,          /**< 大数0只设置为正数符号 */
    CRYPT_BN_ERR_RAND_ZERO,             /**< 生成比0小的随机数 */
    CRYPT_BN_ERR_RAND_NEGATIVE,         /**< 生成负数随机数 */
    CRYPT_BN_ERR_RAND_TOP_BOTTOM,       /**< 随机数生成时top或bottom无效 */
    CRYPT_BN_ERR_RAND_BITS_NOT_ENOUGH,  /**< 随机数生成时bit过小 */
    CRYPT_BN_OPTIMIZER_GET_FAIL,        /**< 从优化器获取空间失败 */
    CRYPT_BN_ERR_DIVISOR_ZERO,          /**< 除数不能为0 */
    CRYPT_BN_ERR_EXP_NO_NEGATIVE,       /**< 幂的值不能为负数 */
    CRYPT_BN_MONT_BASE_TOO_MAX,         /**< 蒙哥马利模幂底数过大 */
    CRYPT_BN_NOR_GEN_PRIME,             /**< 素数生成失败 */
    CRYPT_BN_NOR_CHECK_PRIME,           /**< 素数检查失败 */
    CRYPT_BN_ERR_GCD_NO_ZERO,           /**< 求最大公约数不能有0 */
    CRYPT_BN_ERR_NO_INVERSE,            /**< 无法求出模逆 */
    CRYPT_BN_ERR_SQRT_PARA,             /**< 模开根号参数错误 */
    CRYPT_BN_ERR_LEGENDE_DATA,          /**< 计算平方根时寻找z对p的勒让德符号(z|p)等于-1特定的数失败 */
    CRYPT_BN_ERR_NO_SQUARE_ROOT,        /**< 找不到平方根 */
    CRYPT_BN_ERR_MASKCOPY_LEN,          /**< 带掩码进行拷贝时，数据长度不一致 */
    CRYPT_BN_ERR_QUICK_MODDATA,         /**< 使用BN_ModNistEccMul和BN_ModNistEccSqr接口时，其模数据不支持 */

    CRYPT_RSA_BUFF_LEN_NOT_ENOUGH = 0x01030001, /**< 缓冲区长度不足 */
    CRYPT_RSA_NO_KEY_INFO,               /**< 缺乏有效秘钥信息 */
    CRYPT_RSA_ERR_KEY_BITS,              /**< 秘钥长度错误 */
    CRYPT_RSA_ERR_E_VALUE,               /**< 参数e数值错误 */
    CRYPT_RSA_NOR_KEYGEN_FAIL,           /**< 秘钥生成失败，正常错误 */
    CRYPT_RSA_NOR_VERIFY_FAIL,           /**< 验签失败，正常错误 */
    CRYPT_RSA_ERR_ENC_BITS,              /**< 公钥加密明文长度错误 */
    CRYPT_RSA_ERR_DEC_BITS,              /**< 私钥解密密文长度错误 */
    CRYPT_RSA_ERR_PSS_SALT_LEN,          /**< PSS操作盐长度错误 */
    CRYPT_RSA_ERR_PSS_SALT_DATA,         /**< PSS操作盐数据错误,验签时提取的盐与用户输入比较失败 */
    CRYPT_RSA_ERR_PKCSV15_SALT_LEN,      /**< PKCSV15操作盐长度错误 */
    CRYPT_RSA_ERR_PKCSV15_SALT_DATA,     /**< PKCSV15操作盐数据错误 */
    CRYPT_RSA_ERR_INPUT_VALUE,           /**< 一些比较特殊的值，作为输入所产生的错误 */
    CRYPT_RSA_ERR_MD_ALGID,              /**< 设置pkcs1.5填充方式时入参的哈希id错误  */
    CRYPT_RSA_PAD_NO_SET_ERROR,          /**< 使用RSA秘钥进行签名验签时尚未设置填充信息  */
    CRYPT_RSA_CTRL_NOT_SUPPORT_ERROR,    /**< 使用RSA进行Ctrl时，使用的Ctrl类型不支持 */
    CRYPT_RSA_SET_SALT_NOT_PSS_ERROR,    /**< 当秘钥的填充类型非pss时，进行盐信息设置，返回失败 */
    CRYPT_RSA_SET_EMS_PKCSV15_LEN_ERROR, /**< 进行PKCSV15填充信息设置时，输入的数据长度错误，返回失败 */
    CRYPT_RSA_SET_EMS_PSS_LEN_ERROR,     /**< 进行PSS填充信息设置时，输入的数据长度错误，返回失败 */
    CRYPT_RSA_SET_RSAES_OAEP_LEN_ERROR,    /**< 进行Oaep填充信息设置时，输入的数据长度错误，返回失败 */
    CRYPT_RSA_SET_FLAG_LEN_ERROR,         /**< 设置flag时，输入数据长度错误，返回失败 */
    CRYPT_RSA_FLAG_NOT_SUPPORT_ERROR,     /**< 不支持的flag */
    CRYPT_RSA_ERR_SALT_LEN,              /**< 盐长度错误 */
    CRYPT_RSA_ERR_ALGID,                 /**< 进行签名、验签以及填充参数设置时，入参的哈希ID错误或者有冲突时返回错误 */
    CRYPT_RSA_ERR_GEN_SALT,              /**< 进行PSS签名时生成盐信息时失败返回错误 */
    CRYPT_RSA_ERR_ENC_INPUT_NOT_ENOUGH,  /**< RSA NO PAD加密，明文长度过短*/

    CRYPT_EAL_BUFF_LEN_NOT_ENOUGH = 0x01040001, /**< 缓冲区长度不足 */
    CRYPT_EAL_ERR_ALGID,                /**< 算法ID错误 */
    CRYPT_EAL_ALG_NOT_SUPPORT,          /**< 算法不支持、算法行为不支持 */
    CRYPT_EAL_RAND_GEN_FAIL,            /**< 随机数生成失败 */
    CRYPT_EAL_ERR_SALT_MEM,             /**< 盐内存错误 */
    CRYPT_EAL_NOR_VERIFY_FAIL,          /**< 验签失败，正常错误 */
    CRYPT_EAL_ERR_NEW_PARA_FAIL,        /**< 生成参数失败 */
    CRYPT_EAL_ERR_RAND_WORKING,         /**< DRBG 处于工作状态 */
    CRYPT_EAL_ERR_RAND_NO_WORKING,      /**< DRBG 处于非工作状态 */
    CRYPT_EAL_ERR_METH_NULL_NUMBER,     /**< method变量成员为NULL */
    CRYPT_EAL_ERR_LOCK_NULL,            /**< 锁为NULL */
    CRYPT_EAL_ERR_GLOBAL_DRBG_NULL,     /**< 全局DRBG为NULL */
    CRYPT_EAL_ERR_DRBG_REPEAT_INIT,     /**< DRBG重复初始化 */
    CRYPT_EAL_ERR_DRBG_INIT_FAIL,       /**< DRBG初始化失败 */
    CRYPT_EAL_ERR_STATE,                /**< 使用流程不正确，例如未执行init，直接执行update，具体用法参见相关算法 */
    CRYPT_EAL_PKEY_PAD_REDUNDANCY,      /**< pkey签名验签接口中，RSA以外的算法不需要填充信息；
                                             如果入参包含填充信息返回该错误码 */
    CRYPT_EAL_CIPHER_DATA_ERROR,        /**< 对于解密后的数据进行去充填是数据出错，
                                             对于X923 最后一位是原始数据的长度，其余充填数据是0，如果不满足此要求，则报错
                                             对于pkcs 所有充填的数据是 充填后的长度 - 原始数据长度，如不满足此要求，则报错
                                             对于ISO7816 充填的第一位数据是0x80, 其余充填0， 如不满足此要求，则报错 */
    CRYPT_EAL_PADDING_NOT_SUPPORT,      /**< 不支持的padding */
    CRYPT_EAL_CIPHER_CTRL_ERROR,        /**< CRYPT_EAL_CipherCtrl接口不支持的CTRL */
    CRYPT_EAL_CIPHER_FIANL_WITH_AEAD_ERROR,     /**< 对AEAD算法进行final操作导致错误 */
    CRYPT_EAL_PKEY_CTRL_ERROR,          /**< CRYPT_EAL_PkeyCtrl接口进行CTRL时功能不支持或者输入的长度不正确 */
    CRYPT_EAL_MAC_CTRL_TYPE_ERROR,          /**< CRYPT_EAL_PkeyCtrl接口进行CTRL时功能不支持或者输入的长度不正确 */

    CRYPT_SHA2_INPUT_OVERFLOW    = 0x01050001, /**< 输入数据的长度超出SHA2的最大处理范围 */
    CRYPT_SHA2_OUT_BUFF_LEN_NOT_ENOUGH,        /**< 承载输出结果的缓冲区长度不足 */

    CRYPT_DRBG_ERR_STATE         = 0x01060001, /**< DRBG 状态错误 */
    CRYPT_DRBG_FAIL_GET_ENTROPY,        /**< 获取熵失败 */
    CRYPT_DRBG_FAIL_GET_NONCE,          /**< 获取 nonce 失败 */
    CRYPT_DRBG_ALG_NOT_SUPPORT,         /**< 不支持给定的算法 */
    CRYPT_DRBG_INVALID_LEN,             /**< 错误的数据长度 */

    CRYPT_MPA_POOL_SIZE_EXCEED   = 0x01070001, /**< 创建pool时单元个数超限 */
    CRYPT_MPA_ERR_ALG_MODE,                    /**< 错误算法模式 */
    CRYPT_MPA_OUT_BUFF_LEN_NOT_ENOUGH,         /**< 承载输出结果的缓冲区长度不足 */

    CRYPT_CURVE25519_NO_PUBKEY         = 0x01080001, /**< 没有公钥 */
    CRYPT_CURVE25519_NO_PRVKEY,                      /**< 没有私钥 */
    CRYPT_CURVE25519_KEYLEN_ERROR,                   /**< 秘钥长度错误 */
    CRYPT_CURVE25519_SIGNLEN_ERROR,                  /**< 签名长度错误 */
    CRYPT_CURVE25519_HASH_METH_ERROR,                /**< 哈希方法不是SHA512 */
    CRYPT_CURVE25519_VERIFY_FAIL,                    /**< 签名不正确，验签失败 */
    CRYPT_CURVE25519_NO_HASH_METHOD,                 /**< 未设置哈希方法 */
    CRYPT_CURVE25519_UNSUPPORTED_CTRL_OPTION,        /**< 不支持的操作模式 */
    CRYPT_CURVE25519_KEY_COMPUTE_FAILED,             /**< 生成共享密钥失败 */
    CRYPT_CURVE25519_INVALID_PUBKEY,                 /**< 公钥非法 */

    CRYPT_SHA1_INPUT_OVERFLOW = 0x01090001,    /**< 输入数据的长度超出SHA1的最大处理范围 */
    CRYPT_SHA1_OUT_BUFF_LEN_NOT_ENOUGH,        /**< 承载输出结果的缓冲区长度不足 */

    CRYPT_ENTROPY_RCT_FAILURE = 0x010A0001,    /**< RCT检测失败，失败时可以重启熵源 */
    CRYPT_ENTROPY_APT_FAILURE,                 /**< APT检测失败，失败时可以重启熵源 */
    CRYPT_ENTROPY_CONDITION_FAILURE,           /**< 调用后处理方法出错 */
    CRYPT_ENTROPY_RANGE_ERROR,                 /**< 熵源生成范围错误 */

    CRYPT_DSA_BUFF_LEN_NOT_ENOUGH = 0x010B0001, /**< 缓冲区长度不足 */
    CRYPT_DSA_ERR_KEY_PARA,                     /**< 秘钥参数数据错误 */
    CRYPT_DSA_ERR_KEY_INFO,                     /**< 秘钥信息错误 */
    CRYPT_DSA_VERIFY_FAIL,                      /**< 验签失败 */
    CRYPT_DSA_ERR_TRY_CNT,                      /**< 秘钥生成、签名在规定的尝试次数内无法生成结果 */
    CRYPT_DSA_DECODE_FAIL,                      /**< 数据解码失败，数据不符合解码要求 */

    CRYPT_HMAC_OUT_BUFF_LEN_NOT_ENOUGH = 0x010C0001, /**< 承载输出结果的缓冲区长度不足 */

    CRYPT_DH_BUFF_LEN_NOT_ENOUGH = 0x010D0001, /**< 缓冲区长度不足 */
    CRYPT_DH_PARA_ERROR,                       /**< 秘钥参数的数值不符合要求、使用ctx时不包含必要的参数信息 */
    CRYPT_DH_KEYINFO_ERROR,                    /**< 公私钥的数值不符合要求、使用ctx时不包含必要的公私钥信息 */
    CRYPT_DH_RAND_GENERATE_ERROR,              /**< 秘钥生成在规定的尝试次数内无法生成结果 */
    CRYPT_DH_PAIRWISE_CHECK_FAIL,              /**< 公私钥不一致*/
    CRYPT_DH_PRVKEY_CHECK_FAIL,                /**< 私钥进行FIPS检查失败*/

    CRYPT_CHACHA20_BUFF_LEN_NOT_ENOUGH = 0x010E0001, /**< 缓冲区长度不足 */
    CRYPT_CHACHA20_KEYLEN_ERROR,                     /**< 进行秘钥设置时 输入的秘钥长度错误 */
    CRYPT_CHACHA20_NONCELEN_ERROR,                   /**< 进行nounce设置时，输入的nounce长度错误 */
    CRYPT_CHACHA20_COUNTLEN_ERROR,                   /**< 进行count设置时，输入的count长度错误 */
    CRYPT_CHACHA20_NO_KEYINFO,                       /**< 进行加解密时缺乏有效的秘钥信息 */
    CRYPT_CHACHA20_NO_NONCEINFO,                     /**< 进行加解密时缺乏有效的nounce信息 */
    CRYPT_CHACHA20_CTRLTYPE_ERROR,                   /**< 使用ctrl接口时输入的type不支持 */

    CRYPT_AES_ERR_KEYLEN = 0x010F0001,          /**< 秘钥长度错误 */

    CRYPT_MODES_TAGLEN_ERROR = 0x01100001,  /**< 在AEAD模式下，获取TAG和校验TAG时输入的TAG长度错误 */
    CRYPT_MODES_IVLEN_ERROR,                /**< 在设置IV时，输入的IV长度错误 */
    CRYPT_MODES_KEYUSE_TOOMANY_TIME,        /**< GCM模式下，限制了秘钥用于加解密的次数，当一个秘钥使用次数超出该限制时报错 */
    CRYPT_MODES_CRYPTLEN_OVERFLOW,          /**< AEAD模式下，单次加密输入的明文/密文长度超出规格限制 */
    CRYPT_MODES_CTRL_TAGLEN_ERROR,          /**< GCM和CCM模式下，使用ctrl接口设置tag长度时入参长度错误或入参数据表示的长度错误 */
    CRYPT_MODES_AAD_REPEAT_SET_ERROR,       /**< 在AEAD模式下，重复设置AAD信息错误 */
    CRYPT_MODE_BUFF_LEN_NOT_ENOUGH,         /**< 缓冲区长度不足 */
    CRYPT_MODE_ERR_INPUT_LEN,               /**< 函数输入长度不是期望的长度 */
    CRYPT_MODES_CTRL_TYPE_ERROR,            /**< 使用ctrl接口时输入的type不支持 */
    CRYPT_MODES_CTRL_TAGLEN_AFTER_USED_ERROR, /**< GCM模式下tagLen信息影响秘钥限制的最大使用次数和加密长度。在使用秘钥进行加解密之后，不允许重新设置tagLen，返回该错误码 */
    CRYPT_MODES_AAD_IS_SET_ERROR,           /**< ccm模式下在设置aad之后进行tagLen、msgLen的设置时返回错误 */
    CRYPT_MODES_MSGLEN_OVERFLOW,            /**< ccm模式下加解密时输入的消息长度超出设置的msgLen */
    CRYPT_MODES_CTRL_MSGLEN_ERROR,          /**< ccm模式下，使用ctrl接口设置msg长度时入参长度错误或入参数据表示的长度错误(该规格受ivLen影响) */
    CRYPT_MODES_MSGLEN_LEFT_ERROR,          /**< ccm模式下，使用ctrl接口获取tag时，已加解密的消息长度尚未达到设置的数量，还有剩余，导致错误 */
    CRYPT_MODES_ERR_KEYLEN,                 /**< 设置了错误的密钥长度 */
    CRYPT_MODES_ERR_KEY,                    /**< 设置了错误的密钥 */
    CRYPT_MODES_ERR_FEEDBACKSIZE,           /**< cfb模式设置了错误的密文反馈长度 */
    CRYPT_MODES_MODE_NOT_SUPPORT,           /**< 当前模式不支持 */

    CRYPT_HKDF_DKLEN_OVERFLOW = 0x01110001,         /**< 派生密钥长度超过最大限制 */
    CRYPT_HKDF_NOT_SUPPORTED,                       /**< 不支持HKDF算法 */

    CRYPT_CMAC_OUT_BUFF_LEN_NOT_ENOUGH = 0x01120001, /**< 承载输出结果的缓冲区长度不足 */
    CRYPT_CMAC_INPUT_OVERFLOW,                       /**< 输入数据的长度超出CMAC的最大处理范围 */

    CRYPT_SCRYPT_PARAM_ERROR = 0x01130001,           /**< 入参错误 */
    CRYPT_SCRYPT_NOT_SUPPORTED,                      /**< 不支持SCRYPT算法 */
    CRYPT_SCRYPT_DATA_TOO_MAX,                      /**< SCRYPT算法计算数据太大 */

    CRYPT_CMVP_ERR_CSP_EXIST = 0x1140001, /**< 内存中存在CSP导致切换模式失败 */
    CRYPT_CMVP_INVALID_INPUT,             /**< 要切换的模式错误 */
    CRYPT_CMVP_ERR_STATUS,                /**< 当前状态不可用 */
    CRYPT_CMVP_ERR_LOCK,                  /**< CMVP锁失败 */
    CRYPT_CMVP_ERR_ALGO_SELFTEST,         /**< 算法自检失败 */
    CRYPT_CMVP_ALREADY_IN_MODE,           /**< 已经处在该模式 */
    CRYPT_CMVP_ERR_PAIRWISETEST,          /**< 密钥配对测试失败 */
    CRYPT_CMVP_NOT_APPROVED,              /**< 不满足标准要求 */
    CRYPT_CMVP_ERR_INTEGRITY,             /**< 完整性验证失败 */

    CRYPT_PBKDF2_PARAM_ERROR = 0x01150001,       /**< 入参错误 */
    CRYPT_PBKDF2_NOT_SUPPORTED,                  /**< 不支持PBKDF2算法 */

    CRYPT_ECC_POINT_AT_INFINITY = 0x1160001,     /**< 点在无穷远 */
    CRYPT_ECC_POINT_NOT_ON_CURVE,                /**< 点不在曲线上 */
    CRYPT_ECC_POINT_ERR_CURVE_ID,                /**< 曲线ID不一致或者错误 */
    CRYPT_ECC_POINT_WINDOW_TOO_MAX,              /**< 窗口过大 */
    CRYPT_ECC_POINT_NOT_EQUAL,                   /**< 两个点不相等 */
    CRYPT_ECC_POINT_BLIND_WITH_ZERO,             /**< 点加盐时随机数生成值为0 */
    CRYPT_ECC_POINT_NOT_AFFINE,                  /**< 点不为仿射坐标 */
    CRYPT_ECC_NOT_SUPPORT,                       /**< 不支持该功能 */
    CRYPT_ECC_POINT_MUL_ADD_WITH_ZERO_K,          /**< 使用点乘加功能时，标量值为0返回错误 */
    CRYPT_ECC_POINT_MUL_ERR_K_LEN,               /**< 使用点乘功能时，标量长度超过曲线规格 */
    CRYPT_ECC_BUFF_LEN_NOT_ENOUGH,               /**< 缓冲区长度不足 */
    CRYPT_ECC_ERR_POINT_FORMAT,                  /**< 进行点编码时输入的编码格式不对 */
    CRYPT_ECC_ERR_POINT_CODE,                    /**< 错误的点编码信息 */
    CRYPT_ECC_PKEY_ERR_UNSUPPORTED_CTRL_OPTION,  /**< 不支持的控制类型 */
    CRYPT_ECC_PKEY_ERR_EMPTY_KEY,                /**< 秘钥为空 */
    CRYPT_ECC_PKEY_ERR_INVALID_POINT_FORMAT,     /**< 无效的点格式 */
    CRYPT_ECC_PKEY_ERR_CTRL_LEN,                 /**< 控制入参的长度不正确 */
    CRYPT_ECC_PKEY_ERR_INVALID_PRIVATE_KEY,      /**< 无效的私钥 */
    CRYPT_ECC_PKEY_ERR_INVALID_PUBLIC_KEY,       /**< 无效的公钥 */
    CRYPT_ECC_PKEY_ERR_TRY_CNT,                  /**< 秘钥生成、签名在规定的尝试次数内无法生成结果 */

    CRYPT_SHA3_OUT_BUFF_LEN_NOT_ENOUGH = 0x01170001, /**< 承载输出结果的缓冲区长度不足 */

    CRYPT_ECDH_ERR_UNSUPPORT_CURVE_TYPE = 0x01180001,       /**< 不支持的曲线类型 */
    CRYPT_ECDH_ERR_EMPTY_KEY,                               /**< 秘钥为空 */
    CRYPT_ECDH_ERR_EMPTY_PARA,                              /**< 曲线参数为空 */
    CRYPT_ECDH_ERR_UNSUPPORTED_CTRL_OPTION,                 /**< 不支持的控制选项 */
    CRYPT_ECDH_ERR_INVALID_COFACTOR,                        /**< 无效的cofactor值 */
    CRYPT_ECDH_ERR_INIFINITY_SHARE_KEY,                     /**< 计算出的share key为0 */

    CRYPT_ECDSA_ERR_EMPTY_KEY = 0x01190001,       /**< 秘钥为空 */
    CRYPT_ECDSA_ERR_TRY_CNT,                     /**< 秘钥生成、签名在规定的尝试次数内无法生成结果 */
    CRYPT_ECDSA_VERIFY_FAIL,                     /**< 验签失败 */
    CRYPT_ECDSA_ERR_UNSUPPORTED_CTRL_OPTION,     /**< 不支持的控制类型 */

    CRYPT_SM3_INPUT_OVERFLOW    = 0x11A0001,  /**< 输入数据的长度超出SM3的最大处理范围 */
    CRYPT_SM3_OUT_BUFF_LEN_NOT_ENOUGH,        /**< 承载输出结果的缓冲区长度不足 */

    CRYPT_SM4_ERR_IV_LEN = 0x011B0001,           /**< SM4 iv长度不对，不是16 */
    CRYPT_SM4_ERR_MSG_LEN,                       /**< SM4消息长度不对，不是16的倍数 */
    CRYPT_SM4_ERR_KEY_LEN,                       /**< SM4秘钥长度不对，不是16 */

    CRYPT_SM9_BUFF_LEN_NOT_ENOUGH = 0x011C0001,     /**< BUFF长度不足 */
    CRYPT_SM9_ERR_KEYTYPE,                          /**< 秘钥类型错误 */
    CRYPT_SM9_ERR_TRY_CNT,                          /**< 签名过程随机数生成次数在尝试次数内无法生成结果 */
    CRYPT_SM9_ERR_MD_METHOD,                        /**< 没有哈希方法信息 */
    CRYPT_SM9_VERIFY_FAIL,                          /**< 验签失败 */
    CRYPT_SM9_ERR_CTRL_TYPE,                        /**< 调用ctrl接口设置信息时ctrlType不对 */
    CRYPT_SM9_ERR_HASH_METHOD,                      /**< 调用ctrl接口设置的哈希方法不对 */
    CRYPT_SM9_POINT_AT_INFINITY,                    /**< 点在无穷远 */
    CRYPT_SM9_POINT_NOT_ON_CURVE,                   /**< 点不在曲线上 */
    CRYPT_SM9_ERR_POINT_FORMAT,                     /**< 点格式不正确 */
    CRYPT_SM9_ERR_POINT_BUFF,                       /**< 点数据不正确 */
    CRYPT_SM9_ERR_POINT_NOT_AFFINE,                 /**< 点不是仿射坐标 */
    CRYPT_SM9_ERR_POINTS2AFFINE_CNT,                /**< 多个点转到仿射坐标时点的数量错误 */
    CRYPT_SM9_FQ2_NOT_EQUAL,                        /**< fq2点信息比较不一致 */

    CRYPT_MD5_INPUT_OVERFLOW    = 0x11D0001,  /**< 输入数据的长度超出MD5的最大处理范围 */
    CRYPT_MD5_OUT_BUFF_LEN_NOT_ENOUGH,        /**< 承载输出结果的缓冲区长度不足 */
    
    CRYPT_TDES_ERR_KEYLEN = 0x11E0001,          /**< tdes秘钥长度错误 */
    CRYPT_TDES_ERR_KEY,                          /**< des秘钥错误 */
    CRYPT_TDES_ERR_LEN,                          /**< des加解密长度错误 */

    CRYPT_CURVE448_NO_PUBKEY         = 0x011F0001, /**< 没有公钥 */
    CRYPT_CURVE448_NO_PRVKEY,                      /**< 没有私钥 */
    CRYPT_CURVE448_KEYLEN_ERROR,                   /**< 秘钥长度错误 */
    CRYPT_CURVE448_SIGNLEN_ERROR,                  /**< 签名长度错误 */
    CRYPT_CURVE448_VERIFY_FAIL,                    /**< 签名不正确，验签失败 */
    CRYPT_CURVE448_NO_HASH_METHOD,                 /**< 未设置哈希方法 */
    CRYPT_CURVE448_UNSUPPORTED_CTRL_OPTION,        /**< 不支持的操作模式 */
    CRYPT_CURVE448_KEY_COMPUTE_FAILED,             /**< 生成共享密钥失败 */
    CRYPT_CURVE448_INVALID_PUBKEY,                 /**< 公钥非法 */
    CRYPT_CURVE448_CONTEXT_TOO_LONG,               /**< context超255 */
    CRYPT_CURVE448_NO_CONTEXT,                     /**< 未设置ctx */
    CRYPT_CURVE448_HASH_METH_ERROR,                /**< 哈希方法不是SHAKE256 */

    CRYPT_SM2_BUFF_LEN_NOT_ENOUGH = 0x01200001, /**< 缓冲区长度不足 */
    CRYPT_SM2_NO_PUBKEY,                        /**< SM2未设置公钥 */
    CRYPT_SM2_NO_PRVKEY,                        /**< SM2未设置私钥 */
    CRYPT_SM2_ERR_EMPTY_KEY,                    /**< SM2秘钥为空 */
    CRYPT_SM2_ERR_TRY_CNT,                      /**< 秘钥生成、签名在规定的尝试次数内无法生成结果 */
    CRYPT_SM2_VERIFY_FAIL,                      /**< 验签失败 */
    CRYPT_SM2_ERR_UNSUPPORTED_CTRL_OPTION,      /**< 不支持的控制类型 */
    CRYPT_SM2_ERR_CTRL_LEN,                    /**< 错误的ctrl长度 */
    CRYPT_SM2_ERR_NO_HASH_METHOD,               /**< 没有哈希方法信息 */
    CRYPT_SM2_HASH_METH_ERROR,                /**< 哈希方法不是SM3 */
    CRYPT_SM2_USERID_NOT_SET,                   /**< userID未设置*/
    CRYPT_SM2_R_NOT_SET,                       /**< 对端R值未设置*/
    CRYPT_SM2_INVALID_SERVER_TYPE,             /**< 用户既不是发起方也不是接收方 */
    CRYPT_SM2_INVALID_CHECK_TYPE,              /**< 错误的校验类型 */
    CRYPT_SM2_DECRYPT_FAIL,                    /**< 解密失败 */
    CRYPT_SM2_ERR_DATA_LEN,                    /**< 数据长度错误 */
    CRYPT_SM2_ERR_GET_S,                       /**< 获取校验和失败 */
    CRYPT_SM2_ERR_S_NOT_SET,                   /**< 未设置校验和 */
    CRYPT_SM2_EXCH_VERIFY_FAIL,                /**< 密钥协商失败 */

    CRYPT_KDFTLS12_NOT_SUPPORTED = 0x01210001,          /**< 不支持KDFTLS12算法 */

    CRYPT_SIPHASH_OUT_BUFF_LEN_NOT_ENOUGH = 0x01220001,           /**< 承载输出结果的缓冲区长度不足 */
    CRYPT_SIPHASH_INPUT_OVERFLOW,                                 /**< 输入数据的长度超出SIPHASH的最大处理范围 */
};

#ifdef __cplusplus
}
#endif

#endif
