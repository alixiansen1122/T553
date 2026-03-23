/**
 * @defgroup    crypt_eal_cipher crypt_eal_cipher.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       对称加解密算法对外接口
 */

#ifndef CRYPT_EAL_CIPHER_H
#define CRYPT_EAL_CIPHER_H

#ifndef HITLS_CRYPTO_NO_CIPHER
#include <stdint.h>
#include <stdbool.h>
#include "crypt_algid.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct CRYPT_EAL_CipherCtxLocal CRYPT_EAL_CipherCtx;

/**
  * @ingroup crypt_eal_cipher
  * @brief 对称合规判断
  *
  * @param id [IN] 对称算法id
  *
  * @return  true 成功
  *          false 失败
  */
typedef bool (*CRYPT_EAL_CipherC2)(CRYPT_CIPHER_AlgId id);

/**
 * @ingroup crypt_eal_cipher
 * @brief   对称合规判断回调注册接口，用于判断算法参数是否合规和模块状态是否可用，EAL的哈希接口在执行操作前会调用该回调
 *            回调失败会导致CRYPT_EAL_CipherNewCtx/CRYPT_EAL_CipherInit/CRYPT_EAL_CipherReinit/
 *            CRYPT_EAL_CipherUpdate/CRYPT_EAL_CipherFinal/CRYPT_EAL_CipherCtrl/CRYPT_EAL_CipherSetPadding接口失败
 *          非核准模式下可以使用任意算法
 *          ISO19790模式下只允许CRYPT_CIPHER_AES128_CBC/CRYPT_CIPHER_AES192_CBC/CRYPT_CIPHER_AES256_CBC/
 *            CRYPT_CIPHER_AES128_CTR/CRYPT_CIPHER_AES192_CTR/CRYPT_CIPHER_AES256_CTR/
 *            CRYPT_CIPHER_AES128_ECB/CRYPT_CIPHER_AES192_ECB/CRYPT_CIPHER_AES256_ECB/
 *            CRYPT_CIPHER_AES128_XTS/CRYPT_CIPHER_AES256_XTS
 *
 * @param   func     [IN] 对称合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegCipherC2(CRYPT_EAL_CipherC2 func);

/**
 * @ingroup crypt_eal_cipher
 * @brief 生成对称加解密句柄
 *
 * @param id [IN] 对称加解密算法id
 * @return 成功返回：cipher ctx
 *         失败返回：NULL
 */
CRYPT_EAL_CipherCtx* CRYPT_EAL_CipherNewCtx(CRYPT_CIPHER_AlgId id);

/**
 * @ingroup crypt_eal_cipher
 * @brief 释放对称加解密句柄，先清除敏感信息再释放
 *
 * @param ctx [IN] 对称加解密句柄, ctx由调用方置空
 * @return 无
 */
void CRYPT_EAL_CipherFreeCtx(CRYPT_EAL_CipherCtx *ctx);

