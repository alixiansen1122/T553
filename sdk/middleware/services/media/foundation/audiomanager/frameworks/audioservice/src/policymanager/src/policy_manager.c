/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio service
* Author: Media Software Group
* Create: 2021-02-28
*/

#include <limits.h>
#include <math.h>
#include "securec.h"
#include "policy_config.h"
#include "policy_manager_internal.h"
#include "interrupt_manager.h"
#include "device_manager.h"
#include "devices_manager.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "media_thread_adapt.h"
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
#include "hifi_clock_freq_manager.h"
#endif
#include "policy_manager.h"

#define LOG_MODULE_NAME "PolicyMgr"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static PolicyMgrContext g_policyMgrCtx;
static bool g_policyMgrInited = false;
static MediaMutexHandle g_policyMgrMutex = NULL;
static bool CheckInterruptIsValid(const AudioInterruptProxy *interrupt, bool isActivating);
static int32_t AudioDeviceCallbackFunc(DeviceCallBackType type, uint32_t dataLen, const uint8_t *callbackData);

int32_t PolicyMgrInit(void)
{
    int32_t ret;
    if (g_policyMgrMutex == NULL) {
        MediaMutexGlobalLock();
        if (g_policyMgrMutex == NULL) {
            g_policyMgrMutex = MediaMutexCreate(NULL);
        }
        MediaMutexGlobalUnLock();
    }

    AUDIO_LOCK(g_policyMgrMutex);
    if (g_policyMgrInited) {
        AUDIO_UNLOCK(g_policyMgrMutex);
        return AUDIO_SUCCESS;
    }
    ret = memset_s(&g_policyMgrCtx, sizeof(PolicyMgrContext),
                   0x0, sizeof(PolicyMgrContext));
    if (ret != EOK) {
        AUDIO_UNLOCK(g_policyMgrMutex);
        ALOGE("memset_s: g_policyMgrCtx failed");
        return AUDIO_ERROR;
    }
    ret = AvlDevicesMgrInit();
    if (ret != AUDIO_SUCCESS) {
        AUDIO_UNLOCK(g_policyMgrMutex);
        ALOGE("AvlDevicesMgrInit failed");
        return AUDIO_ERROR;
    }
    AudioDeviceCallback deviceCallBack;
    deviceCallBack.OnCallback = AudioDeviceCallbackFunc;
    ret = DeviceMgrRegisterDeviceCallback(&deviceCallBack);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("DeviceMgrRegisterDeviceCallback failed");
    }
    ALOGI("DeviceMgrRegisterDeviceCallback SUCCESS");
    ret = LoadPolicyConfig();
    if (ret != AUDIO_SUCCESS) {
        AUDIO_UNLOCK(g_policyMgrMutex);
        ALOGE("LoadPolicyConfig failed");
        return ret;
    }
    ALOGI("LoadPolicyConfig SUCCESS");
    InterruptMgrInit();
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    HifiClockFreqMgrInit();
#endif
    g_policyMgrInited = true;
    AUDIO_UNLOCK(g_policyMgrMutex);
    ALOGI("PolicyMgrInit SUCCESS");
    return AUDIO_SUCCESS;
}

int32_t PolicyMgrDeInit(void)
{
    AUDIO_LOCK(g_policyMgrMutex);
    if (!g_policyMgrInited) {
        AUDIO_UNLOCK(g_policyMgrMutex);
        return AUDIO_SUCCESS;
    }
    AvlDevicesMgrDeInit();
    InterruptMgrDeInit();
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    HifiClockFreqMgrDeInit();
#endif
    g_policyMgrInited = false;
    AUDIO_UNLOCK(g_policyMgrMutex);
    ALOGI("SUCCESS");
    return AUDIO_SUCCESS;
}

bool PolicyMgrCanBeRoutedToThisDevice(AudioDeviceType device, bool isOutput)
{
    CHK_FAILED_RETURN(g_policyMgrInited, true, false, "Policy Mgr not init");
    AUDIO_LOCK(g_policyMgrMutex);
    if (!AvlDevicesMgrIsAvailable(device)) {
        AUDIO_UNLOCK(g_policyMgrMutex);
        return false;
    }
    AUDIO_UNLOCK(g_policyMgrMutex);

    /* 设备类型和isOutput是否匹配，例如：device:MIC 且 isOutput:false 或 devic:Speaker 且 isOutput:true 等 */
    bool isOutputDevice = (((uint32_t)device & IN_FLAG) != 0) ? false : true;
    if ((isOutput && isOutputDevice) || (!isOutput && !isOutputDevice)) {
        return true;
    }

    return false;
}

