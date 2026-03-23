/*
 * Copyright (c) 2022 CompanyNameMagicTag Technologies Co., Ltd.
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

#ifndef OHOS_MEM_UTIL_H
#define OHOS_MEM_UTIL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#if !defined(_WIN32)
#include "los_memory.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void *PSRAM_Malloc(uint32_t size);
void *PSRAM_Calloc(uint32_t nitems, uint32_t size);
void PSRAM_Free(void *buffer);
void AceMemInfoPrint(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // OHOS_MEM_UTIL_H
