/**
 * @defgroup    bsl_err bsl_err.h
 * @ingroup     bsl
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       由产品调用，初始化、反初始化错误码模块，获取错误码，使用之前必
 *              须要注册内存分配、释放相关函数，单线程环境可以不注册线程相关函数
 */

/** @defgroup    bsl  BSL */

#ifndef BSL_ERR_H
#define BSL_ERR_H

#include <stdint.h>
#include <stdbool.h>
#include "bsl_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup bsl_err
 * @brief 错误码模块初始化
 *
 * @par 描述：
 * 产品如果是单线程的，则该函数可调可不调；
 * 产品如果是多线程的，则必须在进程初始化时调用，且必须在调用BSL_SAL_RegThreadCallback
 * 之后调用。
 *
 * @attention 无
 * @retval #BSL_SUCCESS 错误码模块初始化成功
 * @retval #BSL_SAL_ERR_NO_MEMORY 内存空间不足，无法申请线程锁空间
 * @retval #BSL_SAL_ERR_UNKNOWN 线程锁初始化失败
 */
int32_t BSL_ERR_Init(void);

/**
 * @ingroup bsl_err
 * @brief 错误码模块反初始化
 *
 * @par 描述：
 * 产品在进程退出时调用
 *
 * @attention 无
 */
void BSL_ERR_DeInit(void);

/**
 * @ingroup bsl_err
 * @brief 删除错误栈
 *
 * @par 描述：
 * 删除错误栈，在进程和线程退出时调用
 *
 * @attention 线程退出时必须要调用，否则会有内存泄漏
 * @param isRemoveAll [IN] 是否删除全部错误栈，进程退出时用true，线程退出时用false
 */
void BSL_ERR_RemoveErrorStack(bool isRemoveAll);

/**
 * @ingroup bsl_err
 * @brief 获取错误栈中最后一次push的错误码
 *
 * @par 描述：
 * 产品在HiTLS某个接口发生错误后调用，获取错误码。可以一直调用，每次返回的错误码组成该接口发生错误
 * 的错误栈，直到返回BSL_SUCCESS为止。
 *
 * @attention
 * @return 错误码，高16位是发生错误的模块子模块id，低16位是原因号
 */
int32_t BSL_ERR_GetLastError(void);

/**
 * @ingroup bsl_err
 * @brief 获取错误栈中最后一次push的错误码、文件名和行号
 *
 * @par 描述：
 * 产品在HiTLS某个接口发生错误后调用，获取一个错误码，并且可以获取文件名和行号，这些已获取的信息会从错误栈中删除。
 * 可以一直调用，每次返回的错误码组成该接口发生错误的错误栈，直到返回BSL_SUCCESS为止。
 *
 * @attention 两个参数其中任一为NULL则无法获取文件名和行号
 * @param file [OUT] 用于获取发生错误的文件名，不含目录路径
 * @param lineNo [OUT] 用于获取发生错误的文件行号
 * @return 错误码，高16位是发生错误的模块子模块id，低16位是原因号
 */
int32_t BSL_ERR_GetLastErrorFileLine(const char **file, uint32_t *lineNo);

/**
 * @ingroup bsl_err
 * @brief 获取错误栈中最后一次push的错误码、文件名和行号
 *
 * @par 描述：
 * 产品在HiTLS某个接口发生错误后调用，获取一个错误码，并且可以获取文件名和行号，这些已获取的信息不会从错误栈中删除。
 *
 * @attention 两个参数其中任一为NULL则无法获取文件名和行号
 * @param file [OUT] 用于获取发生错误的文件名，不含目录路径
 * @param lineNo [OUT] 用于获取发生错误的文件行号
 * @return 错误码，高16位是发生错误的模块子模块id，低16位是原因号
 */
int32_t BSL_ERR_PeekLastErrorFileLine(const char **file, uint32_t *lineNo);

/**
 * @ingroup bsl_err
 * @brief 获取错误栈中最早一次push的错误码
 *
 * @par 描述：
 * 产品在HiTLS某个接口发生错误后调用，获取错误码。可以一直调用，每次返回的错误码组成该接口发生错误
 * 的错误栈，直到返回BSL_SUCCESS为止。
 *
 * @attention
 * @return 错误码，高16位是发生错误的模块子模块id，低16位是原因号
 */
int32_t BSL_ERR_GetError(void);

/**
 * @ingroup bsl_err
 * @brief 获取错误栈中最早一次push的错误码、文件名和行号
 *
 * @par 描述：
 * 产品在HiTLS某个接口发生错误后调用，获取一个错误码，并且可以获取文件名和行号，这些已获取的信息会从错误栈中删除。
 * 可以一直调用，每次返回的错误码组成该接口发生错误的错误栈，直到返回BSL_SUCCESS为止。
 *
 * @attention 两个参数其中任一为NULL则无法获取文件名和行号
 * @param file [OUT] 用于获取发生错误的文件名，不含目录路径
 * @param lineNo [OUT] 用于获取发生错误的文件行号
 * @return 错误码，高16位是发生错误的模块子模块id，低16位是原因号
 */
int32_t BSL_ERR_GetErrorFileLine(const char **file, uint32_t *lineNo);

/**
 * @ingroup bsl_err
 * @brief 获取错误栈中最早一次push的错误码、文件名和行号
 *
 * @par 描述：
 * 产品在HiTLS某个接口发生错误后调用，获取一个错误码，并且可以获取文件名和行号，这些已获取的信息不会从错误栈中删除。
 *
 * @attention 两个参数其中任一为NULL则无法获取文件名和行号
 * @param file [OUT] 用于获取发生错误的文件名，不含目录路径
 * @param lineNo [OUT] 用于获取发生错误的文件行号
 * @return 错误码，高16位是发生错误的模块子模块id，低16位是原因号
 */
int32_t BSL_ERR_PeekErrorFileLine(const char **file, uint32_t *lineNo);

/**
 * @ingroup bsl_err
 * @brief 清除错误栈
 *
 * @par 描述：
 * 在调用HiTLS接口后检测到错误，如果无需理会错误信息，那么在再次调用HiTLS接口之前应该调用本接口清理错误信息
 *
 * @attention 无
 */
void BSL_ERR_ClearError(void);

#ifdef __cplusplus
}
#endif

#endif
