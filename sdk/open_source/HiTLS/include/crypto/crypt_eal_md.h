/**
 * @defgroup    crypt_eal_md    crypt_eal_md.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       消息摘要(MD)算法对外接口文件
 */

#ifndef CRYPT_EAL_MD_H
#define CRYPT_EAL_MD_H

#include <stdbool.h>
#include <stdint.h>
#include "crypt_algid.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct EAL_MdCtx CRYPT_EAL_MdCTX;

/**
  * @ingroup crypt_eal_md
  * @brief 哈希合规判断
  *
  * @param   id [IN] 哈希算法ID
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_MdC2)(CRYPT_MD_AlgId id);

/**
 * @ingroup crypt_eal_md
 * @brief   哈希合规判断回调注册接口，用于判断算法参数是否合规和模块状态是否可用，EAL的哈希接口在执行操作前会调用该回调
 *            回调失败会导致CRYPT_EAL_MdNewCtx/CRYPT_EAL_MdInit/CRYPT_EAL_MdUpdate/CRYPT_EAL_MdFinal接口失败
 *          非核准模式下可以使用任意算法
 *          ISO19790模式下只允许CRYPT_MD_SHA224/CRYPT_MD_SHA256/CRYPT_MD_SHA384/CRYPT_MD_SHA512
 *
 * @param   func     [IN] 哈希合规判断回调
 *
 * @return  无
 */
void CRYPT_EAL_RegMdC2(CRYPT_EAL_MdC2 func);

/**
 * @ingroup crypt_eal_md
 * @brief   创建MD上下文，计算结束后需要调用CRYPT_EAL_MdFreeCtx接口释放内存。
 *
 * @param   id [IN] 算法ID
 *
 * @return  CRYPT_EAL_MdCTX MD上下文指针
 *          若失败返回空
 */
CRYPT_EAL_MdCTX *CRYPT_EAL_MdNewCtx(CRYPT_MD_AlgId id);

/**
 * @ingroup crypt_eal_md
 * @brief   MD上下文释放
 *
 * @param   ctx [IN] MD上下文，使用CRYPT_EAL_MdNewCtx接口创建, ctx由调用方置空
 *
 * @return  void 无返回值
 */
void CRYPT_EAL_MdFreeCtx(CRYPT_EAL_MdCTX *ctx);

/**
 * @ingroup crypt_eal_md
 * @brief   MD上下文初始化
 *
 * @param   ctx [IN/OUT] MD上下文，使用CRYPT_EAL_MdNewCtx接口创建。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_MdInit(CRYPT_EAL_MdCTX *ctx);

/**
 * @ingroup crypt_eal_md
 * @brief   持续输入待摘要的数据
 *
 * @param   ctx [IN/OUT] MD上下文，使用CRYPT_EAL_MdNewCtx接口创建。
 * @param   data [IN] 待摘要数据
 * @param   len [IN] 数据长度，sha384、sha512最大总长度限制为[0,2^128bits)，
 *                   sha1、sha224、sha256、sm3、md5最大总长度限制[0,2^64bits)。单次最大长度限制都[0,0xffffffff]。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_MdUpdate(CRYPT_EAL_MdCTX *ctx, const uint8_t *data, uint32_t len);

/**
 * @ingroup crypt_eal_md
 * @brief   完成摘要，输出最终摘要结果
 *
 * @param   ctx [IN/OUT] MD上下文，使用CRYPT_EAL_MdNewCtx接口创建。
 * @param   out [OUT] 摘要结果缓存，需要用户创建管理。
 * @param   len [IN/OUT] 入参标识out的buffer长度，出参标识获取的out有效长度。长度大小必须大于等于对应算法哈希长度。
 *                       哈希长度大小可以通过CRYPT_EAL_MdGetDigestSize接口获取。需要用户创建管理。
 *
 * @return  CRYPT_SUCCESS 成功
 *          其他错误码参考 crypt_errno.h
 */
int32_t CRYPT_EAL_MdFinal(CRYPT_EAL_MdCTX *ctx, uint8_t *out, uint32_t *len);

/**
 * @ingroup crypt_eal_md
 * @brief   获取算法输出的摘要长度
 *
 * @param   id [IN] 算法ID
 *
 * @return  成功返回摘要长度
 *          失败返回0，且仅有id不合法的情况下返回为0
 */
uint32_t CRYPT_EAL_MdGetDigestSize(CRYPT_MD_AlgId id);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CRYPT_EAL_MD_H
