/**
 * @defgroup    bsl_errno   bsl_errno.h
 * @ingroup     bsl
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       BSL模块的函数返回值
 */

#ifndef BSL_ERRNO_H
#define BSL_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup bsl_errno
 * @brief   返回成功
 */
#define BSL_SUCCESS 0

/**
 * @ingroup bsl_errno
 *
 * BSL模块的返回值从 0x03000000 ~ 0x03ffffff
 */
enum BSL_ERROR {
    /* 通用返回值从0x03000000开始 */
    BSL_NULL_INPUT = 0x03000000,
    BSL_INTERNAL_EXCEPTION,
    BSL_MEMCPY_FAIL,
    BSL_MEMMOVE_FAIL,
    BSL_ERROR_INPUT,

    /* SAL子模块的返回值从0x03010001开始 */
    BSL_SAL_ERR_UNKNOWN = 0x03010001, /**< 未知错误 */
    BSL_SAL_ERR_NO_MEMORY, /**< 申请内存失败 */
    BSL_SAL_ERR_BAD_PARAM, /**< 参数错误 */

    /* LOG子模块的返回值从0x03020001开始 */
    BSL_LOG_ERR_REG_FUNC_FAIL = 0x03020001, /**< 注册日志输出函数失败 */
    BSL_LOG_ERR_BAD_PARAM,                  /**< 参数错误 */
    BSL_LOG_ERR_MEMCPY_FAIL,                /**< 内存拷贝失败 */

    /* TLV子模块的返回值从0x03030001开始 */
    BSL_TLV_ERR_BAD_PARAM = 0x03030001,     /**< 参数错误 */
    BSL_TLV_ERR_MEMCPY_FAIL,                /**< 内存拷贝失败 */
    BSL_TLV_ERR_NO_WANT_TYPE,               /**< 没有找到TLV */

    /* ERR子模块的返回值从0x03040001开始 */
    BSL_ERR_ERR_ACQUIRE_READ_LOCK_FAIL = 0x03040001, /**< 获取读锁失败 */
    BSL_ERR_ERR_ACQUIRE_WRITE_LOCK_FAIL,
    BSL_ERR_ERR_NO_STACK,
    BSL_ERR_ERR_NO_ERROR,
    BSL_ERR_ERR_NO_MARK,
    
    // 0x03050001 预留给time

    /* UIO子模块的返回值从0x03060001开始 */
    BSL_UIO_FAIL = 0x03060001,
    BSL_UIO_IO_EXCEPTION,
    BSL_UIO_IO_BUSY,
    BSL_UIO_CTRL_INVALID_PARAM,
    BSL_UIO_FILE_OPEN_FAIL,
    BSL_UIO_MEM_GROW_FAIL,
    BSL_UIO_REF_MAX,
    BSL_UIO_MEM_ALLOC_FAIL,
};

#ifdef __cplusplus
}
#endif

#endif
