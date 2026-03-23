/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: interrupt manager interfaces
* Author: Media Software Group
* Create: 2021-04-07
*/

#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#include <stdlib.h>
#include <stdio.h>
#include "policy_manager.h"
#include "audio_base_type.h"
#include "audio_interrupt_proxy.h"

typedef struct {
    AudioStreamType streamType;
    AudioSession sessionID;
    AudioStrategy strategy;
    /* 当前流是被打断或者delay 被压到后台的流 */
    bool isInterrupted;
    /* 当前流是是否已经恢复过 */
    bool isResumed;
    bool isExecutCallbackfunc;
    AudioStreamType interruptedStreamType;
    AudioStreamType delayedStreamType;
    int64_t interruptTime;
    uintptr_t owner;
    /* user data */
    uintptr_t userData;
    InterruptListenerCb interruptListenerCb;
    int64_t streamCreateTime;
    int64_t streamDestroyTime;
    AudioRoutedStreamDescriptor routedStreamDesc;
} InterruptElement;

/* Interrupt */
void InterruptMgrInit(void);
void InterruptMgrDeInit(void);
bool InterruptMgrCreateStream(AudioSession sessionID, const AudioStreamDescriptor *streamDesc, bool isOutput);
bool InterruptMgrDestroyStream(AudioSession sessionID, const AudioStreamDescriptor *streamDesc, bool isOutput);

bool InterruptMgrGetActiveStream(AudioStreamStrategy *streamStrategy, AudioStreamType streamType, bool isOutput);
/* interrupt might already be time out in the stack, handle it */
void InterruptMgrSignalExceptionCheckThread(void);
void InterruptMgrHandleException(void);
bool InterruptMgrCheckCanbeRouted(const AudioDeviceInfo *deviceInfo);
/* 获取当前焦点流的路由设备 */
AudioDeviceType InterruptMgrGetCurrRoutedDevice(const AudioDeviceInfo *deviceInfo);

int32_t InterruptMgrRemove(const InterruptElement *interruptEl);

int32_t InterruptMgrTryRequest(InterruptElement *interruptEl);

/* for Interrupt debug */
void InterruptMgrDump(void);
int32_t InterruptMgrDumpInfo(AudioInterruptDebugInfo *audioInterruptInfo);

#endif  // INTERRUPT_MANAGER_H