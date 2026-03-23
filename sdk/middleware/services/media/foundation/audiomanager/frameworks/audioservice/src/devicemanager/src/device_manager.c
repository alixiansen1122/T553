/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#include <stdlib.h>
#include <stdio.h>
#include "device_config.h"
#include "device_manager_internel.h"
#include "device_hal.h"
#include "securec.h"
#include "audio_errors.h"
#include "audio_capture.h"
#include "audio_render.h"
#include "media_thread_adapt.h"
#include "device_manager.h"
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
#include "hifi_clock_freq_manager.h"
#endif

#define LOG_MODULE_NAME "DeviceMgr"

#define OUT_SPEAKER_NAME         "speaker"
#define OUT_BLUETOOTH_SCO_NAME   "out_bt_sco"
#define OUT_BLUETOOTH_A2DP_NAME  "out_bt_a2dp"
#define OUT_MODEM_NAME           "out_modem"
#define OUT_MODEM_HEADSET_NAME   "out_modem_headset"
#define IN_BUILTIN_MIC_NAME      "in_mic"
#define IN_BLUETOOTH_SCO_NAME    "in_bt_sco"
#define IN_BLUETOOTH_A2DP_NAME   "in_bt_a2dp"
#define IN_MODEM_NAME            "in_modem"
#define IN_MODEM_HEADSET_NAME    "in_modem_headset"

typedef struct {
    int32_t             init;
    uint32_t            deviceCount;
    MediaMutexHandle     mutex;
    AudioDeviceCallback callback;
} DeviceMgrCtx;

static DeviceMgrCtx  g_deviceMgrctx = {
    .init = false,
    .deviceCount = 0,
    .callback = { NULL }
};

