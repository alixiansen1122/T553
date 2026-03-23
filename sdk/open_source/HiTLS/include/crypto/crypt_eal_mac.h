/**
 * @defgroup    crypt_eal_mac    crypt_eal_mac.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief    MAC算法接口，包含HMAC、CMAC
 */
#ifndef CRYPT_EAL_MAC_H
#define CRYPT_EAL_MAC_H

#ifndef HITLS_CRYPTO_NO_MAC
#include <stdbool.h>
#include <stdint.h>
#include "crypt_algid.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EAL_MacCtx CRYPT_EAL_MacCtx;

/**
  * @ingroup crypt_eal_mac
  * @brief MAC合规判断
  *
  * @param   id [IN] MAC算法id
  *                 非核准模式下可以使用任意算法
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_MacC2)(CRYPT_MAC_AlgId id);

/**
 * @ingroup crypt_eal_mac
 * @brief   MAC合规判断回调注册接口，用于判断算法参数是否合规和模块状态是否可用
 *            EAL的哈希接口在执行操作前会调用该回调，回调失败会导致CRYPT_EAL_MacNewCtx/CRYPT_EAL_MacInit/
 *            CRYPT_EAL_MacUpdate/CRYPT_EAL_MacFinal/CRYPT_EAL_MacReinit接口失败
 *          非核准模式下，该验证不生效
 *          ISO19790模式下只允许CRYPT_MAC_HMAC_SHA1/CRYPT_MAC_HMAC_SHA224/CRYPT_MAC_HMAC_SHA256/
 *            CRYPT_MAC_HMAC_SHA384/CRYPT_MAC_HMAC_SHA512/CRYPT_MAC_CMAC_AES128/
 *            CRYPT_MAC_CMAC_AES192/CRYPT_MAC_CMAC_AES256
 *
 * @param   func     [IN] MAC合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegMacC2(CRYPT_EAL_MacC2 func);


/**
 * @ingroup crypt_eal_mac
 * @brief   申请MAC上下文
 *
 * @param   id [IN] MAC算法ID
 *
 * @return  CRYPT_EAL_MacCtx指针
 *          若失败返回NULL
 */
CRYPT_EAL_MacCtx *CRYPT_EAL_MacNewCtx(CRYPT_MAC_AlgId id);

/**
 * @ingroup crypt_eal_mac
 * @brief   释放MAC上下文内存
 *
 * @param   ctx [IN] MAC上下文, ctx由调用方置空
 */
void CRYPT_EAL_MacFreeCtx(CRYPT_EAL_MacCtx *ctx);

