/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 错误码栈的实现
 * Author: guzhou
 * Create: 2022-02-11
 */

#include <stdbool.h>
#include <inttypes.h>
#include "securec.h"
#include "bsl_log_internal.h"
#include "sal_mem.h"
#include "sal_threadlock.h"
#include "avl.h"
#include "bsl_errno.h"
#include "bsl_binlog_id.h"
#include "bsl_err_internal.h"
#include "bsl_err.h"

/* 错误信息栈大小 */
#define SAL_MAX_ERROR_STACK 20

/* 错误信息栈 */
typedef struct {
    /* 栈的当前指向位置，为-1时表示空栈 */
    uint16_t bottom; // 栈底
    uint16_t top; // 栈顶
    uint32_t flag; // 用于保护错误栈不被清理，目前用于异步场景中

    /* 存储特定线程的错误码信息 */
    int32_t errorStack[SAL_MAX_ERROR_STACK];

    /* 错误码的标记，用于部分清除、防侧信道 */
    uint32_t errorFlags[SAL_MAX_ERROR_STACK];

    /* 存储出错的文件名 */
    const char *filename[SAL_MAX_ERROR_STACK];

    /* 存储出错文件的行号 */
    uint32_t line[SAL_MAX_ERROR_STACK];
} ErrorCodeStack;

/* 错误栈的avl树根节点 */
static BSL_AvlTree *g_avlRoot = NULL;

/* 当前avl节点数量 */
static uint32_t g_avlNodeCount = 0;

/* avl树所允许的最大节点数 */
static uint32_t g_maxAvlNodes = 0x0000FFFF;

/* 线程锁的句柄 */
static BSL_SAL_ThreadLockHandle g_errLock;

static void StackReset(ErrorCodeStack *stack)
{
    if (stack != NULL) {
        (void)memset_s(stack, sizeof(*stack), 0, sizeof(*stack));
    }
}

static void StackResetIndex(ErrorCodeStack *stack, uint32_t i)
{
    bool invalid = stack == NULL || i >= SAL_MAX_ERROR_STACK;
    if (!invalid) {
        stack->errorStack[i] = 0;
        stack->line[i] = 0;
        stack->filename[i] = NULL;
        stack->errorFlags[i] = 0;
    }
}

static void StackDataFree(BSL_ElementData data)
{
    SAL_FREE(data);
}

static ErrorCodeStack *GetStack(void)
{
    const uint64_t threadId = SAL_ThreadGetId();
    BSL_AvlTree *curNode = BSL_AVL_SearchNode(g_avlRoot, threadId);
    if (curNode != NULL) {
        return curNode->data; // 已有错误栈直接返回
    }
    // 需要新建错误栈
    if (g_avlNodeCount >= g_maxAvlNodes) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05005, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "New Avl Node failed.", 0, 0, 0, 0);
        return NULL;
    }
    ErrorCodeStack *stack = (ErrorCodeStack *)SAL_CALLOC(1, sizeof(ErrorCodeStack));
    if (stack == NULL) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05004, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "CALLOC error code stack failed", 0, 0, 0, 0);
        return NULL;
    }
    BSL_AvlTree *node = BSL_AVL_MakeLeafNode(stack);
    if (node == NULL) {
        StackDataFree(stack);
        LOG_BINLOG_FIXLEN(BINLOG_ID05006, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "avl insert node failed, threadId %"PRIu64, threadId, 0, 0, 0);
        return NULL;
    }
    g_avlNodeCount++;
    g_avlRoot = BSL_AVL_InsertNode(g_avlRoot, threadId, node); /* 上层已经保证threadId节点不存在 */
    return stack;
}

void BSL_ERR_PushError(int32_t err, const char *file, uint32_t lineNo)
{
    if (err == BSL_SUCCESS) {
        return; // 不允许push success
    }

    const uint64_t threadId = SAL_ThreadGetId();

    int32_t ret = SAL_ThreadWriteLock(g_errLock);
    if (ret != BSL_SUCCESS) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05007, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "acquire lock failed when pushing error, threadId %"PRIu64", error code %d", threadId, ret, 0, 0);
        return;
    }

    ErrorCodeStack *stack = GetStack();
    if (stack != NULL) {
        if (stack->top == stack->bottom && stack->errorStack[stack->top] != 0) {
            stack->bottom = (stack->bottom + 1) % SAL_MAX_ERROR_STACK;
        }
        stack->errorFlags[stack->top] = 0;
        stack->errorStack[stack->top] = err;
        stack->filename[stack->top] = file;
        stack->line[stack->top] = lineNo;
        stack->top = (stack->top + 1) % SAL_MAX_ERROR_STACK;
    }

    SAL_ThreadUnlock(g_errLock);
}

void BSL_ERR_ClearError(void)
{
    uint64_t threadId = SAL_ThreadGetId();
    int32_t ret = SAL_ThreadWriteLock(g_errLock);
    if (ret != BSL_SUCCESS) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05008, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "acquire lock failed when clearing error, threadId %"PRIu64, threadId, 0, 0, 0);
        return;
    }

    BSL_AvlTree *curNode = BSL_AVL_SearchNode(g_avlRoot, threadId);
    if (curNode != NULL) {
        ErrorCodeStack *errStack = curNode->data;
        if (errStack->flag == 0) {
            StackReset(errStack);
        }
    }

    SAL_ThreadUnlock(g_errLock);
}