bool PolicyMgrGetSupportDevicesForStream(AudioStreamType streamType, uint32_t *supportDeviceCount,
                                         AudioDeviceType **supportDevices)
{
    CHK_FAILED_RETURN(g_policyMgrInited, true, false, "Policy Mgr not init");
    CHK_NULL_RETURN(supportDeviceCount, false, "supportDeviceCount is NULL");
    CHK_NULL_RETURN(supportDevices, false, "supportDevices is NULL");
    StrategyConfig *strategyConfig = InquireStrategyConfigForStream(streamType, true);
    CHK_NULL_RETURN(strategyConfig, false, "InquireStrategyConfigForStream failed");
    *supportDeviceCount = strategyConfig->supportDeviceCount;
    *supportDevices = strategyConfig->supportDevices;
    return true;
}

int32_t PolicyMgrRegisterPolicyCallback(const AudioPolicyCallback *callback)
{
    CHK_FAILED_RETURN(g_policyMgrInited, true, AUDIO_NO_INIT, "Policy Mgr not init");
    CHK_NULL_RETURN(callback, AUDIO_INVALID_PARAMS, "callback is NULL");
    AUDIO_LOCK(g_policyMgrMutex);
    g_policyMgrCtx.callback.OnCallback = callback->OnCallback;
    AUDIO_UNLOCK(g_policyMgrMutex);
    return AUDIO_SUCCESS;
}

static int32_t ProcessDeviceConnected(uint32_t dataLen, const uint8_t *callbackData)
{
    AudioDeviceInfo *deviceInfo = (AudioDeviceInfo *)callbackData;
    ALOGD("ProcessDeviceConnected in");
    if (dataLen != sizeof(AudioDeviceInfo) ||
        deviceInfo == NULL) {
        ALOGE("invalid dataLen %u OR deviceInfo", dataLen);
        return AUDIO_ERROR;
    }
    if (deviceInfo->device == OUT_BLUETOOTH_A2DP) {
        if (AvlDevicesMgrIsAvailable(IN_BLUETOOTH_A2DP)) {
            ALOGI("not support connect A2dp source device, when a2dp sink device is connected!");
            return AUDIO_NOT_SUPPORT;
        }
    }
    int32_t ret = AvlDevicesMgrPushFront(deviceInfo);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AvlDevicesMgrPushFront failed :%x", ret);
        return ret;
    }
    ALOGI("device flag:0x%x type:0x%x", deviceInfo->flag, deviceInfo->device);
    if (deviceInfo->flag == OUTPUT_DEVICES_FLAG &&
        deviceInfo->device != OUT_BLUETOOTH_SCO &&
        deviceInfo->device != OUT_MODEM &&
        deviceInfo->device != OUT_MODEM_HEADSET &&
        InterruptMgrCheckCanbeRouted(deviceInfo) &&
        g_policyMgrCtx.callback.OnCallback != NULL) {
        g_policyMgrCtx.callback.OnCallback(POLICY_ROUTE_DEVICE_CHANGED,
                                           sizeof(AudioDeviceType), (uint8_t *)&deviceInfo->device);
    }
    ALOGD("out");
    return AUDIO_SUCCESS;
}

