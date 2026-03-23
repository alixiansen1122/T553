/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 日志模块的实现，调用由产品注册的日志输出函数
 * Author: guzhou
 * Create: 2022-02-11
 */

#include <stdbool.h>
#include "securec.h"
#include "bsl_errno.h"
#include "bsl_log_internal.h"

/* HiTLS版本字符串 */
static char g_hitlsVersion[HITLS_VERSION_LEN] = HITLS_VERSION;

int32_t BSL_LOG_GetVersion(char *version, uint32_t *versionLen)
{
    if (version == NULL || versionLen == NULL) {
        return BSL_LOG_ERR_BAD_PARAM;
    }

    if (*versionLen < HITLS_VERSION_LEN) {
        return BSL_LOG_ERR_BAD_PARAM;
    }

    uint32_t len = (uint32_t)strlen(g_hitlsVersion);
    if (memcpy_s(version, *versionLen, g_hitlsVersion, len) != EOK) {
        return BSL_LOG_ERR_MEMCPY_FAIL;
    }

    *versionLen = len;
    return BSL_SUCCESS;
}

static BSL_LOG_BinLogFixLenFunc g_fixLenFunc = NULL;
static BSL_LOG_BinLogVarLenFunc g_varLenFunc = NULL;
static uint32_t g_binlogLevel = BSL_LOG_LEVEL_ERR; // 默认开启error级
static uint32_t g_binlogType = BSL_LOG_BINLOG_TYPE_RUN; // 默认开启run类型，可扩展增加其它类型

int32_t BSL_LOG_RegBinLogFunc(const BSL_LOG_BinLogFuncs *funcs)
{
    bool invalid = funcs == NULL || funcs->fixLenFunc == NULL || funcs->varLenFunc == NULL;
    if (invalid) {
        return BSL_NULL_INPUT;
    }
    g_fixLenFunc = funcs->fixLenFunc;
    g_varLenFunc = funcs->varLenFunc;
    return BSL_SUCCESS;
}

int32_t BSL_LOG_SetBinLogLevel(uint32_t level)
{
    if (level > BSL_LOG_LEVEL_DEBUG) {
        return BSL_LOG_ERR_BAD_PARAM;
    }
    g_binlogLevel = level;
    return BSL_SUCCESS;
}

uint32_t BSL_LOG_GetBinLogLevel(void)
{
    return g_binlogLevel;
}

void LOG_BinLogFixLen(uint32_t logId, uint32_t logLevel, uint32_t logType,
    void *format, void *para1, void *para2, void *para3, void *para4)
{
    bool invalid = (logLevel > g_binlogLevel) || ((logType & g_binlogType) == 0) || (g_fixLenFunc == NULL);
    if (!invalid) {
        g_fixLenFunc(logId, logLevel, logType, format, para1, para2, para3, para4);
    }
}

void LOG_BinLogVarLen(uint32_t logId, uint32_t logLevel, uint32_t logType, void *format, void *para)
{
    bool invalid = (logLevel > g_binlogLevel) || ((logType & g_binlogType) == 0) || (g_varLenFunc == NULL);
    if (!invalid) {
        g_varLenFunc(logId, logLevel, logType, format, para);
    }
}
