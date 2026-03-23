/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#ifndef MEM_POOL_TYPES_H
#define MEM_POOL_TYPES_H
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CHECK_NULL_POINTER_RETURN_VALUE(pointer, ret) do { \
    if ((pointer) == NULL) { \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_POINTER_RETURN(pointer) do { \
    if ((pointer) == NULL) { \
        return; \
    } \
} while (0)

typedef struct _MemEntryTable {
    uint8_t *memAddr;
} MemEntryTable;

#define MEMORY_ID_INVALID 0xFFFF      /* invalid memory ID */
#define MEMORY_POOL_ID_INVALID NULL   /* invalid memory ID */
#define MIN_MEMORY_ID 1
#define MEM_ALIGN 16
#define MEM_NODE_USED_FLAG 0x80000000
#define MEM_NODE_USED 1
#define MEM_NODE_UNUSED 0
#define MEM_NODE_GET_SIZE(uwSizeAndFlag) ((uwSizeAndFlag) & (~MEM_NODE_USED_FLAG))

#define MEM_OK 0
#define MEM_NOK 1

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* MEMORY_POOL_COMMON_H */