void BSL_ERR_RemoveErrorStack(bool isRemoveAll)
{
    int32_t ret = SAL_ThreadWriteLock(g_errLock);
    if (ret != BSL_SUCCESS) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05009, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "acquire lock failed when removing error stack, threadId %"PRIu64, SAL_ThreadGetId(), 0, 0, 0);
        return;
    }

    if (g_avlRoot != NULL) {
        if (isRemoveAll) {
            BSL_AVL_DeleteTree(g_avlRoot, StackDataFree);
            g_avlNodeCount = 0;
            g_avlRoot = NULL;
        } else {
            uint64_t threadId = SAL_ThreadGetId();
            BSL_AvlTree *curNode = BSL_AVL_SearchNode(g_avlRoot, threadId);
            if (curNode != NULL) {
                g_avlNodeCount--;
                g_avlRoot = BSL_AVL_DeleteNode(g_avlRoot, threadId, StackDataFree);
            }
        }
    }

    SAL_ThreadUnlock(g_errLock);
}

// 获取索引，last表示获取最后一个错误码或第一个错误码
uint16_t GetIndex(ErrorCodeStack *errStack, bool last)
{
    uint16_t idx;

    if (last) {
        idx = errStack->top - 1;
        if (idx >= SAL_MAX_ERROR_STACK) {
            idx = SAL_MAX_ERROR_STACK - 1;
        }
    } else  {
        idx = errStack->bottom;
    }

    return idx;
}

// clr为true表示外部操作是get，false表示外部操作是peek
// get操作会在获取错误信息后清理，而peek操作则不会
// last为true表示获取栈顶的最后一个错误码，false表示获取栈底的第一个错误码
static int32_t GetErrorInfo(const char **file, uint32_t *lineNo, bool clr, bool last)
{
    uint16_t idx;

    int32_t ret = SAL_ThreadReadLock(g_errLock);
    if (ret != BSL_SUCCESS) {
        return BSL_ERR_ERR_ACQUIRE_READ_LOCK_FAIL;
    }

    if (g_avlRoot == NULL) {
        /* 如果avlRoot为空，说明没有任何线程push error，所以error理应为success */
        SAL_ThreadUnlock(g_errLock);
        return BSL_SUCCESS;
    }

    const uint64_t threadId = SAL_ThreadGetId();
    BSL_AvlTree *curNode = BSL_AVL_SearchNode(g_avlRoot, threadId);
    if (curNode == NULL) {
        /* 如果curNode为空，说明当前线程没有push error，所以error理应为success */
        SAL_ThreadUnlock(g_errLock);
        return BSL_SUCCESS;
    }

    ErrorCodeStack *errStack = curNode->data; // 不会为NULL

    idx = GetIndex(errStack, last);
    if (errStack->errorStack[idx] == 0) { // 错误栈为空
        SAL_ThreadUnlock(g_errLock);
        return BSL_SUCCESS;
    }

    int32_t errorCode = errStack->errorStack[idx];   /* 获取指定的错误号 */
    uint32_t fileLine = errStack->line[idx]; /* 获取指定的行号 */
    const char *f = errStack->filename[idx]; // 获取指定的文件名
    if (clr) { // get or peek?
        StackResetIndex(errStack, idx);
        if (last) {
            errStack->top = idx;
        } else {
            errStack->bottom = (idx + 1) % SAL_MAX_ERROR_STACK;
        }
    }

    SAL_ThreadUnlock(g_errLock);

    if (file != NULL && lineNo != NULL) { // 这两一起，只获得其中一个没有意义
        if (f == NULL) {
            *file = "NA";
            *lineNo = 0;
        } else {
            *file = f;
            *lineNo = fileLine;
        }
    }

    return errorCode;
}

static int32_t GetLastErrorInfo(const char **file, uint32_t *lineNo, bool clr)
{
    return GetErrorInfo(file, lineNo, clr, true);
}

static int32_t GetFirstErrorInfo(const char **file, uint32_t *lineNo, bool clr)
{
    return GetErrorInfo(file, lineNo, clr, false);
}

int32_t BSL_ERR_GetLastErrorFileLine(const char **file, uint32_t *lineNo)
{
    return GetLastErrorInfo(file, lineNo, true);
}

int32_t BSL_ERR_PeekLastErrorFileLine(const char **file, uint32_t *lineNo)
{
    return GetLastErrorInfo(file, lineNo, false);
}

int32_t BSL_ERR_GetLastError(void)
{
    return GetLastErrorInfo(NULL, NULL, true);
}

int32_t BSL_ERR_GetErrorFileLine(const char **file, uint32_t *lineNo)
{
    return GetFirstErrorInfo(file, lineNo, true);
}

int32_t BSL_ERR_PeekErrorFileLine(const char **file, uint32_t *lineNo)
{
    return GetFirstErrorInfo(file, lineNo, false);
}

int32_t BSL_ERR_GetError(void)
{
    return GetFirstErrorInfo(NULL, NULL, true);
}

int32_t BSL_ERR_Init(void)
{
    if (g_errLock != NULL) {
        return BSL_SUCCESS;
    }
    int32_t ret = SAL_ThreadLockNew(&g_errLock);
    return ret;
}

void BSL_ERR_DeInit(void)
{
    if (g_errLock == NULL) {
        return;
    }
    SAL_ThreadLockFree(g_errLock);
    g_errLock = NULL;
}
