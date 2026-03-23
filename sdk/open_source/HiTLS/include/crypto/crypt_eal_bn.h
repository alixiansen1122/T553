/**
 * @defgroup    crypt_eal_bn crypt_eal_bn.h
 * @ingroup     crypto
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * @brief       大数运算对外接口
 */

#ifndef CRYPT_EAL_BN_H
#define CRYPT_EAL_BN_H

#include <stdbool.h>
#include <stdint.h>
#include "crypt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BigNum CRYPT_BN_BigNum;

typedef struct BnMont CRYPT_BN_Mont;

typedef struct BnOptimizer CRYPT_BN_Optimizer;

/**
 * @ingroup crypt_eal_bn
 * @brief   大数创建
 *
 * @param   bits [IN] 位数
 *
 * @retval 非NULL 成功
 * @retval NULL 失败
 */
CRYPT_BN_BigNum *CRYPT_EAL_BN_Create(uint32_t bits);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数销毁
 *
 * @param   a [IN] 大数
 *
 * @retval 无
 */
void CRYPT_EAL_BN_Destroy(CRYPT_BN_BigNum *a);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数优化器创建
 *
 * @retval 非NULL 成功
 * @retval NULL 失败
 */
CRYPT_BN_Optimizer *CRYPT_EAL_BN_OptimizerCreate(void);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数优化器销毁
 *
 * @param   opt [IN] 大数优化器
 *
 * @retval 无
 */
void CRYPT_EAL_BN_OptimizerDestroy(CRYPT_BN_Optimizer *opt);

/**
 * @ingroup crypt_eal_bn
 * @brief   设置符号
 *
 * @param   a [IN] 大数
 * @param   sign [IN] 符号，true为负数 false为正数
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_NO_NEGATIVE_ZERO 0不能设置为负数符号
 */
int32_t CRYPT_EAL_BN_SetSign(CRYPT_BN_BigNum *a, bool sign);

/**
 * @ingroup crypt_eal_bn
 * @brief   判断大数的值是否为1
 *
 * @attention 大数入参不能为空指针
 * @param   a [IN] 大数
 *
 * @retval  true,大数的值为1
 * @retval  false,大数的值不为1
 *
 */
bool CRYPT_EAL_BN_IsOne(const CRYPT_BN_BigNum *a);

/**
 * @ingroup crypt_eal_bn
 * @brief   判断大数是否为负数
 *
 * @attention 大数入参不能为空指针
 * @param   a [IN] 大数
 *
 * @retval  true,大数的值为负数
 * @retval  false,大数的值不为负数
 *
 */
bool CRYPT_EAL_BN_IsNegative(const CRYPT_BN_BigNum *a);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数加法
 *
 * @param   r [OUT] 和
 * @param   a [IN] 加数
 * @param   b [IN] 加数
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 */
int32_t CRYPT_EAL_BN_Add(CRYPT_BN_BigNum *r, const CRYPT_BN_BigNum *a, const CRYPT_BN_BigNum *b);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数减法
 *
 * @param   r [OUT] 差
 * @param   a [IN] 被减数
 * @param   b [IN] 减数
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 */
int32_t CRYPT_EAL_BN_Sub(CRYPT_BN_BigNum *r, const CRYPT_BN_BigNum *a, const CRYPT_BN_BigNum *b);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数乘法
 *
 * @param   r [OUT] 乘积
 * @param   a [IN] 乘数
 * @param   b [IN] 乘数
 * @param   opt [IN] 优化器
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 * @retval  CRYPT_CRYPT_BN_Optimizer_GET_FAIL 从优化器申请空间失败
 */
int32_t CRYPT_EAL_BN_Mul(
    CRYPT_BN_BigNum *r, const CRYPT_BN_BigNum *a, const CRYPT_BN_BigNum *b, CRYPT_BN_Optimizer *opt);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数除法
 *
 * @param   q [OUT] 商
 * @param   r [OUT] 余数
 * @param   x [IN] 被除数
 * @param   y [IN] 除数
 * @param   opt [IN] 优化器
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 * @retval  CRYPT_CRYPT_BN_Optimizer_GET_FAIL 从优化器申请空间失败
 * @retval  CRYPT_BN_ERR_DIVISOR_ZERO 除数不能为0
 */
int32_t CRYPT_EAL_BN_Div(CRYPT_BN_BigNum *q, CRYPT_BN_BigNum *r, const CRYPT_BN_BigNum *x, const CRYPT_BN_BigNum *y,
    CRYPT_BN_Optimizer *opt);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数求最大公约数
 * @par 描述: gcd(a,b) (a、b!=0)
 *
 * @param   r [OUT] 最大公约数
 * @param   a [IN] 大数
 * @param   b [IN] 大数
 * @param   opt [IN] 优化器
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 * @retval  CRYPT_CRYPT_BN_Optimizer_GET_FAIL 从优化器申请空间失败
 * @retval  CRYPT_BN_ERR_GCD_NO_ZERO 求最大公约数不能有0
 */
int32_t CRYPT_EAL_BN_Gcd(
    CRYPT_BN_BigNum *r, const CRYPT_BN_BigNum *a, const CRYPT_BN_BigNum *b, CRYPT_BN_Optimizer *opt);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数模逆
 *
 * @param   r [OUT] 结果
 * @param   x [IN] 大数
 * @param   m [IN] 模
 * @param   opt [IN] 优化器
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 * @retval  CRYPT_CRYPT_BN_Optimizer_GET_FAIL 从优化器申请空间失败
 * @retval  CRYPT_BN_ERR_NO_INVERSE 无法求出模逆
 */
int32_t CRYPT_EAL_BN_ModInv(
    CRYPT_BN_BigNum *r, const CRYPT_BN_BigNum *x, const CRYPT_BN_BigNum *m, CRYPT_BN_Optimizer *opt);

/**
 * @ingroup crypt_eal_bn
 * @brief   二进制转大数
 *
 * @param   r [OUT] 大数
 * @param   bin [IN] 待转换数据流
 * @param   binLen [IN] 数据流长度
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 */
int32_t CRYPT_EAL_BN_Bin2Bn(CRYPT_BN_BigNum *r, const uint8_t *bin, uint32_t binLen);

/**
 * @ingroup crypt_eal_bn
 * @brief   大数转二进制
 *
 * @param   a [IN] 大数
 * @param   bin [IN/OUT] 待转换数据流 --传入指针不能为空
 * @param   binLen [IN/OUT] 数据流长度 --传入时也是bin缓存区的长度
 *
 * @retval  CRYPT_SUCCESS
 * @retval  CRYPT_NULL_INPUT 无效空指针
 * @retval  CRYPT_BN_SPACE_NOT_ENOUGH 空间不够
 * @retval  CRYPT_SECUREC_FAIL 拷贝出错
 */
int32_t CRYPT_EAL_BN_Bn2Bin(const CRYPT_BN_BigNum *a, uint8_t *bin, uint32_t *binLen);

#ifdef __cplusplus
}
#endif

#endif