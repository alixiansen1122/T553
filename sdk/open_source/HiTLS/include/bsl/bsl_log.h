/**
 * @defgroup    bsl_log bsl_log.h
 * @ingroup     bsl
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       由产品注册日志相关的函数
 */

#ifndef BSL_LOG_H
#define BSL_LOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup bsl_log
 *
 * 审计日志级别
 */
#define BSL_LOG_LEVEL_SEC         0U

/**
 * @ingroup bsl_log
 *
 * 紧急日志级别
 */
#define BSL_LOG_LEVEL_FATAL       1U

/**
 * @ingroup bsl_log
 *
 * 错误日志级别
 */
#define BSL_LOG_LEVEL_ERR         2U

/**
 * @ingroup bsl_log
 *
 * 警告日志级别
 */
#define BSL_LOG_LEVEL_WARN        3U

/**
 * @ingroup bsl_log
 *
 * 信息日志级别
 */
#define BSL_LOG_LEVEL_INFO        4U

/**
 * @ingroup bsl_log
 *
 * 调试日志级别
 */
#define BSL_LOG_LEVEL_DEBUG       5U

/**
 * @ingroup bsl_log
 *
 * HiTLS版本字符串
 */
#ifndef HITLS_VERSION
#define HITLS_VERSION ""
#endif

#define HITLS_VERSION_LEN 150

/**
 * @ingroup bsl_log
 * @brief   获取HiTLS版本号
 * @attention 接收版本字符串的空间长度versionLen必须大于等于HITLS_VERSION_LEN
 * @param   version [OUT] HiTLS当前版本号字符串
 * @param   versionLen [IN/OUT] HiTLS当前版本号字符串长度
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_LOG_ERR_MEMCPY 内存拷贝失败
 */
int32_t BSL_LOG_GetVersion(char *version, uint32_t *versionLen);

/**
 * @ingroup bsl_log
 * @brief   打点日志类型，可扩展其它类型
 */
#define BSL_LOG_BINLOG_TYPE_RUN 0x01

/**
 * @ingroup bsl_log
 * @brief   打点日志定长回调类型
 */
typedef void (*BSL_LOG_BinLogFixLenFunc)(uint32_t logId, uint32_t logLevel, uint32_t logType,
    void *format, void *para1, void *para2, void *para3, void *para4);

/**
 * @ingroup bsl_log
 * @brief   打点日志变长回调类型
 */
typedef void (*BSL_LOG_BinLogVarLenFunc)(uint32_t logId, uint32_t logLevel, uint32_t logType,
    void *format, void *para);

/**
 * @ingroup bsl_log
 * @brief   注册打点日志回调函数的参数类型
 */
typedef struct {
    BSL_LOG_BinLogFixLenFunc fixLenFunc; // 4参回调
    BSL_LOG_BinLogVarLenFunc varLenFunc; // 1参回调
} BSL_LOG_BinLogFuncs;

/**
 * @ingroup bsl_log
 * @brief   设置打点日志的定长和变长回调函数
 * @attention 入参可为NULL
 * @param   funcs [IN] 打点日志回调函数指针集合体，不可为NULL，其成员可为NULL
 * @retval  BSL_SUCCESS 成功
 */
int32_t BSL_LOG_RegBinLogFunc(const BSL_LOG_BinLogFuncs *funcs);

/**
 * @ingroup bsl_log
 * @brief   设置打点日志的日志级别
 * @attention 级别必须要合法
 * @param   level [IN] 打点日志的级别，合法值为BSL_LOG_LEVEL_SEC、BSL_LOG_LEVEL_FATAL、BSL_LOG_LEVEL_ERR、
 *          BSL_LOG_LEVEL_WARN、BSL_LOG_LEVEL_INFO、BSL_LOG_LEVEL_DEBUG
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_LOG_ERR_BAD_PARAM 入参非法
 */
int32_t BSL_LOG_SetBinLogLevel(uint32_t level);

/**
 * @ingroup bsl_log
 * @brief   获取打点日志的日志级别
 * @return  打点日志的级别，合法值为BSL_LOG_LEVEL_SEC、BSL_LOG_LEVEL_FATAL、BSL_LOG_LEVEL_ERR、
 *          BSL_LOG_LEVEL_WARN、BSL_LOG_LEVEL_INFO、BSL_LOG_LEVEL_DEBUG
 */
uint32_t BSL_LOG_GetBinLogLevel(void);

#ifdef __cplusplus
}
#endif

#endif