/**
 * @ingroup crypt_eal_cipher
 * @brief 初始化对称加解密句柄， key不能为NULL;除了ECB模式，其他模式iv不能为NULL
 * iv的长度必须与块长度一致(ECB模式不做此要求)，
 * 块长度可以通过CRYPT_EAL_CipherCtrl的CRYPT_CTRL_GET_BLOCKSIZE获取。
 * CRYPT_EAL_CipherInit 在任意阶段可以重复调用，会重置key和iv, 并清除缓存数据。
 * <table border="1">
 * <tr><th>         算法ID            </th><th>  keyLen  </th><th>  ivLen  </th></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_CBC  </td><td>   16     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_CBC  </td><td>   24     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_CBC  </td><td>   32     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_CTR  </td><td>   16     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_CTR  </td><td>   24     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_CTR  </td><td>   32     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_ECB  </td><td>   16     </td><td>  0      </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_ECB  </td><td>   24     </td><td>  0      </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_ECB  </td><td>   32     </td><td>  0      </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_XTS  </td><td>   32     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_XTS  </td><td>   64     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_CCM  </td><td>   16     </td><td>  7 <= ivLen <= 13     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_CCM  </td><td>   24     </td><td>  7 <= ivLen <= 13     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_CCM  </td><td>   32     </td><td>  7 <= ivLen <= 13     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_GCM  </td><td>   16     </td><td>  1 ≤ ivLen ≤ 2^32 - 1 (byte)     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_GCM  </td><td>   24     </td><td>  1 ≤ ivLen ≤ 2^32 - 1 (byte)     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_GCM  </td><td>   32     </td><td>  1 ≤ ivLen ≤ 2^32 - 1 (byte)     </td></tr>
 * <tr><td>  CRYPT_CIPHER_CHACHA20_POLY1305  </td><td>   32     </td><td>  8或12     </td></tr>
 * <tr><td>  CRYPT_CIPHER_SM4_CBC     </td><td>   16     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_SM4_ECB     </td><td>   16     </td><td>  0      </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_CFB  </td><td>   16     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_CFB  </td><td>   24     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_CFB  </td><td>   32     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES128_OFB  </td><td>   16     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES192_OFB  </td><td>   24     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_AES256_OFB  </td><td>   32     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_TDES_ECB    </td><td>   24     </td><td>  0      </td></tr>
 * <tr><td>  CRYPT_CIPHER_TDES_CBC    </td><td>   24     </td><td>  8或24      </td></tr>
 * <tr><td>  CRYPT_CIPHER_TDES_OFB    </td><td>   24     </td><td>  8或24      </td></tr>
 * <tr><td>  CRYPT_CIPHER_TDES_CFB    </td><td>   24     </td><td>  8或24      </td></tr>
 * <tr><td>  CRYPT_CIPHER_SM4_CTR     </td><td>   16     </td><td>  16     </td></tr>
 * <tr><td>  CRYPT_CIPHER_SM4_GCM     </td><td>   16     </td><td>  1 ≤ ivLen ≤ 2^32 - 1 (byte)     </td></tr>
 * </table>
 *
 * @param ctx [IN] 对称加解密句柄
 * @param key [IN] 密钥
 * @param keyLen [IN] 密钥长度
 * @param iv [IN] 初始化向量
 * @param ivLen [IN] 初始化向量长度
 * @param enc [IN] true 为加密， false 为解密
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_CipherInit(CRYPT_EAL_CipherCtx *ctx, const uint8_t *key, uint32_t keyLen, const uint8_t *iv,
    uint32_t ivLen, bool enc);

/**
 * @ingroup crypt_eal_cipher
 * @brief 反初始化句柄，还原句柄到刚调用完CRYPT_EAL_CipherNewCtx的状态
 *
 * @param ctx [IN] 对称加解密句柄
 * @return 无
 */
void CRYPT_EAL_CipherDeinit(CRYPT_EAL_CipherCtx *ctx);

/**
 * @ingroup crypt_eal_cipher
 * @brief 重新初始化句柄，保留 key, 重新设置iv, 清除缓存、敏感数据； 除了ECB模式，其他模式iv不能为NULL
 * iv的设置需要结合对应的算法ID， 参见CRYPT_EAL_CipherInit中的对应关系。
 *
 * @param ctx [IN] 对称加解密句柄
 * @param iv [IN] 向量
 * @param ivlen [IN] 向量的长度
 * @return 无
 */
int32_t CRYPT_EAL_CipherReinit(CRYPT_EAL_CipherCtx *ctx, uint8_t *iv, uint32_t ivLen);

