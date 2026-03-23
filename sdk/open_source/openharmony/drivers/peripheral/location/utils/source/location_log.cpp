/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "location_log.h"
#include "constant_definition.h"
#include <cstdio>
#include <cstring>
#include <climits>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

using namespace OHOS::Location;

#ifdef ENABLE_INFO_LEVEL_LOG
static LOCATION_LOG_LEVEL g_enabledLevel = LOCATION_LOG_INFO;
#else
static LOCATION_LOG_LEVEL g_enabledLevel = LOCATION_LOG_DEBUG;
#endif

static const uint32_t MAX_BUFFER_SZ = 256;

static void LogTime()
{
    struct timeval tv = {};
    gettimeofday(&tv, nullptr);
    struct tm tm = {};
    localtime_r(&tv.tv_sec, &tm);
    const int64_t usecToMsec = 1000;
    printf("[%02d:%02d:%02d:%03lld]", tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec / usecToMsec);
}

int32_t LocationLogPrintf(LOCATION_LOG_LEVEL level, const char *fmt, ...)
{
    if (level < g_enabledLevel) {
        return ERRCODE_SUCCESS;
    }
    if (fmt == nullptr) {
        return ERRCODE_INVALID_PARAMS;
    }
    const char *levelStr = nullptr;
    char out[MAX_BUFFER_SZ] = {0};
    switch (level) {
        case LOCATION_LOG_DEBUG:
            levelStr = "D";
            break;
        case LOCATION_LOG_INFO:
            levelStr = "I";
            break;
        case LOCATION_LOG_WARN:
            levelStr = "W";
            break;
        case LOCATION_LOG_ERR:
            levelStr = "E";
            break;
        case LOCATION_LOG_FATAL:
            levelStr = "F";
            break;
        default:
            levelStr = "E";
            break;
    }
    LogTime();
    va_list args;
    printf("[%s]", levelStr);
    va_start(args, fmt);
    int result = vsnprintf_s(out, MAX_BUFFER_SZ, MAX_BUFFER_SZ - 1, fmt, args);
    if (result == -1) {
        printf("vsnprintf_s failed!\n");
        va_end(args);
        return ERRCODE_INVALID_PARAMS;
    }
    va_end(args);
    printf("%s \n", out);
    return ERRCODE_SUCCESS;
}

int32_t SetLocationLogEnabledLevel(LOCATION_LOG_LEVEL level)
{
    if (level < LOCATION_LOG_DEBUG ||
        level > LOCATION_LOG_FATAL) {
        return ERRCODE_INVALID_PARAMS;
    }
    g_enabledLevel = level;
    return ERRCODE_SUCCESS;
}