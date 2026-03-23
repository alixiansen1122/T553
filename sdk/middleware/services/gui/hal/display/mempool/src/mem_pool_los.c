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

#include "mem_pool_los.h"
#include "mem_pool_types.h"

#if defined(__LITEOS_M__) || defined(__FREERTOS__)
#ifdef __FREERTOS__
#include "soc_osal.h"
#else
#include "los_memory.h"
#include "los_memory_pri.h"
#endif
#include "display_common.h"
#include "securec.h"

typedef struct _MemPoolInfo {
    uint8_t *poolID;              /* memory pool ID */
    int32_t nodeCount;
    uint32_t maxNodeCount;        /* max count of allowed alloc */
    uint32_t minNodeSize;         /* min alloc size */
    struct _MemPoolInfo* next;    /* next memory pool */
} MemPoolInfo;

static MemPoolInfo* g_memPoolHead = NULL;
static MemPoolInfo* g_memPoolTail = NULL;

static MemPoolInfo* GetMemPoolInfo(const uint8_t *poolID)
{
    MemPoolInfo* curPool = g_memPoolHead;
    while (curPool != NULL) {
        if (curPool->poolID == poolID) {
            return curPool;
        }
        curPool = curPool->next;
    }
    return NULL;
}

void LosUnregisterMemPool(uint8_t *poolID)
{
    MemPoolInfo* pre = NULL;
    MemPoolInfo* cur = g_memPoolHead;

    while (cur != NULL) {
        if (cur->poolID == poolID) {
            if (pre == NULL) {
                g_memPoolHead = cur->next;
            } else {
                pre->next = cur->next;
            }
            free(cur);
            cur = NULL;
            return;
        }
        pre = cur;
        cur = cur->next;
    }
#ifdef __FREERTOS__
    (void)osal_pool_mem_deinit(poolID);
#endif
}

uint8_t* LosRegisterMemPool(uint8_t *pool, uint32_t poolSize, uint32_t maxItemCount, uint32_t minItemSize)
{
    if ((pool == NULL) || (poolSize == 0)) {
        DISP_LOGE("wrong parameter.\n");
        return MEMORY_POOL_ID_INVALID;
    }

    if (GetMemPoolInfo(pool) != NULL) {
        DISP_LOGW("pool is already existed\n");
        return pool;
    }

    MemPoolInfo* poolInfo = (MemPoolInfo*)malloc(sizeof(MemPoolInfo));
    if (poolInfo == NULL) {
        DISP_LOGE("malloc MemPoolInfo failed\n");
        return MEMORY_POOL_ID_INVALID;
    }

#ifdef __FREERTOS__
    if (osal_pool_mem_init(pool, poolSize) != OSAL_SUCCESS) {
        DISP_LOGE("osal_pool_mem_init failed\n");
        free(poolInfo);
        return MEMORY_POOL_ID_INVALID;
    }
#else
    if (LOS_MemInit(pool, poolSize) != LOS_OK) {
        DISP_LOGE("LOS_MemInit failed\n");
        free(poolInfo);
        return MEMORY_POOL_ID_INVALID;
    }
#endif
    poolInfo->nodeCount = 0;
    poolInfo->maxNodeCount = maxItemCount;
    poolInfo->minNodeSize = minItemSize;
    poolInfo->poolID = pool;
    poolInfo->next = NULL;
    if (g_memPoolHead == NULL) {
        g_memPoolHead = poolInfo;
    } else {
        g_memPoolTail->next = poolInfo;
    }
    g_memPoolTail = poolInfo;
    return pool;
}

uint8_t *LosAllocMem(uint8_t *poolID, uint32_t allocSize)
{
    MemPoolInfo* info = GetMemPoolInfo(poolID);
    CHECK_NULL_POINTER_RETURN_VALUE(info, NULL);
    if ((info->maxNodeCount != 0) && (info->nodeCount > info->maxNodeCount)) {
        DISP_LOGE("nodeCount is larger than max value: %u\n", info->maxNodeCount);
        return NULL;
    }

    if ((info->minNodeSize != 0) && (allocSize < info->minNodeSize)) {
        allocSize = info->minNodeSize;
    }
#ifdef __FREERTOS__
    uint8_t *addr = (uint8_t *)osal_pool_mem_alloc_align(poolID, allocSize, MEM_ALIGN);
#else
    uint8_t *addr = (uint8_t *)LOS_MemAllocAlign(poolID, allocSize, MEM_ALIGN);
#endif
    if (addr != NULL) {
        info->nodeCount++;
    }
    return addr;
}

void LosFreeMem(uint8_t *poolID, uint8_t *addr)
{
    MemPoolInfo* info = GetMemPoolInfo(poolID);
    CHECK_NULL_POINTER_RETURN(info);
#ifdef __FREERTOS__
    osal_pool_mem_free(poolID, addr);
#else
    LOS_MemFree(poolID, addr);
#endif
    info->nodeCount--;
    if (info->nodeCount < 0) {
        DISP_LOGE("nodeCount is less than 0\n");
    }
}
#endif