/**
 * @ingroup crypt_eal_cipher
 * @brief 持续输入加密、解密数据
 * CRYPT_EAL_CipherUpdate 应与CRYPT_EAL_CipherFinal配合使用，在一次或多次调用完CRYPT_EAL_CipherUpdate后，
 * 调用CRYPT_EAL_CipherFinal 获取最终的加密数据。
 *
 * @param ctx [IN] 对称加解密句柄
 * @param in [IN] 持续输入的数据
 * @param inLen [IN] 持续输入数据的长度
 * @param out [OUT] 输出的数据
 * @param outLen [IN/OUT] 输入： CBC、ECB块加密 建议输入outLen > inLen + blockSize， CTR、XTS流加密 建议 outLen >= inLen
 * blockSize 可使用CRYPT_EAL_CipherCtrl 的CRYPT_CTRL_GET_BLOCKSIZE获取
 * 输出 ： 输出加密数据的长度， 如果是块加密算法且最后待处理数据不足块长度时，outLen输出为0;
 * eg: CBC、ECB块加密
 *     1. 首次输入加密数据，且inLen < blockSize                          此时outLen输出为0
 *     2. 首次输入的加密数据长度inLen为blockSize的整数倍                      此时outLen 等于 inLen
 *     3. 首次输入的加密数据长度inLen > blockSize 且不是blockSize的整数倍     此时outLen < inLen
 *     4. 多次输入加密数据，(inLen % blockSize) + cache(ctx缓存数据) >= blockSize
 *     此时outLen = (inlen / blockSize) * blockSize + blockSize
 *     CTR outLen 等于 inLen
 *     XTS 模式update会预留最后2 个block到final进行处理, 如果输入数据加上缓存的总长度不足2 * blocksize， 则输出为0
 *     1. 首次输入数据          此时outLen = (inLen / blocksize - 2) * blocksize
 *     2. 多次输入加密数据      此时outLen = ((inLen + cache) / blocksize - 2) * blocksize
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_CipherUpdate(CRYPT_EAL_CipherCtx *ctx, const uint8_t *in, uint32_t inLen, uint8_t *out,
    uint32_t *outLen);

/**
 * @ingroup crypt_eal_cipher
 * @brief 对不足块大小的数据进行充填，输出加密数据;AEAD的tag通过CRYPT_EAL_CipherCtrl获取
 * 对于CBC、ECB等块加密算法，必须设置padding, XTS模式需要调用final得到最后2个block数据
 *
 * @param ctx [IN] 对称加解密句柄
 * @param out [OUT] 输出加密数据
 * @param outLen [IN/OUT] 输入：outLen >= blockSize
 * 输出：流加密输出为0。
 *      CBC、ECB块加密设置了padding则outLen输出为blockSize
 *      CBC、ECB块加密没设置padding, 且ctx中有缓存数据，则报错
 *      CBC、ECB块加密没设置padding，且ctx中无缓存数据， outLen输出为0
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_CipherFinal(CRYPT_EAL_CipherCtx *ctx, uint8_t *out, uint32_t *outLen);

/**
 * @ingroup crypt_eal_cipher
 * @brief 设置ctx里面的mode ctx的参数
 * <table border="1">
 * <tr><th>         参数               </th><th>  数据类型       </th><th>  len的长度（data的字节数）  </th></tr>
 * <tr><td>  CRYPT_CTRL_GET_IV         </td><td>  uint8_t数组   </td><td>  IV的长度跟对应算法有关，参见 CRYPT_EAL_CipherInit 中的对应关系  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_IV         </td><td>  uint8_t数组   </td><td>  IV的长度跟对应算法有关，参见 CRYPT_EAL_CipherInit 中的对应关系  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_AAD        </td><td>  uint8_t数组   </td><td>  仅限AEAD计算使用，长度和对应AEAD算法有关  </td></tr>
 * <tr><td>  CRYPT_CTRL_GET_TAG        </td><td>  uint8_t数组   </td><td>  仅限AEAD计算使用，长度为用户设置的tagLen  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_COUNT     </td><td>  uint8_t[4]      </td><td>  len长度为 4  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_TAGLEN     </td><td>  uint32_t      </td><td>  len长度为 4  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_MSGLEN     </td><td>  uint64_t      </td><td>  len长度为 8  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_FEEDBACKSIZE     </td><td>  uint32_t      </td><td>  len长度为 4  </td></tr>
 * <tr><td>  CRYPT_CTRL_GET_FEEDBACKSIZE     </td><td>  uint32_t指针      </td><td>  sizeof(*uint32_t)  </td></tr>
 * <tr><td>  CRYPT_CTRL_GET_BLOCKSIZE  </td><td>  uint32_t      </td><td>  len长度为 4 </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_SM4_CONSTTIME     </td><td>  NULL      </td><td>  0  </td></tr>
 * </table>
 * @param ctx [IN] 对称加解密句柄
 * @param type [IN] 参数类型
 * @param data [IN/OUT] 输入、输出数据
 * @param len [OUT] 数据长度
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_CipherCtrl(CRYPT_EAL_CipherCtx *ctx, CRYPT_CipherCtrl type, void *data, uint32_t len);

/**
 * @ingroup crypt_eal_cipher
 * @brief 设置padding模式
 *
 * @param ctx 对称加解密句柄
 * @param type padding类型
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_CipherSetPadding(CRYPT_EAL_CipherCtx *ctx, CRYPT_PaddingType type);

/**
 * @ingroup crypt_eal_cipher
 * @brief 获取padding类型
 *
 * @param ctx 对称加解密句柄
 * @return 返回模式
 */
int32_t CRYPT_EAL_CipherGetPadding(CRYPT_EAL_CipherCtx *ctx);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#endif
