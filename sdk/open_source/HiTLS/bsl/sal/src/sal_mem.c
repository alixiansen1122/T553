/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 内存分配、释放的实现，由产品注册内存分配、释放的函数
 * Author: guzhou
 * Create: 2022-02-11
 */

#include <stdlib.h>
#include "securec.h"
#include "bsl_log_internal.h"
#include "bsl_sal.h"
#include "bsl_errno.h"
#include "bsl_binlog_id.h"
#include "sal_mem.h"

static BSL_SAL_MemCallback g_memCallback = {0};

void *SAL_Malloc(uint32_t size)
{
    if (g_memCallback.pfMalloc == NULL) {
        return NULL;
    }
    return g_memCallback.pfMalloc(size);
}

void SAL_Free(void *value)
{
    if (g_memCallback.pfFree == NULL) {
        return;
    }
    g_memCallback.pfFree(value);
}

void *SAL_Calloc(uint32_t num, uint32_t size)
{
    if (num == 0 || size == 0) {
        return SAL_Malloc(0);
    }
    if (num > UINT32_MAX / size) { // 按《华为C语言编程规范V5.1》G.INT.02处理回绕
        return NULL;
    }
    uint32_t blockSize = num * size;
    uint8_t *ptr = SAL_Malloc(blockSize);
    if (ptr == NULL) {
        return NULL;
    }
    // 大于SECUREC_MEM_MAX_LEN的话要分段处理，因为memset_s仅能处理SECUREC_MEM_MAX_LEN这么大的
    uint32_t offset = 0;
    while (blockSize > SECUREC_MEM_MAX_LEN) {
        if (memset_s(&ptr[offset], SECUREC_MEM_MAX_LEN, 0, SECUREC_MEM_MAX_LEN) != EOK) {
            SAL_FREE(ptr);
            return NULL;
        }
        offset += SECUREC_MEM_MAX_LEN;
        blockSize -= SECUREC_MEM_MAX_LEN;
    }
    if (memset_s(&ptr[offset], blockSize, 0, blockSize) != EOK) {
        SAL_FREE(ptr);
        return NULL;
    }
    return ptr;
}

void *SAL_Dump(const void *src, uint32_t size)
{
    if (g_memCallback.pfMalloc == NULL) {
        return NULL;
    }

    void *ptr = g_memCallback.pfMalloc(size);
    if (ptr == NULL) {
        return NULL;
    }

    if (memcpy_s(ptr, size, src, size) != EOK) {
        SAL_FREE(ptr);
        return NULL;
    }

    return ptr;
}

int32_t BSL_SAL_RegMemCallback(BSL_SAL_MemCallback *cb)
{
    if ((cb == NULL) || (cb->pfMalloc == NULL) || (cb->pfFree == NULL)) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05011, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "invalid params", NULL, NULL, NULL, NULL);
        return BSL_SAL_ERR_BAD_PARAM;
    }
    g_memCallback.pfMalloc = cb->pfMalloc;
    g_memCallback.pfFree = cb->pfFree;
    return BSL_SUCCESS;
}

#if !defined(__clang__)
#pragma GCC push_options
#pragma GCC optimize("O3")
#endif
#define CLEAN_THRESHOLD_SIZE 16UL

static void CleanSensitiveDataLess16Byte(void *buf, uint32_t bufLen)
{
    uint8_t *tmp = (uint8_t *)buf;
    switch (bufLen) {
        case 16: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-16内存写0
        /* FALLTHRU */
        case 15: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-15内存写0
        /* FALLTHRU */
        case 14: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-14内存写0
        /* FALLTHRU */
        case 13: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-13内存写0
        /* FALLTHRU */
        case 12: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-12内存写0
        /* FALLTHRU */
        case 11: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-11内存写0
        /* FALLTHRU */
        case 10: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-10内存写0
        /* FALLTHRU */
        case 9: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-9内存写0
        /* FALLTHRU */
        case 8: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-8内存写0
        /* FALLTHRU */
        case 7: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-7内存写0
        /* FALLTHRU */
        case 6: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-6内存写0
        /* FALLTHRU */
        case 5: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-5内存写0
        /* FALLTHRU */
        case 4: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-4内存写0
        /* FALLTHRU */
        case 3: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-3内存写0
        /* FALLTHRU */
        case 2: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-2内存写0
        /* FALLTHRU */
        case 1: *(tmp++) = (uint8_t)0; // fall-through, 对第bufLen-1内存写0
        /* FALLTHRU */
        default:                   /* Do nothing */
            break;
    }
}

static void CleanSensitiveData(void *buf, uint32_t bufLen)
{
    uint8_t *tmp = (uint8_t *)buf;
    uint32_t boundOpt;

    if (((uintptr_t)buf & 0x3) == 0) { // buf & 0x3，用于判断是否是4字节对齐
        boundOpt = (bufLen >> 4) << 4; // 右移4bit再左移4bit，用于计算16的整数倍
        for (uint32_t i = 0; i < boundOpt; i += 16) { // 每次清零16片内存
            uint32_t *ctmp = (uint32_t *)(tmp + i);
            ctmp[0] = 0;
            ctmp[1] = 0; // 对i + 1内存进行清零
            ctmp[2] = 0; // 对i + 2内存进行清零
            ctmp[3] = 0; // 对i + 3内存进行清零
        }
    } else {
        boundOpt = (bufLen >> 2) << 2; // 右移2bit再左移2bit，用于计算4的整数倍
        for (uint32_t i = 0; i < boundOpt; i += 4) { // 每次清零4片内存
            tmp[i] = 0;
            tmp[i + 1] = 0; // 对i + 1内存进行清零
            tmp[i + 2] = 0; // 对i + 2内存进行清零
            tmp[i + 3] = 0; // 对i + 3内存进行清零
        }
    }
    for (uint32_t i = boundOpt; i < bufLen; ++i) {
        tmp[i] = 0;
    }
}

void SAL_CleanseData(void *ptr, uint32_t size)
{
    if (ptr == NULL) {
        return;
    }
    if (size > CLEAN_THRESHOLD_SIZE) {
        CleanSensitiveData(ptr, size);
    } else {
        CleanSensitiveDataLess16Byte(ptr, size);
    }
}

#if !defined(__clang__)
#pragma GCC pop_options
#endif
