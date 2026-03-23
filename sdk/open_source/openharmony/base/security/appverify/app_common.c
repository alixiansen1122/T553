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

#include "app_common.h"
#include <stdio.h>
#include <stdlib.h>

#if (!defined(LITE_WEARABLE) && !defined(__Z_LITE__))
#include <stdarg.h>
#include "securec.h"
#endif

#if (!defined(LITE_WEARABLE) && !defined(__Z_LITE__))
static MemStastics g_memStats = {NULL, 0, 0, 0, 0};

static AppVMemStats *FindMemStsNode(const void *buf, AppVMemStats **pre)
{
    AppVMemStats *node = g_memStats.head;
    *pre = NULL;
    while (node != NULL) {
        if (node->buf == buf) {
            break;
        }
        *pre = (AppVMemStats *)node;
        node = node->next;
    }
    return node;
}
static int AddNode2MemSts(void *buf, int len, const char *filename, const int line)
{
    AppVMemStats *pre = NULL;
    AppVMemStats *node = FindMemStsNode(buf, &pre);
    if (node != NULL) {
        return V_ERR;
    }
    node = malloc(sizeof(AppVMemStats));
    if (node == NULL) {
        //LOG_PRINT_STR("malloc error");
        return V_ERR;
    }
    node->next = g_memStats.head;
    g_memStats.head = node;
    g_memStats.totalMallocCnt++;
    g_memStats.totalMem += len;
    node->buf = buf;
    node->len = len;
    node->state = 1;
    node->fileName = filename;
    node->line = line;
    int curUsed = g_memStats.totalMem - g_memStats.totalFree;
    if (curUsed > g_memStats.maxMemUsed) {
        g_memStats.maxMemUsed = curUsed;
    }
    return V_OK;
}

void *AppVMalloc(int size, const char *filename, const int line)
{
    if (size > MAX_MALLOC_SIZE || size < 0) {
        //LOG_PRINT_STR("malloc exceed max size : %d", size);
        return NULL;
    }
    void *buf = malloc(size);
    int ret = AddNode2MemSts(buf, size, filename, line);
    if (ret != V_OK) {
        //LOG_PRINT_STR("AppVMalloc error");
    }
    return buf;
}

void *AppVCalloc(int size, int num, const char *filename, const int line)
{
    if (size * num > MAX_MALLOC_SIZE || size * num < 0) {
        //LOG_PRINT_STR("malloc exceed max size : %d", size * num);
        return NULL;
    }
    void *buf = calloc(size, num);
    int ret = AddNode2MemSts(buf, size * num, filename, line);
    if (ret != V_OK) {
        //LOG_PRINT_STR("AppVMalloc error");
    }
    return buf;
}

void AppVFree(void *buf)
{
    AppVMemStats *pre = NULL;
    AppVMemStats *node = FindMemStsNode(buf, &pre);
    if (node == NULL) {
        //LOG_PRINT_STR("free error %p", buf);
        goto EXIT;
    }

    g_memStats.totalFreeCnt++;
    g_memStats.totalFree += node->len;
    if (pre == NULL) {
        g_memStats.head = node->next;
    } else {
        pre->next = node->next;
    }
    free(node);
EXIT:
    free(buf);
}
void AppVCheckMemStatics()
{
    //LOG_PRINT_STR("-------------------AppVCheckMemStatics-------------------");
    //LOG_PRINT_STR("total memory malloc     :%d", g_memStats.totalMem);
    //LOG_PRINT_STR("total memory malloc cnt :%d", g_memStats.totalMallocCnt);
    //LOG_PRINT_STR("total memory free       :%d", g_memStats.totalFree);
    //LOG_PRINT_STR("total memory free cnt   :%d", g_memStats.totalFreeCnt);
    //LOG_PRINT_STR("max memory needed       :%d", g_memStats.maxMemUsed);
    AppVMemStats *node = g_memStats.head;
    while (node != NULL) {
        //LOG_PRINT_STR("memory node: %p, len: %6d, %s:%d", node->buf, node->len, node->fileName, node->line);
        node = node->next;
    }
    //LOG_PRINT_STR("-------------------AppVCheckMemStatics-------------------");
    return;
}
#endif

#if (!defined(LITE_WEARABLE) && !defined(__Z_LITE__))
void PrintLog(const char *format, ...)
{
    char logbuf[LOG_BUF_SIZE + 1] = {0};
    int cnt;
    va_list pArgList;
    va_start(pArgList, format);
    cnt = vsnprintf_s(logbuf, LOG_BUF_SIZE, LOG_BUF_SIZE, format, pArgList);
    va_end(pArgList);
    if (cnt < 0) {
        return;
    }
    printf("%s", logbuf);
    return;
}

void *AppVMallocMbed(int size, const char *filename, const int line)
{
    return AppVMalloc(size, filename, line);
}

void *AppVCallocMbed(int size, int num, const char *filename, const int line)
{
    return AppVCalloc(size, num, filename, line);
}

void AppVFreeMbed(void *buf)
{
    return AppVFree(buf);
}
#endif

#if defined(LITE_WEARABLE)
void *AppVPsmemMalloc(uint32_t size)
{
    return PSRAM_Malloc(size);
}

void AppVPsmemFree(const void *buffer)
{
    PSRAM_Free(buffer);
}
#endif
