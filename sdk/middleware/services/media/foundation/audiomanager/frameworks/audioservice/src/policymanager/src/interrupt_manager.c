/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: interrupt manager impl
 * Author: Media Software Group
 * Create: 2021-04-07
 */


#include "interrupt_manager.h"
#include <sys/time.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <unistd.h>
#include "policy_config.h"
#include "devices_manager.h"
#include "stream_manager.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "am_list.h"
#include "securec.h"
#include "media_thread_adapt.h"

#define LOG_MODULE_NAME "InterruptMgr"

#define TIME_CONVERSION_US_TO_S 1000000LL /*  us to s */

#define DUCK_RATIO 0.5f /* 0.5 */

#define COMPARE_THRESHOLD 1e-6

/* Interrupt 到流的创建时间超时阈值 5000ms */
const uint32_t STREAM_CREATE_SPAN_US = 5000000;

/* 流的销毁时间 到 deactivate Interrupt 超时阈值 5000ms */
const uint32_t STREAM_DESTROY_SPAN_US = 5000000;

/* 异常检查线程在上面两个超时阈值基础上额外等的时间 100ms */
#define ADDITIONAL_TIME_TO_WAIT_US 100000

#define STREAM_NOT_CREATED 0
#define STREAM_NOT_DESTROYED 0

const float MAX_MIX_WEIGHT = 1.0;

typedef struct {
    List outputInterrupts;
    List inputInterrupts;
    MediaMutexHandle interruptLock;
    bool checkThreadStarted;
    MediaThreadIdHandle checkThread;
    MediaThreadCondHandle checkThreadCond;
    MediaMutexHandle condMutex;
} InterruptMgrContext;

static InterruptMgrContext g_interruptMgrCtx;

static AudioStreamType g_notSupportHandleException[] = {
    AUDIO_STREAM_VOICE_ASSISTANT,
};

static int64_t GetNowTimeUs(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    const int64_t int64Max = 0x7fffffffffffffff;
    /* 溢出检查，如果溢出，则返回-1 */
    if ((int64_t)tv.tv_sec > ((int64Max - tv.tv_usec) / TIME_CONVERSION_US_TO_S)) {
        return -1;
    }
    return (tv.tv_sec * TIME_CONVERSION_US_TO_S + tv.tv_usec);
}

static List *GetInterrupts(bool isOutput)
{
    if (isOutput) {
        return &g_interruptMgrCtx.outputInterrupts;
    } else {
        return &g_interruptMgrCtx.inputInterrupts;
    }
}

void InterruptMgrDump(void)
{
    /* for Interrupt debug */
    bool interruptMgrDebug = true;
    if (!interruptMgrDebug) {
        return;
    }
    ALOGD("in");
    List *list = &g_interruptMgrCtx.outputInterrupts;
    int32_t index = 0;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        ALOGD("Output Interrupt index:%d sType:0x%x sID:%u isInterrupted:%d ITime:%lld SCTime:%lld SDTime:%lld", index,
            interruptEl->streamType, interruptEl->sessionID, interruptEl->isInterrupted, interruptEl->interruptTime,
            interruptEl->streamCreateTime, interruptEl->streamDestroyTime);
        ALOGD("Output Interrupt index:%d", index);
        index++;
    }
    ALOGD("Interrupt Stack Output Dump END");
    ALOGD("Interrupt Stack Input Dump Begin");
    list = &g_interruptMgrCtx.inputInterrupts;
    index = 0;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        ALOGD("Input Interrupt index:%d sType:0x%x sID:%u isInterrupted:%d ITime:%lld SCTime:%lld SDTime:%lld", index,
            interruptEl->streamType, interruptEl->sessionID, interruptEl->isInterrupted, interruptEl->interruptTime,
            interruptEl->streamCreateTime, interruptEl->streamDestroyTime);
        ALOGD("Input Interrupt index:%d", index);
        index++;
    }
    ALOGD("out");
}

static VoidPtr InterruptChkThread(VoidPtr arg)
{
    AUDIO_UNUSED(arg);
    ALOGD("in");
#ifdef SUPPORT_SYS_PRCTL
    prctl(PR_SET_NAME, "InterruptChkThread", 0, 0, 0);
#endif
    while (g_interruptMgrCtx.checkThreadStarted) {
        MediaMutexLock(g_interruptMgrCtx.condMutex);
        /* 无超时等待，触发在ActivateAudioInterrupt 和 流销毁后 */
        uint32_t sleepMicroSeconds =
            STREAM_CREATE_SPAN_US >= STREAM_DESTROY_SPAN_US ? STREAM_CREATE_SPAN_US : STREAM_DESTROY_SPAN_US;
        MediaThreadCondTimeWait(g_interruptMgrCtx.checkThreadCond, g_interruptMgrCtx.condMutex,
            sleepMicroSeconds + ADDITIONAL_TIME_TO_WAIT_US);
        // 如果是DeInit的时候触发，就不需要再进行流的检查了，直接退出
        if (g_interruptMgrCtx.checkThreadStarted) {
            InterruptMgrHandleException();
        }
        MediaMutexUnLock(g_interruptMgrCtx.condMutex);
    }
    return NULL;
}

static bool InterruptElementIsEqual(const InterruptElement *interruptEl, const InterruptElement *newInterruptEl)
{
    if (interruptEl == NULL || newInterruptEl == NULL) {
        return false;
    }
    if (interruptEl->sessionID == newInterruptEl->sessionID && interruptEl->userData == newInterruptEl->userData) {
        return true;
    }
    return false;
}

static uintptr_t InterruptDup(uintptr_t ptr)
{
    if (ptr == 0) {
        return 0;
    }
    InterruptElement *interruptEl = (InterruptElement *)malloc(sizeof(InterruptElement));
    if (interruptEl == NULL) {
        ALOGE("InterruptElement malloc failed");
        return 0;
    }
    if (memcpy_s(interruptEl, sizeof(InterruptElement), (InterruptElement *)ptr, sizeof(InterruptElement)) != EOK) {
        ALOGE("memcpy_s  interruptEl failed");
        free(interruptEl);
        return 0;
    }
    return (uintptr_t)interruptEl;
}

static void InterruptFree(uintptr_t ptr)
{
    if (ptr == 0) {
        return;
    }
    InterruptElement *interruptEl = (InterruptElement *)ptr;
    free(interruptEl);
}