/**
 * @ingroup crypt_eal_mac
 * @brief  MAC算法初始化上下文，在调用CRYPT_EAL_MacNewCtx接口之后使用。
 *         在计算过程中任何时候都可以使用初始化接口，但注意，调用了初始化接口上一次的计算数据全部清零。
 *
 * @param   ctx [IN] MAC上下文
 * @param   key [IN] 密钥，长度规格如下:
 *                   HMAC:长度为大于等于0的任意整数, 其中HMAC-SHA1、HMAC-SHA224、HMAC-SHA256长度为小于2^64bits，
 *                        HMAC-SHA384、HMAC-SHA512长度为小于2^128bits
 *                   CMAC:CMAC-AES128 长度需要等于128bits、CMAC-AES192 长度需要等于192bits、
 *                        CMAC-AES256 长度需要等于256bits
 * @param   len [IN] 密钥长度
 *
 * @retval #CRYPT_SUCCESS 初始化成功
 * @retval #CRYPT_NULL_INPUT 指针ctx参数或者key参数为NULL
 * @retval #CRYPT_AES_ERR_KEYLEN  AES CMAC算法密钥长度错误
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_MacInit(CRYPT_EAL_MacCtx *ctx, const uint8_t *key, uint32_t len);

/**
 * @ingroup crypt_eal_mac
 * @brief   持续输入待mac的数据。需要在接口CRYPT_EAL_MacInit接口执行成功之后使用。
 *
 * @param   ctx [IN] MAC上下文
 * @param   in  [IN] 输入数据，当该变量为NULL时，len参数必须为0，否则报错
 * @param   len [IN] 输入数据长度，可以为0长度。
 *
 * @retval #CRYPT_SUCCESS  更新摘要内部状态成功
 * @retval #CRYPT_NULL_INPUT  输入参数为NULL
 * @retval #CRYPT_EAL_ERR_STATE 状态错误
 * @retval #CRYPT_SHA1_INPUT_OVERFLOW  HMAC-SHA1输入数据的长度超出最大值
 * @retval #CRYPT_SHA2_INPUT_OVERFLOW  HMAC-SHA224、HMAC-SHA256、HMAC-SHA384、HMAC-SHA512输入数据的长度超出最大值
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_MacUpdate(CRYPT_EAL_MacCtx *ctx, const uint8_t *in, uint32_t len);

/**
 * @ingroup crypt_eal_mac
 * @brief   输出MAC结果。需要在接口CRYPT_EAL_MacInit接口执行成功之后使用，过程中可以不调用CRYPT_EAL_MacUpdate接口。
 *          MAC输出长度，HMAC-SHA1对应20bytes、HMAC-SHA224对应28bytes、HMAC-SHA256对应32bytes、
 *          HMAC-SHA384对应48bytes、HMAC-SHA512对应64bytes、CMAC-AES对应的16bytes
 *
 * @param   ctx [IN] MAC上下文
 * @param   out [OUT] 输出数据，必须分配足够大的内存用于存储MAC结果，不能为NULL。
 * @param   len [IN/OUT] 输出数据长度，入参该值需要指定out长度，该长度需要大于等于MAC生成的长度，出参为MAC 输出的长度.
 *
 * @retval #CRYPT_SUCCESS  计算成功
 * @retval #CRYPT_NULL_INPUT  输入参数为NULL
 * @retval #CRYPT_EAL_ERR_STATE 状态错误
 * @retval #CRYPT_HMAC_OUT_BUFF_LEN_NOT_ENOUGH  HMAC算法输出buffer长度不足
 * @retval #CRYPT_CMAC_OUT_BUFF_LEN_NOT_ENOUGH  CMAC算法输出buffer长度不足
 * @retval #CRYPT_SHA1_INPUT_OVERFLOW  HMAC-SHA1输入数据的长度超出最大值
 * @retval #CRYPT_SHA2_INPUT_OVERFLOW  HMAC-SHA224、HMAC-SHA256、HMAC-SHA384、HMAC-SHA512输入数据的长度超出最大值
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_MacFinal(CRYPT_EAL_MacCtx *ctx, uint8_t *out, uint32_t *len);

/**
 * @ingroup crypt_eal_mac
 * @brief   去初始化函数，调用后如果要计算则需要重新初始化。
 * @param   ctx [IN] MAC上下文
 */
void CRYPT_EAL_MacDeinit(CRYPT_EAL_MacCtx *ctx);

/**
 * @ingroup crypt_eal_mac
 * @brief  使用ctx中保留的信息重新初始化。不需要重新调用init接口初始化。相当与deinit和init两个接口的结合。
 * @param   ctx [IN] MAC上下文
 * @retval #CRYPT_SUCCESS  reinit成功
 * @retval #CRYPT_NULL_INPUT  输入参数为NULL
 */
int32_t CRYPT_EAL_MacReinit(CRYPT_EAL_MacCtx *ctx);

/**
 * @ingroup crypt_eal_mac
 * @brief   通过上下文获取对应算法的输出MAC长度
 * @param   ctx [IN] MAC上下文
 * @return  上下文对应的MAC长度
 */
uint32_t CRYPT_EAL_GetMacLen(const CRYPT_EAL_MacCtx *ctx);

/**
 * @ingroup crypt_eal_mac
 * @brief   设置算法参数，该接口需要在CRYPT_EAL_MacInit接口调用之后使用，且该接口仅支持GMAC算法
 * <table border="1">
 * <tr><th>         参数              </th><th>  数据类型      </th><th>  len为长度,in的字节数      </th></tr>
 * <tr><td>  CRYPT_CTRL_SET_IV        </td><td>  uint8_t数组   </td><td>  IV的长度                  </td></tr>
 * <tr><td>  CRYPT_CTRL_SET_TAGLEN    </td><td>  uint32_t      </td><td>  4字节，sizeof(uint32_t)   </td></tr>
 * </table>
 * @param   ctx [IN] MAC上下文
 * @param   type [IN] 设置参数类型，当前仅支持两种操作码，CRYPT_CTRL_SET_IV 和 CRYPT_CTRL_SET_TAGLEN
 * @param   in [IN] 输入数据
 * @param   len [IN] 输入数据长度
 * @retval #CRYPT_SUCCESS  参数设置成功
 * @retval #CRYPT_EAL_ERR_STATE 状态错误
 * @retval #CRYPT_EAL_MAC_CTRL_TYPE_ERROR 设置参数类型错误
 * @retval #CRYPT_EAL_ERR_ALGID  算法ID为非GMAC
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_MacCtrl(CRYPT_EAL_MacCtx *ctx, uint32_t type, void *in, uint32_t len);
#ifdef __cplusplus
}   // end extern "C"
#endif

#endif
#endif
