/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio utils impl
* Author: Media Software Group
* Create: 2021-02-28
*/
#include <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "securec.h"
#include "audio_errors.h"
#include "audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#define MODULE_NAME "AudioUtils"

#define MAX_BUFFER_SZ 256

static AUDIO_LOG_LEVEL g_enabledLevel = AUDIO_LOG_LEVEL_ERR;

static char g_level2Ch[AUDIO_LOG_LEVEL_BUTT] = {
    [AUDIO_LOG_LEVEL_VERBOSE] = 'V',
    [AUDIO_LOG_LEVEL_DEBUG]   = 'D',
    [AUDIO_LOG_LEVEL_TRACE]   = 'T',
    [AUDIO_LOG_LEVEL_INFO]    = 'I',
    [AUDIO_LOG_LEVEL_WARN]    = 'W',
    [AUDIO_LOG_LEVEL_ERR]     = 'E',
    [AUDIO_LOG_LEVEL_FATAL]   = 'F',
};

static char GetLevelChar(AUDIO_LOG_LEVEL level)
{
    if (level < AUDIO_LOG_LEVEL_VERBOSE ||
        level > AUDIO_LOG_LEVEL_FATAL) {
        return 'F';
    }
    return g_level2Ch[level];
}

int32_t AudioLogPrintf(const char *modName, AUDIO_LOG_LEVEL level, const char *fmt, ...)
{
    if (level < g_enabledLevel) {
        return AUDIO_SUCCESS;
    }
    char buf[MAX_BUFFER_SZ + 1];
    char *p = buf;
    buf[MAX_BUFFER_SZ] = 0;
    if (modName != NULL) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm tm;
        localtime_r(&tv.tv_sec, &tm);
        const int64_t USEC_TO_MSEC = 1000;
        errno_t ret = snprintf_s(buf, sizeof(buf), MAX_BUFFER_SZ, "[%02d:%02d:%02d:%03lld] %c/%s :",
                                 tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec / USEC_TO_MSEC,
                                 GetLevelChar(level), modName);
        if (ret < 0) {
            printf("%c/%s AudioLogPrintf() line:%d vsnprintf_s failed!", GetLevelChar(level), modName, __LINE__);
            return AUDIO_ERROR;
        }
        p = buf + strlen(buf);
    }
    va_list var;
    va_start(var, fmt);
    errno_t ret = vsnprintf_s(p, MAX_BUFFER_SZ + 1 - (p - buf), MAX_BUFFER_SZ - (p - buf), fmt, var);
    if (ret < 0) {
        printf("%c/%s AudioLogPrintf() line:%d vsnprintf_s failed!", GetLevelChar(level), modName, __LINE__);
        va_end(var);
        return AUDIO_ERROR;
    }
    va_end(var);
    p = strchr(buf, '\0');
    if (p != NULL && (uintptr_t)p > (uintptr_t)buf && p[-1] != '\n') {
        p[0] = '\n';  // add a new line if it does not have
        p[1] = 0;
    }
    printf("%s", buf);
    return AUDIO_SUCCESS;
}

int32_t SetAudioLogEnabledLevel(AUDIO_LOG_LEVEL level)
{
    if (level < AUDIO_LOG_LEVEL_VERBOSE ||
        level > AUDIO_LOG_LEVEL_FATAL) {
        return AUDIO_INVALID_PARAMS;
    }
    g_enabledLevel = level;
    return AUDIO_SUCCESS;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