void InterruptMgrInit(void)
{
    g_interruptMgrCtx.interruptLock = MediaMutexCreate(NULL);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    DupFreeFuncPair dataFunc;
    dataFunc.dupFunc = InterruptDup;
    dataFunc.freeFunc = InterruptFree;
    ListInit(&g_interruptMgrCtx.outputInterrupts, &dataFunc);
    ListInit(&g_interruptMgrCtx.inputInterrupts, &dataFunc);
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);

    g_interruptMgrCtx.checkThreadCond = MediaThreadCondCreate();
    g_interruptMgrCtx.condMutex = MediaMutexCreate(NULL);
    AUDIO_LOCK(g_interruptMgrCtx.condMutex);
    g_interruptMgrCtx.checkThreadStarted = true;
    AUDIO_UNLOCK(g_interruptMgrCtx.condMutex);
    // stack size 0xA00: obtained from construct function deepest path scene, and upward adjust 512 bytes
    MediaThreadattr attr = { "InterruptChkThread", 0xA00u, 0, 0, false };
    g_interruptMgrCtx.checkThread = MediaThreadCreate(InterruptChkThread, NULL, &attr);
    CHK_NULL_RETURN_NONE(g_interruptMgrCtx.checkThread, "InterruptChkThread create failed");
    ALOGI("InterruptChkThread create success");
}

void InterruptMgrDeInit(void)
{
    AUDIO_LOCK(g_interruptMgrCtx.condMutex);
    g_interruptMgrCtx.checkThreadStarted = false;
    MediaThreadCondSignal(g_interruptMgrCtx.checkThreadCond);
    AUDIO_UNLOCK(g_interruptMgrCtx.condMutex);
    MediaThreadJoin(&g_interruptMgrCtx.checkThread);
    MediaThreadCondDestroy(&g_interruptMgrCtx.checkThreadCond);
    MediaMutexDestroy(&g_interruptMgrCtx.condMutex);

    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    ListDeinit(&g_interruptMgrCtx.inputInterrupts);
    ListDeinit(&g_interruptMgrCtx.outputInterrupts);
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    MediaMutexDestroy(&g_interruptMgrCtx.interruptLock);
}

static bool InterruptsIsEmpty(bool isOutput)
{
    bool ret = false;
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    ret = ListEmpty(GetInterrupts(isOutput));
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return ret;
}

void InterruptMgrSignalExceptionCheckThread(void)
{
    ALOGD("in");
    AUDIO_LOCK(g_interruptMgrCtx.condMutex);
    MediaThreadCondSignal(g_interruptMgrCtx.checkThreadCond);
    AUDIO_UNLOCK(g_interruptMgrCtx.condMutex);
    ALOGD("out");
}

