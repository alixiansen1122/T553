/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 线程锁接口的实现，由产品注册线程相关的函数
 * Author: guzhou
 * Create: 2022-02-11
 */

#include <stddef.h>
#include "bsl_log_internal.h"
#include "bsl_errno.h"
#include "bsl_binlog_id.h"
#include "sal_threadlock.h"

static BSL_SAL_ThreadCallback g_threadCallback = {0};

/* 线程功能可裁剪，若产品不注册线程相关函数，视为使用单线程，线程相关函数默认返回成功 */

int32_t SAL_ThreadLockNew(BSL_SAL_ThreadLockHandle *lock)
{
    if (g_threadCallback.pfThreadLockNew == NULL) {
        return BSL_SUCCESS;
    }
    if (lock == NULL) {
        return BSL_SAL_ERR_BAD_PARAM;
    }
    return g_threadCallback.pfThreadLockNew(lock);
}

int32_t SAL_ThreadReadLock(BSL_SAL_ThreadLockHandle lock)
{
    if (g_threadCallback.pfThreadReadLock == NULL) {
        return BSL_SUCCESS;
    }
    if (lock == NULL) {
        return BSL_SAL_ERR_BAD_PARAM;
    }
    return g_threadCallback.pfThreadReadLock(lock);
}

int32_t SAL_ThreadWriteLock(BSL_SAL_ThreadLockHandle lock)
{
    if (g_threadCallback.pfThreadWriteLock == NULL) {
        return BSL_SUCCESS;
    }
    if (lock == NULL) {
        return BSL_SAL_ERR_BAD_PARAM;
    }
    return g_threadCallback.pfThreadWriteLock(lock);
}

int32_t SAL_ThreadUnlock(BSL_SAL_ThreadLockHandle lock)
{
    if (g_threadCallback.pfThreadUnlock == NULL) {
        return BSL_SUCCESS;
    }
    if (lock == NULL) {
        return BSL_SAL_ERR_BAD_PARAM;
    }
    return g_threadCallback.pfThreadUnlock(lock);
}

void SAL_ThreadLockFree(BSL_SAL_ThreadLockHandle lock)
{
    if (g_threadCallback.pfThreadLockFree == NULL) {
        return;
    }
    if (lock == NULL) {
        return;
    }
    g_threadCallback.pfThreadLockFree(lock);
}

uint64_t SAL_ThreadGetId(void)
{
    if (g_threadCallback.pfThreadGetId == NULL) {
        return 0;
    }
    return g_threadCallback.pfThreadGetId();
}

int32_t BSL_SAL_RegThreadCallback(BSL_SAL_ThreadCallback *cb)
{
    if ((cb == NULL) || (cb->pfThreadLockNew == NULL) || (cb->pfThreadLockFree == NULL) ||
        (cb->pfThreadReadLock == NULL) || (cb->pfThreadWriteLock == NULL) ||
        (cb->pfThreadUnlock == NULL) || (cb->pfThreadGetId == NULL)) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05012, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "invalid params", NULL, NULL, NULL, NULL);
        return BSL_SAL_ERR_BAD_PARAM;
    }
    g_threadCallback.pfThreadLockNew = cb->pfThreadLockNew;
    g_threadCallback.pfThreadLockFree = cb->pfThreadLockFree;
    g_threadCallback.pfThreadReadLock = cb->pfThreadReadLock;
    g_threadCallback.pfThreadWriteLock = cb->pfThreadWriteLock;
    g_threadCallback.pfThreadUnlock = cb->pfThreadUnlock;
    g_threadCallback.pfThreadGetId = cb->pfThreadGetId;
    return BSL_SUCCESS;
}
