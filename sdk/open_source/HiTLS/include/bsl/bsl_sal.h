/**
 * @defgroup    bsl_sal bsl_sal.h
 * @ingroup     bsl
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       系统抽象层，由产品注册系统相关的函数
 */

#ifndef BSL_SAL_H
#define BSL_SAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup bsl_sal
 *
 * 产品为内存分配/释放可注册函数结构体
 */
typedef struct MemCallback {
    /**
     * @ingroup bsl_sal
     * @brief 分配一块内存
     *
     * @par 描述：
     * 分配一块内存
     *
     * @param size [IN] 分配内存的大小
     * @retval 非NULL 内存分配成功，被分配内存的首地址
     * @retval NULL 内存分配失败
     */
    void *(*pfMalloc)(uint32_t size);

    /**
     * @ingroup bsl_sal
     * @brief 回收由pfMalloc分配的一块内存
     *
     * @par 描述：
     * 回收由pfMalloc分配的一块内存
     *
     * @param addr [IN] 由pfMalloc分配的内存首地址
     */
    void (*pfFree)(void *addr);
} BSL_SAL_MemCallback;

/**
 * @ingroup bsl_sal
 *
 * 线程锁句柄，在产品注册时由产品提供相应结构体
 */
typedef void *BSL_SAL_ThreadLockHandle;

/**
 * @ingroup bsl_sal
 *
 * 产品为线程相关操作可注册函数结构体
 */
typedef struct ThreadCallback {
    /**
     * @ingroup bsl_sal
     * @brief 新建一个线程锁
     *
     * @par 描述：
     * 新建一个线程锁
     *
     * @param lock [IN/OUT] 锁句柄
     * @retval #BSL_SUCCESS 新建成功
     * @retval #BSL_SAL_ERR_NO_MEMORY 内存空间不足，无法申请线程锁空间
     * @retval #BSL_SAL_ERR_UNKNOWN 线程锁初始化失败
     * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
     */
    int32_t (*pfThreadLockNew)(BSL_SAL_ThreadLockHandle *lock);

    /**
    * @ingroup bsl_sal
    * @brief 释放线程锁
    *
    * @par 描述：
    * 释放线程锁，要保证在有其它线程获得锁的情况下，释放锁是没有问题的
    *
    * @param lock [IN] 锁句柄
    */
    void (*pfThreadLockFree)(BSL_SAL_ThreadLockHandle lock);

    /**
     * @ingroup bsl_sal
     * @brief 锁住读操作
     *
     * @par 描述：
     * 锁住读操作
     *
     * @param lock [IN] 锁句柄
     * @retval #BSL_SUCCESS 成功
     * @retval #BSL_SAL_ERR_UNKNOWN 操作失败
     * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
     */
    int32_t (*pfThreadReadLock)(BSL_SAL_ThreadLockHandle lock);

    /**
     * @ingroup bsl_sal
     * @brief 锁住写操作
     *
     * @par 描述：
     * 锁住写操作
     *
     * @param lock [IN] 锁句柄
     * @retval #BSL_SUCCESS 成功
     * @retval #BSL_SAL_ERR_UNKNOWN 操作失败
     * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
     */
    int32_t (*pfThreadWriteLock)(BSL_SAL_ThreadLockHandle lock);

    /**
     * @ingroup bsl_sal
     * @brief 解锁
     *
     * @par 描述：
     * 解锁
     *
     * @param lock [IN] 锁句柄
     * @retval #BSL_SUCCESS 成功
     * @retval #BSL_SAL_ERR_UNKNOWN 操作失败
     * @retval #BSL_SAL_ERR_BAD_PARAM 参数错误，lock为NULL
     */
    int32_t (*pfThreadUnlock)(BSL_SAL_ThreadLockHandle lock);

    /**
     * @ingroup bsl_sal
     * @brief 获取线程id
     *
     * @par 描述：
     * 获取线程id
     *
     * @return 线程id
     */
    uint64_t (*pfThreadGetId)(void);
} BSL_SAL_ThreadCallback;

/**
 * @ingroup bsl_sal
 * @brief   内存相关回调函数注册接口
 *
 * @par 描述：
 * 注册内存申请、释放函数
 *
 * @attention 无
 * @param cb [IN] 内存相关回调函数指针
 * @retval #BSL_SUCCESS 内存申请、释放函数注册成功
 * @retval #BSL_SAL_ERR_BAD_PARAM 0x02010003，cb为NULL或者cb的成员有NULL，请仔细填写cb指针
 */
int32_t BSL_SAL_RegMemCallback(BSL_SAL_MemCallback *cb);

/**
 * @ingroup bsl_sal
 * @brief   线程相关回调函数注册接口
 *
 * @par 描述：
 * 注册线程锁创建、释放、加锁、解锁、获取线程id相关函数。
 * 如果产品是单线程的，则不能调用；
 * 如果产品是多线程的，则必须调用。
 *
 * @attention 无
 * @param cb [IN] 线程相关回调函数指针
 * @retval #BSL_SUCCESS 线程相关函数注册成功
 * @retval #BSL_SAL_ERR_BAD_PARAM 0x02010003，cb为NULL或者cb的成员有NULL，请仔细填写cb指针
 */
int32_t BSL_SAL_RegThreadCallback(BSL_SAL_ThreadCallback *cb);

#ifdef __cplusplus
}
#endif

#endif