bool InterruptMgrCheckCanbeRouted(const AudioDeviceInfo *deviceInfo)
{
    ALOGD("in");
    bool isOutput = (((uint32_t)deviceInfo->device & IN_FLAG) != 0) ? false : true;
    List *list = GetInterrupts(isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    if (ListEmpty(list)) {
        ALOGI(" List is Empty  ");
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return false;
    }

    bool found = false;
    StrategyConfig *config = NULL;
    InterruptElement *interruptEl = NULL;
    AudioStreamType streamType = AUDIO_STREAM_NONE;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        interruptEl = (InterruptElement *)ListIterData(it);
        streamType = interruptEl->streamType;
        ALOGI("streamType: 0x%x", streamType);
        config = InquireStrategyConfigForStream(streamType, isOutput);
        if (config == NULL) {
            ALOGI("strategy config is NULL");
            AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
            return false;
        }
        ALOGI("supportDeviceCount (%u) ", config->supportDeviceCount);
        for (uint32_t i = 0; i < config->supportDeviceCount; ++i) {
            ALOGI("support device index (%u) type (0x%x)", i, config->supportDevices[i]);
            if (config->supportDevices[i] == deviceInfo->device) {
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return found;
}

static AudioDeviceType FindNewRoutedDevice(StrategyConfig *config)
{
    for (uint32_t i = 0; i < config->supportDeviceCount; ++i) {
        if (AvlDevicesMgrIsAvailable(config->supportDevices[i])) {
            return config->supportDevices[i];
        }
    }
    return UNKNOWN;
}

AudioDeviceType InterruptMgrGetCurrRoutedDevice(const AudioDeviceInfo *deviceInfo)
{
    ALOGD("in");
    bool isOutput = (((uint32_t)deviceInfo->device & IN_FLAG) != 0) ? false : true;
    List *list = GetInterrupts(isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    if (ListEmpty(list)) {
        ALOGI(" List is Empty  ");
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return UNKNOWN;
    }

    // find corresponding InterruptElement for deviceInfo
    bool found = false;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        AudioStreamType streamType = interruptEl->streamType;
        ALOGI("streamType: 0x%x", streamType);
        StrategyConfig *config = InquireStrategyConfigForStream(streamType, isOutput);
        if (config == NULL) {
            ALOGI("strategy config is NULL");
            AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
            return UNKNOWN;
        }
        ALOGI("supportDeviceCount (%u) ", config->supportDeviceCount);
        for (uint32_t i = 0; i < config->supportDeviceCount; ++i) {
            ALOGI("support device index (%u) type (0x%x)", i, config->supportDevices[i]);
            if (config->supportDevices[i] != deviceInfo->device) {
                continue;
            }
            found = true;
            AudioDeviceType newDevice = FindNewRoutedDevice(config);
            if (newDevice != UNKNOWN) {
                AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
                return newDevice;
            }
            break;
        }
        if (found) {
            break;
        }
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return UNKNOWN;
}

static int32_t PushInterruptEntryAtFront(InterruptElement *interruptEl, bool isOutput)
{
    ALOGD("in");
    List *list = GetInterrupts(isOutput);
    if (interruptEl == NULL) {
        return AUDIO_ERROR;
    }
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    int64_t nowTimeUs = GetNowTimeUs();
    if (nowTimeUs == -1) {
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return AUDIO_ERROR;
    }
    interruptEl->interruptTime = nowTimeUs;
    interruptEl->strategy = STRATEGY_EXCLUSIVE;
    uint32_t ret = ListPushFront(list, (uintptr_t)interruptEl);
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    ALOGD("out");
    return ret == LIST_OK ? INTERRUPT_GRANTED : INTERRUPT_FAILED;
}

static int32_t PushAtBackInternal(InterruptElement *interruptEl, bool isOutput)
{
    /* Pre-conditions: interrupt stack is not empty and delay Interrupt */
    List *interrupts = GetInterrupts(isOutput);
    /* 先处理已经有后台的情况 , 如果有则回调stop 并删除 */
    InterruptElement *interruptBackEl = (InterruptElement *)ListBack(interrupts);
    /* !ListEmpty(list) 可以保证 interruptEl 不为 NULL */
    if (!ListEmpty(interrupts)) {
        /* 如果当前栈顶的流和即将来的流类型一致才需要进行弹出处理 */
        if (interruptBackEl->isInterrupted &&
            interruptBackEl->streamType == interruptEl->streamType) {
            interruptBackEl->interruptListenerCb(interruptBackEl->owner, interruptBackEl->userData, INTERRUPT_TYPE_END,
                INTERRUPT_HINT_STOP);
            ListPopBack(interrupts);
        }
    }
    /* 处理独占被移至后台的情况 */
    if (interruptEl->interruptTime == 0) {
        int64_t nowTimeUs = GetNowTimeUs();
        if (nowTimeUs == -1) {
            return AUDIO_ERROR;
        }
        interruptEl->interruptTime = nowTimeUs;
    }
    interruptEl->isInterrupted = true;
    interruptEl->isResumed = false;
    uint32_t ret = ListPushBack(interrupts, (uintptr_t)interruptEl);
    if (ret == LIST_OK && !interruptEl->isExecutCallbackfunc) {
        interruptEl->interruptListenerCb(interruptEl->owner, interruptEl->userData, INTERRUPT_TYPE_BEGIN,
            INTERRUPT_HINT_PAUSE);
        interruptEl->isExecutCallbackfunc = true;
    }
    InterruptMgrDump();
    return ret == LIST_OK ? AUDIO_SUCCESS : AUDIO_ERROR;
}

static int32_t PushInterruptEntryAtBack(const InterruptElement *interruptEl, bool isOutput)
{
    /* only for output delay */
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    int32_t ret = PushAtBackInternal((InterruptElement *)interruptEl, isOutput);
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("PushAtBackInternal failed streamType:0x%x sessionID:%u", interruptEl->streamType,
            interruptEl->sessionID);
        return INTERRUPT_FAILED;
    }
    ALOGD("out");
    return INTERRUPT_DELAYED;
}

static bool InterruptCheckIsReject(InterruptElement *newInterruptEl, bool isOutput, bool *isDelay)
{
    CHK_NULL_RETURN(newInterruptEl, false, "newInterruptEl is NULL");
    CHK_NULL_RETURN(isDelay, false, "isDelay is NULL");
    ALOGD("in");
    List *interrupts = GetInterrupts(isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    AudioStreamType incomingStream = newInterruptEl->streamType;
    for (ListIterator it = ListIterBegin(interrupts); it != ListIterEnd(interrupts);
        it = ListIterNext(interrupts, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        AudioStreamType focusStream = interruptEl->streamType;
        /* 从配置文件查询策略 */
        AudioStrategy strategy = STRATEGY_NONE;
        int32_t ret = InquireStrategyForStream(focusStream, incomingStream, isOutput, &strategy);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("InquireStrategyForStream failed streamType:0x%x sessionID:%u", interruptEl->streamType,
                interruptEl->sessionID);
            AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
            return true;
        }
        /* 是否可以入栈 */
        if (strategy == STRATEGY_REJECT) {
            newInterruptEl->strategy = STRATEGY_REJECT;
            AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
            return true;
        }
        if (strategy == STRATEGY_DELAY) {
            newInterruptEl->interruptedStreamType = interruptEl->streamType;
            newInterruptEl->delayedStreamType = focusStream;
            newInterruptEl->strategy = STRATEGY_DELAY;
            *isDelay = true;
            continue;
        }
        if (strategy == STRATEGY_EXCLUSIVE) {
            interruptEl->interruptedStreamType = newInterruptEl->streamType;
        }
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return false;
}

static int32_t HandleForExclusiveStrategy(const InterruptElement *interruptEl, bool isOutput)
{
    AudioStreamType focusStream = interruptEl->streamType;
    /* 如果是不可恢复,回调stop */
    if (!IsNeedResumeForStream(focusStream)) {
        interruptEl->interruptListenerCb(interruptEl->owner, interruptEl->userData, INTERRUPT_TYPE_BEGIN,
            INTERRUPT_HINT_STOP);
    } else {
        /* 如果是可恢复,需要压入 栈底 */
        InterruptElement interruptElTemp = *interruptEl;
        int32_t ret = PushAtBackInternal(&interruptElTemp, isOutput);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("PushAtBackInternal failed streamType:0x%x sessionID:%u", interruptEl->streamType,
                interruptEl->sessionID);
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t ProcessExclusiveInterrupt(const InterruptElement *newInterruptEl, bool isOutput)
{
    ALOGD("in");
    CHK_NULL_RETURN(newInterruptEl, false, "newInterruptEl is NULL");
    List *interrupts = GetInterrupts(isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    AudioStreamType incomingStream = newInterruptEl->streamType;
    for (ListIterator it = ListIterBegin(interrupts); it != ListIterEnd(interrupts);) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        AudioStreamType focusStream = interruptEl->streamType;
        /* 从配置文件查询策略 */
        AudioStrategy strategy = STRATEGY_NONE;
        int32_t ret = InquireStrategyForStream(focusStream, incomingStream, isOutput, &strategy);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("InquireStrategyForStream failed streamType:0x%x sessionID:%u", interruptEl->streamType,
                interruptEl->sessionID);
            AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
            return ret;
        }
        /* 如果是独占，根据是否可恢复处理 (不处理已经压到后台的情况，否则会死循环) */
        if (strategy == STRATEGY_EXCLUSIVE) {
            interruptEl->interruptedStreamType = newInterruptEl->streamType;
            if (interruptEl->isInterrupted) {
                it = ListIterNext(interrupts, it);
                continue;
            }
            ret = HandleForExclusiveStrategy(interruptEl, isOutput);
            if (ret != AUDIO_SUCCESS) {
                AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
                return ret;
            }
            /* 删除该 Interrupt */
            it = ListIterErase(interrupts, it);
        } else if (strategy == STRATEGY_NONE) { // 同类型的流 打断处理
            interruptEl->interruptListenerCb(interruptEl->owner, interruptEl->userData, INTERRUPT_TYPE_BEGIN,
                INTERRUPT_HINT_STOP);
            /* 删除该 Interrupt */
            it = ListIterErase(interrupts, it);
        } else {
            if (strategy == STRATEGY_MIX) {
                interruptEl->strategy = STRATEGY_MIX;
            }
            it = ListIterNext(interrupts, it);
        }
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    ALOGD("out");
    return AUDIO_SUCCESS;
}

bool InterruptMgrCreateStream(AudioSession sessionID, const AudioStreamDescriptor *streamDesc, bool isOutput)
{
    CHK_NULL_RETURN(streamDesc, false, "streamDesc is NULL");
    ALOGD("CreateStream sessionID:%u streamType(0x%x) isOutput:%d", sessionID, streamDesc->streamType, isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    List *list = GetInterrupts(isOutput);
    InterruptMgrDump();
    bool foundInterrupt = false;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        /* sessionID 和streamType 相等 才认为已经请求interrupt */
        ALOGD("CreateStream interruptEl->isInterrupted:%d", interruptEl->isInterrupted);
        ALOGD("CreateStream interruptEl->sessionID:%d", interruptEl->sessionID);
        ALOGD("CreateStream interruptEl->streamType:0x%x", interruptEl->streamType);
        if (interruptEl->sessionID == sessionID &&
            interruptEl->streamType == streamDesc->streamType) {
            /* 先把 streamDesc copy 到 streamDesc 中 */
            interruptEl->routedStreamDesc.streamDesc = *streamDesc;
            interruptEl->streamCreateTime = GetNowTimeUs();
            interruptEl->streamDestroyTime = STREAM_NOT_DESTROYED;
            foundInterrupt = true;
            break;
        }
    }
    if (!foundInterrupt) {
        ALOGE("sessionID:%u is invalid or no SUCCESS request Interrupt for stream:0x%x", sessionID,
            streamDesc->streamType);
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return false;
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return true;
}

bool InterruptMgrDestroyStream(AudioSession sessionID, const AudioStreamDescriptor *streamDesc, bool isOutput)
{
    bool ret = true;
    ALOGI("DestroyStream sessionID:%u streamType:0x%x", sessionID, streamDesc->streamType);
    List *list = GetInterrupts(isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    bool streamDestroyed = false;
    InterruptMgrDump();
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        /* sessionID 和streamType 才认为已经请求interrupt */
        if (interruptEl->sessionID == sessionID && interruptEl->streamType == streamDesc->streamType) {
            int64_t nowTimeUs = GetNowTimeUs();
            if (nowTimeUs == -1) {
                ret = false;
                goto DESTROY_RET;
            }
            /* 先把 streamDesc 置空 */
            if (memset_s(&interruptEl->routedStreamDesc.streamDesc, sizeof(AudioStreamDescriptor), 0x0,
                sizeof(AudioStreamDescriptor)) != EOK) {
                ALOGE("sessionID:%u memset_s failed for stream:0x%x", sessionID, streamDesc->streamType);
                ret = false;
                goto DESTROY_RET;
            }
            interruptEl->streamDestroyTime = nowTimeUs;
            streamDestroyed = true;
            break;
        }
    }
    /* streamMgr 已经校验 sessionID 和 流信息的合法性，即流是存在的，
    在 interruptMgr 管理链表中找不到，说明该流是不可恢复,已经被回调stop，失去焦点 */
    if (!streamDestroyed) {
        ALOGW("sessionID:%u stream:0x%x is not in the interruptMgr list", sessionID, streamDesc->streamType);
    }

DESTROY_RET:
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return ret;
}

static bool FindA2dpMusicStream(void)
{
    List *list = GetInterrupts(true);
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        if (interruptEl->streamType == AUDIO_STREAM_A2DP_MUSIC) {
            return true;
        }
    }
    return false;
}

static uint32_t CheckRoutedDevice(const AudioRoutedStreamDescriptor *routedStreamDesc, AudioDeviceType supportDevice,
    bool isOutput)
{
    ALOGD("CheckRoutedDevice streamType(0x%x) supportDevice(0x%x)", routedStreamDesc->streamDesc.streamType,
        supportDevice);
    ALOGD("CheckRoutedDevice audioLinkDir(%d) ", routedStreamDesc->streamDesc.audioLinkDir);
    bool isDownlinkDevice = false;
    bool isUplinkDevice = false;
    if (routedStreamDesc->streamDesc.streamType == AUDIO_STREAM_VOICE_CALL_BT_SCO) {
        if (routedStreamDesc->streamDesc.audioLinkDir == AUDIO_DOWN_LINK) {
            isDownlinkDevice = (isOutput && supportDevice == OUT_BUILTIN_SPEAKER) ||
                (!isOutput && supportDevice == IN_BLUETOOTH_SCO);
            return isDownlinkDevice ? supportDevice : UNKNOWN;
        } else {
            isUplinkDevice = (isOutput && supportDevice == OUT_BLUETOOTH_SCO) ||
                (!isOutput && supportDevice == IN_BUILTIN_MIC);
            return isUplinkDevice ? supportDevice : UNKNOWN;
        }
    } else if (routedStreamDesc->streamDesc.streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        if (routedStreamDesc->streamDesc.audioLinkDir == AUDIO_DOWN_LINK) {
            isDownlinkDevice = (!isOutput && (supportDevice == IN_MODEM || supportDevice == IN_MODEM_HEADSET)) ||
                (isOutput && (supportDevice == OUT_BLUETOOTH_SCO || supportDevice == OUT_BUILTIN_SPEAKER));
            return isDownlinkDevice ? supportDevice : UNKNOWN;
        } else {
            isUplinkDevice = (isOutput && (supportDevice == OUT_MODEM || supportDevice == OUT_MODEM_HEADSET)) ||
                (!isOutput && (supportDevice == IN_BLUETOOTH_SCO || supportDevice == IN_BUILTIN_MIC));
            return isUplinkDevice ? supportDevice : UNKNOWN;
        }
    } else {
        // 如果是蓝牙设备，就需要判断一下当前是否有A2DP sink链路，如果有则不能路由到蓝牙设备，只能是本地设备
        if (FindA2dpMusicStream()) {
            if (supportDevice == OUT_BLUETOOTH_A2DP) {
                return UNKNOWN;
            }
        }
        return supportDevice;
    }
}

static int32_t GetRoutedDeviceForStream(AudioRoutedStreamDescriptor *routedStreamDesc, bool isOutput)
{
    ALOGD("streamType (0x%x) isOutput:%d", routedStreamDesc->streamDesc.streamType, isOutput);
    AudioStreamType streamType = routedStreamDesc->streamDesc.streamType;
    StrategyConfig *config = InquireStrategyConfigForStream(streamType, isOutput);
    /* streamType is invalid */
    CHK_NULL_RETURN(config, AUDIO_INVALID_PARAMS, "config is NULL");
    ALOGD("supportDeviceCount (%u) ", config->supportDeviceCount);
    /* 每次获取时需要 reset */
    routedStreamDesc->currentRoutedDeviceNum = 0;
    routedStreamDesc->currentRoutedDevices = 0;
    for (uint32_t i = 0; i < config->supportDeviceCount; ++i) {
        ALOGD("supportDevice index (%u) ", i);
        ALOGD("supportDevice config->supportDevices[i] (0x%x) ", config->supportDevices[i]);
        if (AvlDevicesMgrIsAvailable(config->supportDevices[i]) &&
            CheckRoutedDevice(routedStreamDesc, config->supportDevices[i], isOutput) != UNKNOWN) {
            routedStreamDesc->currentRoutedDevices |= (uint32_t)config->supportDevices[i];
            ++routedStreamDesc->currentRoutedDeviceNum;
            if (!config->routeAllDevices) {
                return AUDIO_SUCCESS;
            }
        }
    }
    if (routedStreamDesc->currentRoutedDeviceNum == 0) {
        ALOGE("Get Routed Device For Stream (0x%x) failed", streamType);
        return AUDIO_ERROR;
    }
    ALOGD("streamType (0x%x) SUCCESS", streamType);
    return AUDIO_SUCCESS;
}

static void GetMixWeight(InterruptElement *interruptEl, float materStreamTypeVolume, bool isOutput)
{
    // 如果是来电铃声则不进行声音的削弱，保证和通话都同时混音存在
    if (interruptEl->routedStreamDesc.streamDesc.streamType == AUDIO_STREAM_RING) {
        interruptEl->routedStreamDesc.mixWeight = 1;
        return;
    }
    if (!isOutput) {
        ALOGI("audio stream in not support get mix weight!");
        return;
    }
    float curStreamTypeVolume = interruptEl->routedStreamDesc.streamDesc.streamTypeVolume;
    if (!StreamMgrGetTypeVolume(interruptEl->streamType, &curStreamTypeVolume)) {
        ALOGI("get nv volume failed!, streamType:%x", interruptEl->streamType);
    }
    // 当即将来的流的音量 >= 当前流音量的2倍时，不需要调整当前流的混音音量，已经足够能区分两条流的音量
    float halfMaterStreamTypeVolume = (materStreamTypeVolume * DUCK_RATIO);
    if (halfMaterStreamTypeVolume >= curStreamTypeVolume) {
        return;
    }
    if (curStreamTypeVolume < COMPARE_THRESHOLD || materStreamTypeVolume < COMPARE_THRESHOLD) {
        interruptEl->routedStreamDesc.mixWeight = DUCK_RATIO;
    } else {
        interruptEl->routedStreamDesc.mixWeight = (halfMaterStreamTypeVolume / curStreamTypeVolume);
    }
}

bool InterruptMgrGetActiveStream(AudioStreamStrategy *streamStrategy, AudioStreamType streamType, bool isOutput)
{
    /* 获取流和对应的混音权重 */
    ALOGD("isOutput:%d", isOutput);
    int32_t index = 0;
    float materStreamTypeVolume = 0.0;
    int32_t ret;
    List *list = GetInterrupts(isOutput);
    CHK_NULL_RETURN(streamStrategy, false, "streamStrategy is NULL");
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        /* 可以恢复的流即使被打断，也需要默认是active的 */
        bool isValidActiveStream = (!interruptEl->isInterrupted || IsNeedResumeForStream(interruptEl->streamType)) &&
            interruptEl->streamType == interruptEl->routedStreamDesc.streamDesc.streamType;
        if (isValidActiveStream) {
            ret = GetRoutedDeviceForStream(&interruptEl->routedStreamDesc, isOutput);
            if (ret != AUDIO_SUCCESS) {
                if (interruptEl->streamType == streamType) {
                    ALOGE("Get Routed Device For Stream (0x%x) failed", interruptEl->streamType);
                    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
                    return false;
                }
                continue;
            }
            /* 第一条流作为混音权重 100% */
            if (index == 0) {
                materStreamTypeVolume = interruptEl->routedStreamDesc.streamDesc.streamTypeVolume;
                interruptEl->routedStreamDesc.mixWeight = MAX_MIX_WEIGHT;
                streamStrategy->currentStrategy = STRATEGY_EXCLUSIVE;
            } else {
                GetMixWeight(interruptEl, materStreamTypeVolume, isOutput);
                streamStrategy->currentStrategy = STRATEGY_MIX;
            }
            streamStrategy->activeStreams[index] = &interruptEl->routedStreamDesc;
            streamStrategy->activeStreamNum++;
            if (++index >= MAX_STREAM_NUM) {
                break;
            }
        }
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return true;
}

static void DeleteDelayStreamFromList(const InterruptElement *interruptEl, List *list)
{
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list);) {
        InterruptElement *curInterruptEl = (InterruptElement *)ListIterData(it);
        // 判断当前的流类型是否已经创建了流，如果已经创建了流则不进行焦点的删除，没有则需要进行焦点的删除
        if (curInterruptEl->streamType ==  interruptEl->streamType &&
            !StreamMgrIsCreateStreamByType(curInterruptEl->streamType)) {
            it = ListIterErase(list, it);
            break;
        } else {
            it = ListIterNext(list, it);
        }
    }
}

static bool InterruptCheckIsCanResume(const InterruptElement *interruptMain,
    InterruptElement *newInterruptEl, List *list, bool isOutput)
{
    // 只有一个焦点不需要再进行策略获取
    if (ListSize(list) <= 1) {
        return true;
    }
    bool isCanResume = true;
    AudioStreamType incomingStream = newInterruptEl->streamType;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list);
        it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        AudioStreamType focusStream = interruptEl->streamType;
        // 如果是已经要删除的流，或者是自己本身不需要再进行策略获取和更新
        if (interruptEl->streamType == interruptMain->streamType ||
            interruptEl->streamType == newInterruptEl->streamType) {
            continue;
        }
        /* 从配置文件查询策略 */
        AudioStrategy strategy = STRATEGY_NONE;
        int32_t ret = InquireStrategyForStream(focusStream, incomingStream, isOutput, &strategy);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("InterruptCheckIsCanResume failed streamType:0x%x sessionID:%u", interruptEl->streamType,
                interruptEl->sessionID);
        }
        /* 根据策略更新被打断的流 */
        if (strategy == STRATEGY_REJECT) {
            newInterruptEl->interruptedStreamType = interruptEl->streamType;
            isCanResume = false;
        } else if (strategy == STRATEGY_DELAY) {
            newInterruptEl->interruptedStreamType = interruptEl->streamType;
            newInterruptEl->delayedStreamType = focusStream;
            isCanResume = false;
        } else if (strategy == STRATEGY_EXCLUSIVE) {
            interruptEl->interruptedStreamType = newInterruptEl->streamType;
        }
    }
    return isCanResume;
}

static void ResumeInterruptStream(const InterruptElement *interruptEl, List *list, bool isOutPut)
{
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *curInterruptEl = (InterruptElement *)ListIterData(it);
        if (curInterruptEl != NULL && curInterruptEl->isInterrupted &&
            curInterruptEl->interruptedStreamType == interruptEl->streamType) {
            // 检查当前的需要恢复的流是否是可以恢复的，需要跟现在已经存在的流重新做策略
            if (!InterruptCheckIsCanResume(interruptEl, curInterruptEl, list, isOutPut)) {
                continue;
            }
            curInterruptEl->isInterrupted = false;
            curInterruptEl->interruptedStreamType = AUDIO_STREAM_INVALID;
            curInterruptEl->interruptListenerCb(curInterruptEl->owner, curInterruptEl->userData, INTERRUPT_TYPE_END,
                INTERRUPT_HINT_RESUME);
            curInterruptEl->isResumed = true;
            // 延时流的焦点在恢复以后需要删除，因为此焦点已经过时，策略可能已经更新，恢复后会再主动进行焦点的获取
            if (curInterruptEl->delayedStreamType == interruptEl->streamType) {
                DeleteDelayStreamFromList(curInterruptEl, GetInterrupts(true));
                DeleteDelayStreamFromList(curInterruptEl, GetInterrupts(false));
                curInterruptEl->delayedStreamType = AUDIO_STREAM_INVALID;
            }
        }
    }
}

static int32_t RemoveInterruptInternal(const InterruptElement *interruptEl, bool isOutput)
{
    /* Pre-conditions: 删除元素 */
    List *list = GetInterrupts(isOutput);
    ALOGI("Remove sessionID:%u streamType:(0x%x)", interruptEl->sessionID, interruptEl->streamType);
    InterruptMgrDump();
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    bool beRemved = false;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list);) {
        InterruptElement *curInterruptEl = (InterruptElement *)ListIterData(it);
        if (InterruptElementIsEqual(curInterruptEl, interruptEl)) {
            it = ListIterErase(list, it);
            beRemved = true;
            break;
        }
        it = ListIterNext(list, it);
    }
    InterruptMgrDump();
    if (!beRemved) {
        ALOGE("Remove sessionID:%u streamType:(0x%x)not found", interruptEl->sessionID, interruptEl->streamType);
        ResumeInterruptStream(interruptEl, list, isOutput);
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return AUDIO_INVALID_OPERATION;
    }
    /* 栈顶是后台则 回调 resume  重新获得打断 */
    ResumeInterruptStream(interruptEl, list, isOutput);
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return AUDIO_SUCCESS;
}

