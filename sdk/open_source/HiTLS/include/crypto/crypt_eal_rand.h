/**
 * @defgroup    crypt_eal_rand  crypt_eal_rand.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       RAND 算法对外接口文件
 */

#ifndef CRYPT_EAL_RAND_H
#define CRYPT_EAL_RAND_H

#ifndef HITLS_CRYPTO_NO_RAND
#include <stdbool.h>
#include <stdint.h>
#include "crypt_algid.h"
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @ingroup crypt_eal_rand
  * @brief 随机数合规判断
  *
  * id：随机数算法id
  *
  * @return  true 测试成功
  *          false 测试失败
  */
typedef bool (*CRYPT_EAL_RandC2)(CRYPT_RAND_AlgId id);

/**
 * @ingroup crypt_eal_rand
 * @brief   随机数算法合规判断注册接口，用于判断模块状态是否可用
 *            EAL的随机数接口在执行操作前会调用该回调，回调失败会导致CRYPT_EAL_RandInit/CRYPT_EAL_RandbytesWithAdin/
 *            CRYPT_EAL_Randbytes/CRYPT_EAL_RandSeedWithAdin/CRYPT_EAL_RandSeed接口失败
 *          非核准模式下可以使用任意算法
 *          ISO19790模式下允许CRYPT_MD_SHA224/CRYPT_MD_SHA256/CRYPT_MD_SHA384/CRYPT_MD_SHA512
 *          修改上面注释
 *
 * @param   func     [IN] 随机数合规判断
 *
 * @return  无
 */
void CRYPT_EAL_RegRandC2(CRYPT_EAL_RandC2 func);

/**
 * @ingroup crypt_eal_rand
 * @brief 随机数初始化接口，此接口不支持多线程
 * @par
 *      将全局随机数初始为HITLS实现的RAND，种子材料中的熵源和额外随机数，必须由用户提供
 *      使用场景如下:
 *      1. seedMeth == NULL ====> 报错
 *      2. seedMeth != NULL ====> 正常使用, seedCtx不做限制，但要保证seedMeth能够处理各种情况
 *
 * @attention 支持多线程获取/生成随机数，但是不支持多线程初始化和去初始化
 * @param id [IN] RAND id
 * @param seedMeth [IN] 种子方法，不可以为空
 * @param seedCtx [IN] 种子上下文信息，可以为空，但用户提供的seedMeth需要能够处理seedCtx为空的情况
 *                     seedCtx一般需要包含熵源entropy，额外随机数nonce等数据
 * @param pers [IN] 个人数据，可以为空
 * @param persLen [IN] 个人数据长度，长度限制为[0,0x7FFFFFF0]
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_RandInit(CRYPT_RAND_AlgId id, CRYPT_RandSeedMethod *seedMeth, void *seedCtx,
    const uint8_t *pers, uint32_t persLen);

/**
 * @ingroup crypt_eal_rand
 * @brief   CRYPT_EAL_RandDeinit 所对应的反初始化全局RAND接口, 此接口不支持多线程
 * @return void 无返回值
 */
void CRYPT_EAL_RandDeinit(void);

/**
 * @ingroup crypt_eal_rand
 * @brief 随机数生成，额外数据允许为空，由用户指定额外的混淆数据，此接口支持多线程访问
 *
 * @param byte  [OUT] 用于输出随机数，内存由用户提供
 * @param len   [IN] 所需随机数长度，限制长度为(0,65536]
 * @param addin [IN] 额外数据，可以为空
 * @param addinLen [IN] 额外数据长度，长度限制为[0,0x7FFFFFF0]
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_RandbytesWithAdin(uint8_t *byte, uint32_t len, uint8_t *addin, uint32_t addinLen);

/**
 * @ingroup crypt_eal_rand
 * @brief 随机数生成，等同于CRYPT_EAL_RandbytesWithAdin(bytes, len, NULL, 0)，此接口支持多线程访问
 *
 * @param byte [OUT] 用于存储输出的随机数，内存由用户提供
 * @param len  [IN] 所需随机数长度, 限制长度为(0,65536]
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_Randbytes(uint8_t *byte, uint32_t len);

/**
 * @ingroup crypt_eal_rand
 * @brief 重新生成种子，额外数据允许为空，此接口支持多线程访问
 *
 * @param addin [IN] 额外数据，可以为空
 * @param addinLen [IN] 额外数据长度， 长度限制为[0,0x7FFFFFF0]
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_RandSeedWithAdin(uint8_t *addin, uint32_t addinLen);

/**
 * @ingroup crypt_eal_rand
 * @brief 重新生成种子，等同于CRYPT_EAL_RandSeedWithAdin(NULL, 0)，此接口支持多线程访问
 *
 * @return 成功返回：CRYPT_SUCCESS
 *         其他错误码，请查询crypt_errno.h
 */
int32_t CRYPT_EAL_RandSeed(void);

#ifdef __cplusplus
}
#endif
#endif
#endif