static int32_t ProcessDeviceDisConnected(uint32_t dataLen, const uint8_t *callbackData)
{
    AudioDeviceInfo *deviceInfo = (AudioDeviceInfo *)callbackData;
    ALOGD("ProcessDeviceDisConnected in");
    if (dataLen != sizeof(AudioDeviceInfo) ||
        deviceInfo == NULL) {
        ALOGE("invalid dataLen %u OR deviceInfo", dataLen);
        return AUDIO_ERROR;
    }
    if (deviceInfo->device == OUT_BLUETOOTH_A2DP) {
        if (AvlDevicesMgrIsAvailable(IN_BLUETOOTH_A2DP)) {
            ALOGI("not need to disconnect A2dp source device!");
            return AUDIO_NOT_SUPPORT;
        }
    }
    int32_t ret = AvlDevicesMgrPop(deviceInfo);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AvlDevicesMgrPop failed :%x", ret);
        return ret;
    }

    if (deviceInfo->flag == OUTPUT_DEVICES_FLAG && deviceInfo->device != OUT_BLUETOOTH_SCO) {
        AudioDeviceType routedDevice = InterruptMgrGetCurrRoutedDevice(deviceInfo);
        if (routedDevice != UNKNOWN &&
            g_policyMgrCtx.callback.OnCallback != NULL) {
            g_policyMgrCtx.callback.OnCallback(POLICY_ROUTE_DEVICE_CHANGED,
                                               sizeof(routedDevice), (uint8_t *)&routedDevice);
        }
    }
    ALOGD("out");
    return AUDIO_SUCCESS;
}