static StrategyConfig *GetStrategyConfig(AudioStreamType streamType)
{
    StrategyConfig *config = NULL;
    config = InquireStrategyConfigForStream(streamType, true);
    if (config == NULL) {
        config = InquireStrategyConfigForStream(streamType, false);
    }
    return config;
}


int32_t InterruptMgrRemove(const InterruptElement *interruptEl)
{
    int32_t ret;
    int32_t retInput;
    int32_t retOutput;
    CHK_NULL_RETURN(interruptEl, AUDIO_ERROR, "interruptEl is NULL");
    StrategyConfig *config = GetStrategyConfig(interruptEl->streamType);
    if (config == NULL) {
        ALOGE("cannot inquire strategy config for stream:(0x%x)", interruptEl->streamType);
        return AUDIO_ERROR;
    }
    switch (config->streamDir) {
        case AUDIO_STREAM_OUT:
            ret = RemoveInterruptInternal(interruptEl, true);
            break;
        case AUDIO_STREAM_IN:
            ret = RemoveInterruptInternal(interruptEl, false);
            break;
        case AUDIO_STREAM_OUT_IN:
            retOutput = RemoveInterruptInternal(interruptEl, true);
            retInput = RemoveInterruptInternal(interruptEl, false);
            if (retInput == AUDIO_SUCCESS && retOutput == AUDIO_SUCCESS) {
                ret = AUDIO_SUCCESS;
            } else {
                ret = AUDIO_ERROR;
            }
            break;
        default:
            ALOGI("invalid stream:(0x%x)", interruptEl->streamType);
            ret = AUDIO_INVALID_OPERATION;
            break;
    }
    return ret;
}
static bool CheckStreamTypeSupportHandleException(AudioStreamType streamType)
{
    for (uint32_t i = 0; i < sizeof(g_notSupportHandleException) /
        sizeof(g_notSupportHandleException[0]); i++) {
        if (g_notSupportHandleException[i] == streamType) {
            return false;
        }
    }
    return true;
}