AudioDeviceInfoInternel g_allAudioDevice[] = {
    {
        {INPUT_DEVICES_FLAG, IN_BUILTIN_MIC,           IN_BUILTIN_MIC_NAME},
        AUDIO_DEVICE_CONNECTED
    },
    {
        {INPUT_DEVICES_FLAG, IN_BLUETOOTH_SCO, IN_BLUETOOTH_SCO_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
    {
        {OUTPUT_DEVICES_FLAG, OUT_BUILTIN_SPEAKER,      OUT_SPEAKER_NAME},
        AUDIO_DEVICE_CONNECTED
    },
    {
        {OUTPUT_DEVICES_FLAG, OUT_BLUETOOTH_SCO,        OUT_BLUETOOTH_SCO_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
    {
        {OUTPUT_DEVICES_FLAG, OUT_BLUETOOTH_A2DP,       OUT_BLUETOOTH_A2DP_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
    {
        {INPUT_DEVICES_FLAG, IN_BLUETOOTH_A2DP,         IN_BLUETOOTH_A2DP_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
    {
        {INPUT_DEVICES_FLAG, IN_MODEM,                  IN_MODEM_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
    {
        {OUTPUT_DEVICES_FLAG, OUT_MODEM,                OUT_MODEM_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
    {
        {INPUT_DEVICES_FLAG, IN_MODEM_HEADSET,          IN_MODEM_HEADSET_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },

    {
        {OUTPUT_DEVICES_FLAG, OUT_MODEM_HEADSET,        OUT_MODEM_HEADSET_NAME},
        AUDIO_DEVICE_DISCONNECTED
    },
};

static int32_t LoadDevice(void)
{
    g_deviceMgrctx.deviceCount = (uint32_t)(sizeof(g_allAudioDevice) / sizeof(AudioDeviceInfoInternel));
    return AUDIO_SUCCESS;
}

int32_t DeviceMgrInit(void)
{
    int32_t ret;
    CHK_COND_RETURN(g_deviceMgrctx.init == true, AUDIO_SUCCESS, "DeviceMgrInit already init success");
    ret = LoadConfig();
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }
    ret = LoadAudioHal();
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }
    ret = GetAdapterCapability();
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }
    ret = LoadDevice();
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }

    g_deviceMgrctx.mutex = MediaMutexCreate(NULL);
    ALOGI("DeviceMgrInit init success");
    g_deviceMgrctx.init = true;
    return AUDIO_SUCCESS;
}

static bool CheckDeviceFlag(const AudioDeviceInfoInternel *dev, uint32_t flag)
{
    return (((uint32_t)dev->deviceInfo.flag & flag) != 0 && dev->state == AUDIO_DEVICE_CONNECTED);
}

static int32_t GetAvailableDeviceCountByFlag(uint32_t flag, uint32_t *deviceCount)
{
    uint32_t deviceNum = 0;
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    for (uint32_t i = 0; i < g_deviceMgrctx.deviceCount; i++) {
        if (CheckDeviceFlag(&g_allAudioDevice[i], flag)) {
            deviceNum++;
        }
    }
    *deviceCount = deviceNum;
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    return AUDIO_SUCCESS;
}

int32_t DeviceMgrGetAvailableDeviceCount(uint32_t flag, uint32_t *deviceCount)
{
    if (g_deviceMgrctx.init == false ||
        deviceCount == NULL) {
        return AUDIO_ERROR;
    }
    return GetAvailableDeviceCountByFlag(flag, deviceCount);
}

int32_t DeviceMgrGetAvailableDevices(uint32_t flag, uint32_t deviceCount, AudioDeviceInfo *devices)
{
    uint32_t deviceNum = 0;
    uint32_t count = 0;
    int32_t ret;
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
    ret = GetAvailableDeviceCountByFlag(flag, &deviceNum);
    if (ret != AUDIO_SUCCESS || deviceNum != deviceCount) {
        ALOGE("deviceCount :%u is invalid", deviceCount);
        return AUDIO_ERROR;
    }
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    for (uint32_t i = 0; i < g_deviceMgrctx.deviceCount; i++) {
        if (CheckDeviceFlag(&g_allAudioDevice[i], flag) && count < deviceNum) {
            devices[count] = g_allAudioDevice[i].deviceInfo;
            count++;
        }
    }
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    return AUDIO_SUCCESS;
}

#ifdef DEVICE_CAPABILITY
static bool GetDeviceIsSupportOffload(AudioDeviceType device, const AudioOffloadInfo *info)
{
    AudioCapability *cap = NULL;
    cap = GetDeviceCapability(device);
    CHK_NULL_RETURN_NO_LOG(cap, false);
    for (uint32_t j = 0; j < cap->numFormat; j++) {
        if (cap->formats != NULL && info != NULL
            && cap->formats[j] == info->format) {
            return true;
        }
    }
    return false;
}
#endif

bool DeviceMgrIsSupportOffload(AudioDeviceType device, const AudioOffloadInfo *info)
{
    uint32_t i;
#ifdef DEVICE_CAPABILITY
    CHK_COND_RETURN(g_deviceMgrctx.init == false, false, NULL);
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    for (i = 0; i < g_deviceMgrctx.deviceCount; i++) {
        if (g_allAudioDevice[i].deviceInfo.device == device
            && g_allAudioDevice[i].state == AUDIO_DEVICE_CONNECTED) {
            break;
        }
    }
    if (i >= g_deviceMgrctx.deviceCount) {
        AUDIO_UNLOCK(g_deviceMgrctx.mutex);
        return false;
    }
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    return GetDeviceIsSupportOffload(device, info);
#else
    AUDIO_UNUSED(device);
    CHK_NULL_RETURN_NO_LOG(info, false);
    static AudioCodecFormat audioFormat[] = {
        PCM, MP3, FLAC, AAC_LC, AAC_HE_V1, AAC_HE_V2, VORBIS, OPUS, SILK, SBC
    };
    for (i = 0; i < sizeof(audioFormat) / sizeof(AudioCodecFormat); i++) {
        if (info->format == audioFormat[i]) {
            return true;
        }
    }
    ALOGE("offload not support format:%d", info->format);
    return false;
#endif
}

static int32_t ParamCheck(AudioDeviceType device, AudioDeviceConnectState state)
{
    if (state != AUDIO_DEVICE_CONNECTED && state != AUDIO_DEVICE_DISCONNECTED) {
        return AUDIO_ERROR;
    }
    if (device == OUT_BUILTIN_SPEAKER || device == IN_BUILTIN_MIC) {
        ALOGE("device:%x is buildin, cannot change state", device);
        return AUDIO_ERROR;
    }
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
    return AUDIO_SUCCESS;
}

static void RestoreDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    for (uint32_t index = 0; index < g_deviceMgrctx.deviceCount; index++) {
        if (g_allAudioDevice[index].deviceInfo.device == device) {
            if (state == AUDIO_DEVICE_CONNECTED) {
                g_allAudioDevice[index].state = AUDIO_DEVICE_DISCONNECTED;
            } else {
                g_allAudioDevice[index].state = AUDIO_DEVICE_CONNECTED;
            }
            break;
        }
    }
}

int32_t DeviceMgrSetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    int32_t changeFlag = false;
    if (ParamCheck(device, state) != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    uint32_t index;
    for (index = 0; index < g_deviceMgrctx.deviceCount; index++) {
        if (g_allAudioDevice[index].deviceInfo.device == device && g_allAudioDevice[index].state != state) {
            g_allAudioDevice[index].state = state;
            changeFlag = true;
            break;
        }
    }
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    if (index >= g_deviceMgrctx.deviceCount) {
        return AUDIO_ERROR;
    }
    ALOGI("device:0x%x, new state:%d", g_allAudioDevice[index].deviceInfo.device, state);
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    if (changeFlag == true && g_deviceMgrctx.callback.OnCallback != NULL) {
        DeviceCallBackType callBackType =
            state == AUDIO_DEVICE_CONNECTED ? DEVICE_CALLBACK_DEVICE_CONNECTED : DEVICE_CALLBACK_DEVICE_DISCONNECTED;
        int32_t ret = g_deviceMgrctx.callback.OnCallback(callBackType, sizeof(AudioDeviceInfo),
            (uint8_t *)&g_allAudioDevice[index].deviceInfo);
        if (ret == AUDIO_NOT_SUPPORT) {
            // If the device status cannot be changed, need to restore the original device status.
            RestoreDeviceConnectionState(device, state);
        }
        changeFlag = false;
        /* get port capability again */
    }
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    return AUDIO_SUCCESS;
}

int32_t DeviceMgrGetDeviceCapability(AudioDeviceType device, AudioCapability *capability)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
    CHK_NULL_RETURN(capability, AUDIO_ERROR, "capability is NULL");
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    for (uint32_t i = 0; i < g_deviceMgrctx.deviceCount; i++) {
        if (g_allAudioDevice[i].deviceInfo.device == device && g_allAudioDevice[i].state == AUDIO_DEVICE_CONNECTED) {
            AudioCapability *cap = NULL;
            cap = GetDeviceCapability(device);
            if (cap == NULL) {
                AUDIO_UNLOCK(g_deviceMgrctx.mutex);
                return AUDIO_ERROR;
            }
            *capability = *cap;
            AUDIO_UNLOCK(g_deviceMgrctx.mutex);
            return AUDIO_SUCCESS;
        }
    }
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    return AUDIO_ERROR;
}

int32_t DeviceMgrRegisterDeviceCallback(const AudioDeviceCallback *callback)
{
    if (callback != NULL) {
        AUDIO_LOCK(g_deviceMgrctx.mutex);
        g_deviceMgrctx.callback.OnCallback = callback->OnCallback;
        AUDIO_UNLOCK(g_deviceMgrctx.mutex);
        return AUDIO_SUCCESS;
    } else {
        return AUDIO_ERROR;
    }
}

int32_t DeviceMgrCreateStreamIn(uint32_t device, AudioStreamConfig attr, Handle *inputStreamHandle)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    SetHifiClockFreqByStreamTypeAndDeviceType(attr.type, (AudioDeviceType)device, true);
#endif
    int32_t ret = OpenInputStream(device, attr, inputStreamHandle);
    if (ret != AUDIO_SUCCESS) {
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
        SetHifiClockFreqByStreamTypeAndDeviceType(attr.type, (AudioDeviceType)device, false);
#endif
    }
    return ret;
}

int32_t DeviceMgrDestroyStreamIn(uint32_t device, Handle inputStreamHandle)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
    return CloseInputStream(device, inputStreamHandle);
}

int32_t DeviceMgrCreateStreamOut(uint32_t device, AudioStreamConfig attr, Handle *outputStreamHandle)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    SetHifiClockFreqByStreamTypeAndDeviceType(attr.type, (AudioDeviceType)device, true);
#endif
    int32_t ret = OpenOutputStream(device, attr, outputStreamHandle);
    if (ret != AUDIO_SUCCESS) {
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
        SetHifiClockFreqByStreamTypeAndDeviceType(attr.type, (AudioDeviceType)device, false);
#endif
    }
    return ret;
}

int32_t DeviceMgrDestroyStreamOut(uint32_t device, Handle outputStreamHandle)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_ERROR, NULL);
    return CloseOutputStream(device, outputStreamHandle);
}

int32_t DeviceMgrDumpInfo(AudioDeviceDebugInfo *audioDeviceInfo)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == 0, AUDIO_ERROR, NULL);
    CHK_NULL_RETURN(audioDeviceInfo, AUDIO_ERROR, "audioInterruptInfo is NULL");
    uint32_t index = 0;
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    for (uint32_t i = 0; i < g_deviceMgrctx.deviceCount; i++) {
        audioDeviceInfo->devicesInfo[i].deviceInfo = g_allAudioDevice[i].deviceInfo;
        audioDeviceInfo->devicesInfo[i].deviceState = g_allAudioDevice[i].state;
        index++;
        if (index >= MAX_INST_NUM) {
            break;
        }
    }
    ALOGI("deviceCount:%d index:%d", g_deviceMgrctx.deviceCount, index);
    audioDeviceInfo->deviceInfoCnt = index;
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    return AUDIO_SUCCESS;
}

int32_t DeviceMgrDeInit(void)
{
    CHK_COND_RETURN(g_deviceMgrctx.init == false, AUDIO_SUCCESS, "DeviceMgrDeInit already deinit success");
    AUDIO_LOCK(g_deviceMgrctx.mutex);
    UnLoadAudioHal();
    AUDIO_UNLOCK(g_deviceMgrctx.mutex);
    g_deviceMgrctx.init = false;
    g_deviceMgrctx.deviceCount = 0;
    MediaMutexDestroy(&g_deviceMgrctx.mutex);
    ALOGI("DeviceMgrDeInit deinit success");
    return AUDIO_SUCCESS;
}

int32_t DeviceMgrInvoke(AudioHaidInvokeId audioHaidInvokeId, char *param, uint32_t *paramlength)
{
    return DeviceHalInvoke(audioHaidInvokeId, param, paramlength);
}

int32_t DeviceMgrRegCallBack(AudioDeviceHaidEventCallback callBack, void *context)
{
    return DeviceHalRegCallBack(callBack, context);
}
