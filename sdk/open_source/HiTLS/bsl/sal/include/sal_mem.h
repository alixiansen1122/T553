/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file sal_mem.h
 * Description: 内存模块
 * Author: guzhou
 * Create: 2022-02-11
 */

/** @defgroup SAL SAL子模块接口 */

#ifndef SAL_MEM_H
#define SAL_MEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup SAL
 * @brief 分配内存空间
 *
 * @par 描述：
 * 分配内存空间
 *
 * @attention 无
 * @param size [IN] 分配内存的大小
 * @return 申请成功返回指向内存的指针，NULL为申请失败
 */
void *SAL_Malloc(uint32_t size);

/**
 * @ingroup SAL
 * @brief 分配内存空间并清零
 *
 * @par 描述：
 * 分配内存空间并清零，最大分配UINT32_MAX大小的内存空间
 *
 * @attention num*size不应出现溢出回绕
 * @param num [IN] 分配内存的数量
 * @param size [IN] 每块内存的大小
 * @return 申请成功返回指向内存的指针，NULL为申请失败或溢出回绕
 */
void *SAL_Calloc(uint32_t num, uint32_t size);

/**
 * @ingroup SAL
 * @brief   拷贝内存空间
 * @param   src 源内存地址
 * @param   size 内存空间总大小
 * @return  申请成功返回指向内存的指针，NULL为申请失败
 */
void *SAL_Dump(const void *src, uint32_t size);

/**
 * @ingroup SAL
 * @brief 释放指定的内存
 *
 * @par 描述：
 * 释放指定的内存
 *
 * @attention 无
 * @param value [IN] 待释放的内存空间指针
 */
void SAL_Free(void *value);

/**
 * @ingroup SAL
 * @brief 敏感信息置零
 *
 * @param ptr [IN] 要置零的内存
 * @param size [IN] 要置零的内存的长度
 *
 * @return 无
 */
void SAL_CleanseData(void *ptr, uint32_t size);

#define SAL_MALLOC(size_) SAL_Malloc((size_))

#define SAL_CALLOC(num_, size_) SAL_Calloc((num_), (size_))

#define SAL_DUMP(src_, size_) SAL_Dump((src_), (size_))

#define SAL_FREE(value_)                        \
    do {                                        \
        if ((value_) != NULL) {                 \
            SAL_Free((void *)(value_));         \
            (value_) = NULL;                    \
        }                                       \
    } while (false)

#ifdef __cplusplus
}
#endif

#endif