static void InterruptMgrHandleExceptionInner(List *list, bool isOutPut)
{
    int64_t nowTimeUs = GetNowTimeUs();
    if (nowTimeUs == -1) {
        return;
    }
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list);) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        /* 后台或者delay的焦点无需进行异常检查 */
        if (interruptEl->isInterrupted || interruptEl->isResumed) {
            it = ListIterNext(list, it);
            continue;
        }
        /* Interrupt 到流的创建时间超时 */
        int64_t interruptTimeUntilNow = nowTimeUs - interruptEl->interruptTime;
        /* 流的销毁到 deactivate Interrupt 超时 */
        int64_t streamDestroyTimeUntilNow = nowTimeUs - interruptEl->streamDestroyTime;
        bool interruptToCreateTimeOutException =
            interruptEl->streamCreateTime == STREAM_NOT_CREATED && interruptTimeUntilNow >= STREAM_CREATE_SPAN_US;
        bool destroyToDeactiveTimeOutException = interruptEl->streamDestroyTime != STREAM_NOT_DESTROYED &&
            streamDestroyTimeUntilNow >= STREAM_DESTROY_SPAN_US;
        ALOGI("interruptTimeUntilNow:%lld streamDestroyTimeUntilNow:%lld", interruptTimeUntilNow,
            streamDestroyTimeUntilNow);
        ALOGI("interruptToCreateTimeOutException:%d destroyToDeactiveTimeOutException:%d sessionID:%u streamType:0x%x",
            interruptToCreateTimeOutException, destroyToDeactiveTimeOutException, interruptEl->sessionID,
            interruptEl->streamType);
        if ((interruptToCreateTimeOutException || destroyToDeactiveTimeOutException) &&
            CheckStreamTypeSupportHandleException(interruptEl->streamType)) {
            /* 栈顶 是 后台流，说明是被打断的应用 重新获取 interrupt，需要回调打断结束的信息 */
            ResumeInterruptStream(interruptEl, list, isOutPut);
            it = ListIterErase(list, it);
        } else {
            it = ListIterNext(list, it);
        }
    }
}

