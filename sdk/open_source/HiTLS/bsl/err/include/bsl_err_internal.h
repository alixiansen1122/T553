/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * @file bsl_err_internal.h
 * Description: 错误码模块
 * Author: zhanghui
 * Create: 2023-05-30
 */

#ifndef BSL_ERR_INTERNAL_H
#define BSL_ERR_INTERNAL_H

#include <stdint.h>

/**
 * @ingroup BSL
 * @brief 将错误信息存入错误信息栈中
 *
 * @par 描述：
 * 将错误信息存入错误信息栈中
 *
 * @attention err不能为0
 * @param err [IN] 错误码，高16位表示模块子模块id，低16位表示错误号
 * @param file [IN] 文件名，不含目录路径
 * @param lineNo [IN] 错误产生的行号
 */
void BSL_ERR_PushError(int32_t err, const char *file, uint32_t lineNo);

/**
 * @ingroup BSL
 * @brief 将错误信息存入错误信息栈中
 *
 * @par 描述：
 * 将错误信息存入错误信息栈中
 *
 * @attention err不能为0
 */
#define BSL_ERR_PUSH_ERROR(e) BSL_ERR_PushError((e), __FILENAME__, __LINE__)

#endif // BSL_ERR_INTERNAL_H