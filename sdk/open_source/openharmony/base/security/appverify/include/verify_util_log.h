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

#ifndef VERIFY_UTIL_LOG_H
#define VERIFY_UTIL_LOG_H

#include "verify_util_config.h"

#ifndef LINUX_X64_PC
#define LINUX_X64_PC 0
#endif

#ifndef NO_ARG
#define NO_ARG 0
#endif

#ifndef ONE_ARG
#define ONE_ARG 1
#endif

#ifndef TWO_ARGS
#define TWO_ARGS 2
#endif

#ifndef THREE_ARGS
#define THREE_ARGS 3
#endif

#ifndef FOUR_ARGS
#define FOUR_ARGS 4
#endif

#ifndef FIVE_ARGS
#define FIVE_ARGS 5
#endif

#ifndef SIX_ARGS
#define SIX_ARGS 6
#endif

#if (RUNNING_BOARD == LINUX_X64_PC)
#ifndef LOG_INFO
#define LOG_INFO(format, argNums, ...) printf("[INFO]:" format "\n", ##__VA_ARGS__)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(format, argNums, ...) printf("[DEBUG]:" format "\n", ##__VA_ARGS__)
#endif

#ifndef LOG_WARN
#define LOG_WARN(format, argNums, ...) printf("[WARN]:" format "\n", ##__VA_ARGS__)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(format, argNums, ...) printf("[ERROR]:" format "\n", ##__VA_ARGS__)
#endif

#elif (RUNNING_BOARD == LITE_WEARABLE)
#include "log.h"

#else

#ifndef LOG_INFO
#define LOG_INFO(format, argNums, ...) printf("[INFO]:" format "\n", ##__VA_ARGS__)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(format, argNums, ...) printf("[DEBUG]:" format "\n", ##__VA_ARGS__)
#endif

#ifndef LOG_WARN
#define LOG_WARN(format, argNums, ...) printf("[WARN]:" format "\n", ##__VA_ARGS__)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(format, argNums, ...) printf("[ERROR]:" format "\n", ##__VA_ARGS__)
#endif
#endif // if (RUNNING_BOARD == LINUX_X64_PC)

#endif // VERIFY_UTIL_LOG_H