void InterruptMgrHandleException(void)
{
    ALOGD("in");
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    InterruptMgrDump();
    InterruptMgrHandleExceptionInner(&g_interruptMgrCtx.outputInterrupts, true);
    InterruptMgrHandleExceptionInner(&g_interruptMgrCtx.inputInterrupts, false);
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    ALOGD("out");
}

/* 相同的Interrupt 替换即可 */
static bool TryReplaceInterrupt(const InterruptElement *newInterruptEl, bool isOutput)
{
    ALOGD("in");
    List *interrupts = GetInterrupts(isOutput);
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    for (ListIterator it = ListIterBegin(interrupts); it != ListIterEnd(interrupts);
        it = ListIterNext(interrupts, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        if (interruptEl->isInterrupted) {
            continue;
        }
        if (InterruptElementIsEqual(interruptEl, newInterruptEl)) {
            int64_t nowTimeUs = GetNowTimeUs();
            if (nowTimeUs == -1) {
                AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
                return false;
            }
            interruptEl->interruptTime = nowTimeUs;
            interruptEl->streamType = newInterruptEl->streamType;
            interruptEl->streamCreateTime = 0;
            interruptEl->streamDestroyTime = 0;
            AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
            return true;
        }
    }
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return false;
}

/*
当前激活打断的 AudioStreamType incomingStream 和栈中每个active stream 获取策略
1：栈为空，则立即入栈返回INTERRUPT_GRANTED
2：栈非空，遍历栈

STRATEGY_REJECT
1：栈中有STRATEGY_REJECT，则当前激活的打断为不播放，返回 INTERRUPT_FAILED

STRATEGY_DELAY
1：栈中没有 STRATEGY_REJECT 且有 STRATEGY_DELAY，则当前激活的打断为延时中断，给当前中断观察函数回调 BEGIN PAUSE
同时把当前中断压入栈底（如果当前栈底已经有被打断的的中断时，需要给栈底被打断的中断回调END STOP并删除），返回
INTERRUPT_DELAYED

STRATEGY_EXCLUSIVE
1：有 STRATEGY_EXCLUSIVE 策略时，如果对应的栈中中断为可恢复中断，则需要把 isInterrupted 置为true 并 压入栈底，
如果对应的栈中中断为不可恢复中断，则回调 BEGIN STOP,同时删除该中断项
处理完以上STRATEGY_EXCLUSIVE策略后把当前激活的中断压入栈顶，返回INTERRUPT_GRANTED
STRATEGY_MIX
*/
static int32_t TryRequestInterruptForOutput(InterruptElement *interruptEl)
{
    /* Pre-conditions: interrupt stack is not empty */
    ALOGD("in");
    int32_t ret;
    bool isDelay = false;
    if (InterruptsIsEmpty(true)) {
        goto PUSH_FRONT;
    }
    if (TryReplaceInterrupt(interruptEl, true)) {
        return INTERRUPT_GRANTED;
    }
    if (InterruptCheckIsReject(interruptEl, true, &isDelay)) {
        return INTERRUPT_FAILED;
    }
    ALOGI(" InterruptCheckIsReject isDelay :%d", isDelay);
    /* 每次插入 delay 插入到栈的底部，同时处理已经有后台的情况(1: 之前被压后台的流 2: 之前的delay ) */
    if (isDelay) {
        return PushInterruptEntryAtBack((InterruptElement *)interruptEl, true);
    }
    /* 处理独占策略 */
    ret = ProcessExclusiveInterrupt(interruptEl, true);
    if (ret != AUDIO_SUCCESS) {
        return INTERRUPT_FAILED;
    }
PUSH_FRONT:
    /* 插入 栈顶 */
    ret = PushInterruptEntryAtFront((InterruptElement *)interruptEl, true);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("PushInterruptEntryAtFront failed streamType:0x%x sessionID:%u", interruptEl->streamType,
            interruptEl->sessionID);
        return INTERRUPT_FAILED;
    }
    ALOGD("out");
    return INTERRUPT_GRANTED;
}

