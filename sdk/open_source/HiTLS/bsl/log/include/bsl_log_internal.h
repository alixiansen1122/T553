/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file bsl_log_internal.h
 * Description: 日志模块
 * Author: guzhou
 * Create: 2022-02-11
 */

#ifndef BSL_LOG_INTERNAL_H
#define BSL_LOG_INTERNAL_H

#include <stdint.h>
#include "bsl_log.h"

#ifdef __cplusplus
extern "C" {
#endif

void LOG_BinLogFixLen(uint32_t logId, uint32_t logLevel, uint32_t logType,
    void *format, void *para1, void *para2, void *para3, void *para4);

void LOG_BinLogVarLen(uint32_t logId, uint32_t logLevel, uint32_t logType, void *format, void *para);

/**
 * @ingroup BSL
 * @brief 打点日志，format中只能含有不超过4个参数，参数中不能有%s
 *
 * @attention format中只能含有不超过4个参数，参数中不能有%s。不够4个参数要被0或NULL，多于4个参数则要多次调用本函数。
 *            要打%s日志请使用LOG_BINLOG_FIXLEN。
 * @param logId [IN] 点号id，每个点号id只能用一次
 * @param level [IN] 日志级别
 * @param logType [IN] 日志类别，目前仅有BSL_LOG_BINLOG_TYPE_RUN，为后续预留扩展
 * @param format [IN] 仅含有1个%s的格式化字符串
 * @param para [IN] 字符串指针
 */
#define LOG_BINLOG_FIXLEN(logId, logLevel, logType, format, para1, para2, para3, para4) \
    LOG_BinLogFixLen(logId, logLevel, logType, \
        (void *)(uintptr_t)(const void *)(format), (void *)(uintptr_t)(para1), (void *)(uintptr_t)(para2), \
        (void *)(uintptr_t)(para3), (void *)(uintptr_t)(para4))

/**
 * @ingroup BSL
 * @brief 打点日志，format中含有且只能含有1个%s
 *
 * @attention format中含有且只能含有1个%s，多个字符串打点则要多次调用本函数。
 *            要打非%s的参数请使用LOG_BINLOG_FIXLEN。
 * @param logId [IN] 点号id，每个点号id只能用一次
 * @param level [IN] 日志级别
 * @param logType [IN] 日志类别，目前仅有BSL_LOG_BINLOG_TYPE_RUN，为后续预留扩展
 * @param format [IN] 仅含有1个%s的格式化字符串
 * @param para [IN] 字符串指针
 */
#define LOG_BINLOG_VARLEN(logId, logLevel, logType, format, para) \
    LOG_BinLogVarLen(logId, logLevel, logType, \
        (void *)(uintptr_t)(const void *)(format), (void *)(uintptr_t)(const void *)(para))

#ifdef __cplusplus
}
#endif

#endif // BSL_LOG_INTERNAL_H
