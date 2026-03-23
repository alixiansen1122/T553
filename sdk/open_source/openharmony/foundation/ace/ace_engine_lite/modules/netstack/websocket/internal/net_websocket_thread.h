/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef __NET_WS_THREAD_H__
#define __NET_WS_THREAD_H__

#include <stdio.h>
#include "cmsis_os2.h"

#define RWS_WAIT_FOREVER 0xffffffffu

/**
 @brief Callback type of thread function.
 @param userObject User object provided during thread creation.
 */
typedef void (*WsThreadFunct)(void *userObject);

struct WsThreadStruct {
    WsThreadFunct threadFunction;
    osThreadId_t thread;
    void *stackMem;
};

/**
 @brief Type of all public objects.
 */
typedef void *WsHandle;

/**
 @brief Mutex object handle.
 */
typedef WsHandle WsMutex;

/**
 @brief Mutex object handle.
 */
typedef WsHandle WsSem;

/**
 @brief Thread object handle.
 */
typedef struct WsThreadStruct *WsThread;

WsThread WsThreadCreate(WsThreadFunct threadFunction, void *userObject);

void WsThreadMsleep(const unsigned int millisec);

WsMutex WsMutexCreateRecursive(void);

void WsMutexLock(WsMutex mutex);

void WsMutexUnLock(WsMutex mutex);

void WsMutexDelete(WsMutex mutex);

WsSem WsSemCreate(void);

void WsSemDelete(WsSem sem);

void WsSemSignal(WsSem sem);

int WsThreadSuspend(WsThread workThread);

int WsThreadResume(WsThread workThread);

#endif