/*
当前激活打断的 AudioStreamType incomingStream 和栈中每个active stream 获取策略
1：栈为空，则立即入栈返回INTERRUPT_GRANTED
2：栈非空，遍历栈

STRATEGY_REJECT
1：栈中有STRATEGY_REJECT，则当前激活的打断为不播放，返回 INTERRUPT_FAILED

STRATEGY_DELAY
1：栈中没有 STRATEGY_REJECT 且有 STRATEGY_DELAY，则当前激活的打断为延时中断，给当前中断观察函数回调 BEGIN PAUSE
同时把当前中断压入栈底（如果当前栈底已经有被打断的的中断时，需要给栈底被打断的中断回调END STOP并删除），返回
INTERRUPT_DELAYED

STRATEGY_EXCLUSIVE
1：有 STRATEGY_EXCLUSIVE 策略时，如果对应的栈中中断为可恢复中断，则需要把 isInterrupted 置为true 并 压入栈底，
如果对应的栈中中断为不可恢复中断，则回调 BEGIN STOP,同时删除该中断项
处理完以上STRATEGY_EXCLUSIVE策略后把当前激活的中断压入栈顶，返回INTERRUPT_GRANTED
STRATEGY_MIX
*/
static int32_t TryRequestInterruptForInput(InterruptElement *interruptEl)
{
    /* Pre-conditions: interrupt stack is not empty */
    ALOGD("in");
    int32_t ret;
    bool isDelay = false;
    if (InterruptsIsEmpty(false)) {
        goto PUSH_FRONT;
    }
    if (TryReplaceInterrupt(interruptEl, false)) {
        return INTERRUPT_GRANTED;
    }
    if (InterruptCheckIsReject(interruptEl, false, &isDelay)) {
        return INTERRUPT_FAILED;
    }
    if (isDelay) {
        return PushInterruptEntryAtBack((InterruptElement *)interruptEl, false);
    }
    ret = ProcessExclusiveInterrupt(interruptEl, false);
    if (ret != AUDIO_SUCCESS) {
        return INTERRUPT_FAILED;
    }
PUSH_FRONT:
    /* 插入 栈顶 */
    ret = PushInterruptEntryAtFront((InterruptElement *)interruptEl, false);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("InterruptMgrPushFront failed streamType:0x%x sessionID:%u", interruptEl->streamType,
            interruptEl->sessionID);
        return INTERRUPT_FAILED;
    }
    ALOGD("out");
    return INTERRUPT_GRANTED;
}