static int32_t AudioDeviceCallbackFunc(DeviceCallBackType type, uint32_t dataLen, const uint8_t *callbackData)
{
    CHK_FAILED_RETURN(g_policyMgrInited, true, AUDIO_NO_INIT, "Policy Mgr not init");
    ALOGD("DeviceCallBackType %d dataLen:%u ", type, dataLen);
    switch (type) {
        case DEVICE_CALLBACK_DEVICE_CONNECTED:
            return ProcessDeviceConnected(dataLen, callbackData);
        case DEVICE_CALLBACK_DEVICE_DISCONNECTED:
            return ProcessDeviceDisConnected(dataLen, callbackData);
        default:
            ALOGE("pass DeviceCallBackType type %d", type);
            return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t ConvertInterruptToElement(const AudioInterruptProxy *interrupt, InterruptElement *interruptEl)
{
    if (interrupt == NULL ||
        interruptEl == NULL) {
        return AUDIO_INVALID_PARAMS;
    }
    int32_t ret = memset_s(&interruptEl->routedStreamDesc, sizeof(AudioRoutedStreamDescriptor),
                           0x0, sizeof(AudioRoutedStreamDescriptor));
    if (ret != EOK) {
        ALOGE("memset_s: routedStreamDesc failed");
        return AUDIO_ERROR;
    }
    interruptEl->sessionID = interrupt->sessionID;
    interruptEl->streamType = interrupt->streamType;
    interruptEl->strategy = STRATEGY_NONE;
    interruptEl->isInterrupted = false;
    interruptEl->isResumed = false;
    interruptEl->isExecutCallbackfunc = false;
    interruptEl->interruptedStreamType = AUDIO_STREAM_INVALID;
    interruptEl->delayedStreamType = AUDIO_STREAM_INVALID;
    interruptEl->interruptTime = 0;
    interruptEl->streamCreateTime = 0;
    interruptEl->streamDestroyTime = 0;
    interruptEl->owner = interrupt->owner;
    interruptEl->userData = interrupt->userData;
    interruptEl->interruptListenerCb = interrupt->interruptListenerCb;
    return AUDIO_SUCCESS;
}

static int32_t RemoveInterruptMgrEntry(const AudioInterruptProxy *interruptToRemove)
{
    int32_t ret;
    InterruptElement toRmInterruptEl;
    ret = ConvertInterruptToElement(interruptToRemove, &toRmInterruptEl);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("ConvertInterruptToElement failed ");
        return AUDIO_ERROR;
    }
    /* 先删除 */
    ret = InterruptMgrRemove(&toRmInterruptEl);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("InterruptMgrRemove failed ");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t TryPushInterruptMgrEntry(const AudioInterruptProxy *interruptToPush)
{
    ALOGD("in");
    InterruptElement newInterruptEl;
    int32_t ret = ConvertInterruptToElement(interruptToPush, &newInterruptEl);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("ConvertInterruptToElement failed ");
        return INTERRUPT_FAILED;
    }
    return InterruptMgrTryRequest(&newInterruptEl);
}

static inline void UpdateFocusAudioInterrupt(const AudioInterruptProxy *interrupt)
{
    g_policyMgrCtx.focusInterrupt = *interrupt;
}

int32_t PolicyMgrActivateAudioInterrupt(const AudioInterruptProxy *interrupt)
{
    int32_t ret;
    CHK_FAILED_RETURN(g_policyMgrInited, true, AUDIO_NO_INIT, "Policy Mgr not init");
    CHK_NULL_RETURN(interrupt, AUDIO_INVALID_PARAMS, "interrupt is NULL");
    ALOGI("streamType:0x%x sessionID:%u", interrupt->streamType, interrupt->sessionID);
    AUDIO_LOCK(g_policyMgrMutex);
    if (!CheckInterruptIsValid(interrupt, true)) {
        ALOGE("InterruptIs invalid streamType:0x%x sessionID:%u", interrupt->streamType,
              interrupt->sessionID);
        AUDIO_UNLOCK(g_policyMgrMutex);
        return INTERRUPT_FAILED;
    }
    /* step 1 : interrupt might already be time out in the stack, handle it */
    InterruptMgrHandleException();
    ret = TryPushInterruptMgrEntry(interrupt);
    if (ret == INTERRUPT_GRANTED) {
        UpdateFocusAudioInterrupt(interrupt);
    }
    if (ret == INTERRUPT_GRANTED ||
        ret == INTERRUPT_DELAYED) {
        InterruptMgrSignalExceptionCheckThread();
    }
    AUDIO_UNLOCK(g_policyMgrMutex);
    ALOGD("out");
    return ret;
}

static void UpdateAudioMaxSessionID(AudioSession sessionID)
{
    if (sessionID > g_policyMgrCtx.maxSessionID) {
        g_policyMgrCtx.maxSessionID = sessionID;
    }
}

int32_t PolicyMgrDeactivateAudioInterrupt(const AudioInterruptProxy *interrupt)
{
    int32_t ret;
    CHK_FAILED_RETURN(g_policyMgrInited, true, AUDIO_NO_INIT, "Policy Mgr not init");
    CHK_NULL_RETURN(interrupt, AUDIO_INVALID_PARAMS, "interrupt is NULL");
    ALOGI("Deactivate Interrupt streamType:0x%x sessionID:%u", interrupt->streamType, interrupt->sessionID);
    AUDIO_LOCK(g_policyMgrMutex);
    if (!CheckInterruptIsValid(interrupt, false)) {
        ALOGE("InterruptIs invalid streamType:0x%x sessionID:%u", interrupt->streamType,
              interrupt->sessionID);
        AUDIO_UNLOCK(g_policyMgrMutex);
        return AUDIO_INVALID_PARAMS;
    }
    ret = RemoveInterruptMgrEntry(interrupt);
    if (ret == AUDIO_SUCCESS) {
        UpdateAudioMaxSessionID(interrupt->sessionID);
    }
    AUDIO_UNLOCK(g_policyMgrMutex);
    ALOGD("out");
    return ret;
}

int32_t PolicyMgrDumpInterruptInfo(AudioInterruptDebugInfo *audioInterruptInfo)
{
    CHK_FAILED_RETURN(g_policyMgrInited, true, AUDIO_ERROR, "Policy Mgr not init");
    CHK_NULL_RETURN(audioInterruptInfo, AUDIO_ERROR, "audioInterruptInfo is NULL");

    return InterruptMgrDumpInfo(audioInterruptInfo);
}

static void ResetActiveStreams(void)
{
    g_policyMgrCtx.streamStrategy.currentStrategy = STRATEGY_NONE;
    g_policyMgrCtx.streamStrategy.activeStreamNum = 0;
}

static AudioStreamStrategy *GetStrategyForStreamCreate(AudioSession sessionID, const AudioStreamDescriptor *streamDesc,
    bool isOutput)
{
    CHK_NULL_RETURN(streamDesc, NULL, "streamDesc is NULL");
    ALOGI("streamType:0x%x isOutput:%d", streamDesc->streamType, isOutput);
    /* step 1 先判断 sessionID 是否 激活 Interrupt，没有激活 Interrupt 则不允许 获取策略 */
    if (!InterruptMgrCreateStream(sessionID, streamDesc, isOutput)) {
        return NULL;
    }
    ResetActiveStreams();
    /* 获取流和对应的混音权重 */
    if (!InterruptMgrGetActiveStream(&g_policyMgrCtx.streamStrategy, streamDesc->streamType, isOutput)) {
        ResetActiveStreams();
        return NULL;
    }
    return &g_policyMgrCtx.streamStrategy;
}

static AudioStreamStrategy *GetStrategyForStreamResume(const AudioStreamDescriptor *streamDesc, bool isOutput)
{
    CHK_NULL_RETURN(streamDesc, NULL, "streamDesc is NULL");
    ResetActiveStreams();
    /* 获取流和对应的混音权重 */
    if (!InterruptMgrGetActiveStream(&g_policyMgrCtx.streamStrategy, streamDesc->streamType, isOutput)) {
        ResetActiveStreams();
        return NULL;
    }
    return &g_policyMgrCtx.streamStrategy;
}

static AudioStreamStrategy *GetStrategyForStreamDestroy(AudioSession sessionID, const AudioStreamDescriptor *streamDesc,
    bool isOutput)
{
    CHK_NULL_RETURN(streamDesc, NULL, "streamDesc is NULL");
    ALOGI("streamType:0x%x isOutput:%d", streamDesc->streamType, isOutput);
    /* step 1 先判断 sessionID 是否 激活 Interrupt，没有激活 Interrupt 则不允许 获取策略 */
    ResetActiveStreams();
    if (!InterruptMgrDestroyStream(sessionID, streamDesc, isOutput)) {
        ALOGE("sessionID:%u destroy stream:0x%x failed", sessionID, streamDesc->streamType);
        return NULL;
    }
    /* 获取流和对应的混音权重 */
    if (!InterruptMgrGetActiveStream(&g_policyMgrCtx.streamStrategy, streamDesc->streamType, isOutput)) {
        ResetActiveStreams();
        ALOGE("sessionID:%u get active stream:0x%x failed", sessionID, streamDesc->streamType);
        return NULL;
    }
    InterruptMgrSignalExceptionCheckThread();
    return &g_policyMgrCtx.streamStrategy;
}

AudioStreamStrategy *PolicyMgrGetStrategyForStream(AudioSession sessionID, const AudioStreamDescriptor *streamDesc,
                                                   StreamStatus streamStatus)
{
    CHK_FAILED_RETURN(g_policyMgrInited, true, NULL, "Policy Mgr not init");
    bool isOutput = (((uint32_t)streamStatus & STREAM_IN_FLAG) != STREAM_IN_FLAG);
    switch (streamStatus) {
        case STREAM_OUT_CREATE:
        case STREAM_IN_CREATE:
            return GetStrategyForStreamCreate(sessionID, streamDesc, isOutput);
        case STREAM_OUT_RESUME:
        case STREAM_IN_RESUME:
            return GetStrategyForStreamResume(streamDesc, isOutput);
        case STREAM_OUT_DESTROY:
        case STREAM_IN_DESTROY:
            return GetStrategyForStreamDestroy(sessionID, streamDesc, isOutput);
        default:
            ALOGE("unkown streamStatus");
            break;
    }
    return NULL;
}

static bool CheckInterruptIsValid(const AudioInterruptProxy *interrupt, bool isActivating)
{
    if (interrupt == NULL || interrupt->interruptListenerCb == NULL) {
        return false;
    }
    /* 1.不同应用，新的 interrupt 激活需要 sessionID 递增 */
    /* 2.相同应用，新的 interrupt 激活需要 sessionID 比已释放的 sessionID 最大值要大 */
    /* 3.相同应用，考虑 focusInterrupt 不切换焦点更改流类型场景，例如：蓝牙语音通话，流类型从 RING 切到 BT SCO */
    if (isActivating) {
        if (g_policyMgrCtx.focusInterrupt.userData != interrupt->userData) {
            if (interrupt->sessionID <= g_policyMgrCtx.focusInterrupt.sessionID) {
                return false;
            }
        } else {
            if (g_policyMgrCtx.maxSessionID != 0 && interrupt->sessionID <= g_policyMgrCtx.maxSessionID) {
                return false;
            }
        }
    }
    if (interrupt->streamType == AUDIO_STREAM_NONE ||
        interrupt->streamType == AUDIO_STREAM_INVALID) {
        return false;
    }
    return true;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
