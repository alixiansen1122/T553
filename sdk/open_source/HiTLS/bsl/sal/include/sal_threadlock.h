/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file sal_threadlock.h
 * Description: 线程锁模块
 * Author: guzhou
 * Create: 2022-02-11
 */

#ifndef SAL_THREADLOCK_H
#define SAL_THREADLOCK_H

#include <stdint.h>
#include "bsl_sal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup SAL
 * @brief 新建一个线程锁
 *
 * @par 描述：
 * 新建一个线程锁，如果是单线程环境则总是返回成功
 *
 * @attention 无
 * @param lock [IN/OUT] 锁句柄
 * @retval #BSL_SUCCESS 新建成功，或者是单线程环境
 * @retval #BSL_SAL_ERR_NO_MEMORY 内存空间不足，无法申请进程锁空间
 * @retval #BSL_SAL_ERR_UNKNOWN 线程锁初始化失败
 * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
 */
int32_t SAL_ThreadLockNew(BSL_SAL_ThreadLockHandle *lock);

/**
 * @ingroup SAL
 * @brief 锁住读操作
 *
 * @par 描述：
 * 锁住读操作，如果是单线程环境则总是返回成功
 *
 * @attention 无
 * @param lock [IN] 锁句柄
 * @retval #BSL_SUCCESS 成功，或者是单线程环境
 * @retval #BSL_SAL_ERR_UNKNOWN 操作失败
 * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
 */
int32_t SAL_ThreadReadLock(BSL_SAL_ThreadLockHandle lock);

/**
 * @ingroup SAL
 * @brief 锁住写操作
 *
 * @par 描述：
 * 锁住写操作，如果是单线程环境则总是返回成功
 *
 * @attention 无
 * @param lock [IN] 锁句柄
 * @retval #BSL_SUCCESS 成功，或者是单线程环境
 * @retval #BSL_SAL_ERR_UNKNOWN 操作失败
 * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
 */
int32_t SAL_ThreadWriteLock(BSL_SAL_ThreadLockHandle lock);

/**
 * @ingroup SAL
 * @brief 解锁
 *
 * @par 描述：
 * 解锁，如果是单线程环境则总是返回成功
 *
 * @attention 无
 * @param lock [IN] 锁句柄
 * @retval #BSL_SUCCESS 成功，或者是单线程环境
 * @retval #BSL_SAL_ERR_UNKNOWN 操作失败
 * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
 */
int32_t SAL_ThreadUnlock(BSL_SAL_ThreadLockHandle lock);

/**
 * @ingroup SAL
 * @brief 释放线程锁
 *
 * @par 描述：
 * 释放线程锁
 *
 * @attention 无
 * @param lock [IN] 锁句柄
 */
void SAL_ThreadLockFree(BSL_SAL_ThreadLockHandle lock);

/**
 * @ingroup SAL
 * @brief 获取线程id
 *
 * @par 描述：
 * 获取线程id
 *
 * @attention 无
 * @retval 线程id，单线程环境为0
 */
uint64_t SAL_ThreadGetId(void);

#ifdef __cplusplus
}
#endif

#endif