static int32_t TryRequestInterruptForTwoWay(InterruptElement *interruptEl)
{
    /* Pre-conditions: interrupt stack is not empty */
    ALOGD("in");
    int32_t ret;
    bool isOutPutDelay = false;
    bool isInPutDelay = false;
    if (InterruptsIsEmpty(true) && InterruptsIsEmpty(false)) {
        goto PUSH_FRONT;
    }
    if (TryReplaceInterrupt(interruptEl, false)) {
        return INTERRUPT_GRANTED;
    }
    if (InterruptCheckIsReject(interruptEl, false, &isInPutDelay) ||
        InterruptCheckIsReject(interruptEl, true, &isOutPutDelay)) {
        return INTERRUPT_FAILED;
    }
    /* 无论是输入流的延时或输出流的延时，都需要进行处理，只要有一个延时整个流类型都需要延时处理 */
    if (isInPutDelay || isOutPutDelay) {
        ret = PushInterruptEntryAtBack((InterruptElement *)interruptEl, true);
        if (ret == INTERRUPT_FAILED) {
            return ret;
        }
        return PushInterruptEntryAtBack((InterruptElement *)interruptEl, false);
    }
    if (ProcessExclusiveInterrupt(interruptEl, true) != AUDIO_SUCCESS ||
        ProcessExclusiveInterrupt(interruptEl, false) != AUDIO_SUCCESS) {
        return INTERRUPT_FAILED;
    }
PUSH_FRONT:
    ret = PushInterruptEntryAtFront((InterruptElement *)interruptEl, true);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("InterruptMgrPushFront failed streamType:0x%x sessionID:%u", interruptEl->streamType,
            interruptEl->sessionID);
        return INTERRUPT_FAILED;
    }
    ret = PushInterruptEntryAtFront((InterruptElement *)interruptEl, false);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("InterruptMgrPushFront failed streamType:0x%x sessionID:%u", interruptEl->streamType,
            interruptEl->sessionID);
        RemoveInterruptInternal(interruptEl, true);
        return INTERRUPT_FAILED;
    }
    ALOGD("out");
    return INTERRUPT_GRANTED;
}

int32_t InterruptMgrTryRequest(InterruptElement *interruptEl)
{
    int32_t ret;
    CHK_NULL_RETURN(interruptEl, INTERRUPT_FAILED, "interruptEl is NULL");
    StrategyConfig *config = GetStrategyConfig(interruptEl->streamType);
    CHK_NULL_RETURN(config, INTERRUPT_FAILED, "config is NULL");
    switch (config->streamDir) {
        case AUDIO_STREAM_OUT:
            ret = TryRequestInterruptForOutput(interruptEl);
            break;
        case AUDIO_STREAM_IN:
            ret = TryRequestInterruptForInput(interruptEl);
            break;
        case AUDIO_STREAM_OUT_IN:
            ret = TryRequestInterruptForTwoWay(interruptEl);
            break;
        default:
            ALOGI("invalid stream:%x", interruptEl->streamType);
            ret = INTERRUPT_FAILED;
            break;
    }
    return ret;
}

static uint32_t DumpInterruptInfo(const List *list, InterruptDebugInfo *interruptInfo, uint32_t maxInterrupts)
{
    CHK_NULL_RETURN(list, AUDIO_ERROR, "list is NULL");
    CHK_NULL_RETURN(interruptInfo, AUDIO_ERROR, "interruptInfo is NULL");
    uint32_t index = 0;
    for (ListIterator it = ListIterBegin(list); it != ListIterEnd(list); it = ListIterNext(list, it)) {
        InterruptElement *interruptEl = (InterruptElement *)ListIterData(it);
        interruptInfo->streamType = interruptEl->streamType;
        interruptInfo->sessionID = interruptEl->sessionID;
        interruptInfo->currentStrategy = interruptEl->strategy;
        interruptInfo->streamCreateTime = interruptEl->streamCreateTime;
        interruptInfo->streamDestroyTime = interruptEl->streamDestroyTime;
        index++;
        if (index >= maxInterrupts) {
            break;
        }
    }
    return index;
}

int32_t InterruptMgrDumpInfo(AudioInterruptDebugInfo *audioInterruptInfo)
{
    CHK_NULL_RETURN(audioInterruptInfo, AUDIO_ERROR, "audioInterruptInfo is NULL");
    AUDIO_LOCK(g_interruptMgrCtx.interruptLock);
    List *list = &g_interruptMgrCtx.outputInterrupts;
    uint32_t cnt = DumpInterruptInfo(list, audioInterruptInfo->outputInterruptInfo, MAX_INST_NUM);
    if ((int32_t)cnt == AUDIO_ERROR) {
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return AUDIO_ERROR;
    }
    audioInterruptInfo->outputInterruptCnt = cnt;

    list = &g_interruptMgrCtx.inputInterrupts;
    cnt = DumpInterruptInfo(list, audioInterruptInfo->inputInterruptInfo, MAX_INST_NUM);
    if ((int32_t)cnt == AUDIO_ERROR) {
        AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
        return AUDIO_ERROR;
    }
    audioInterruptInfo->inputInterruptCnt = cnt;
    AUDIO_UNLOCK(g_interruptMgrCtx.interruptLock);
    return AUDIO_SUCCESS;
}
