/*
* Copyright (c) @CompanyNameMagicTag. 2020-2020. All rights reserved.
* Description:Audio Adapter
* Author: Media Software Group
* Create: 2020-08-11
*/
#include <stdlib.h>
#include "audio_internal.h"
#include "audio_port_plugin.h"
#include "audio_adapter.h"

#define CONFIG_CHANNEL_COUNT  2 // two channels
#define CODEC_ATTR_BUF_LEN 10
#define EVENT_TYPE_BUF_LEN 10

static struct PluginInvokeNameId g_pluginInvokeNameId[] = {
    {"HaidCreate", PLUGIN_INVOKE_HAID_CREATE},
    {"HaidDestroy", PLUGIN_INVOKE_HAID_DESTROY},
    {"HaidSetEnable", PLUGIN_INVOKE_HAID_SET_ENABLE},
    {"HaidGetEnable", PLUGIN_INVOKE_HAID_GET_ENABLE},
    {"HaidSetEventCallback", PLUGIN_INVOKE_HAID_SET_EVENT_CALL_BACK},
    {"HaidSetVolume", PLUGIN_INVOKE_HAID_SET_VOLUME},
    {"HaidGetVolume", PLUGIN_INVOKE_HAID_GET_VOLUME},
    {"HaidSetHardwareConfig", PLUGIN_INVOKE_HAID_SET_HARDWARE_CONFIG},
    {"HaidSetConfig", PLUGIN_INVOKE_HAID_SET_CONFIG},
    {"HaidGetConfig", PLUGIN_INVOKE_HAID_GET_CONFIG},
    {"HaidSetAllConfig", PLUGIN_INVOKE_HAID_SET_ALL_CONFIG },
    {"HaidGetAllConfig", PLUGIN_INVOKE_HAID_GET_ALL_CONFIG },
    {"HaidTonePlayForExamine", PLUGIN_INVOKE_HAID_TONE_PLAY},
    {"HaidToneStopForExamine", PLUGIN_INVOKE_HAID_TONE_STOP},
    {"HaidSetScene", PLUGIN_INVOKE_HAID_SET_SCENE},
    {"HaidGetScene", PLUGIN_INVOKE_HAID_GET_SCENE},
    {"TwsSetMode", PLUGIN_INVOKE_TWS_SET_MODE},
    {"TwsSetRole", PLUGIN_INVOKE_TWS_SET_ROLE},
    {"AncInit", PLUGIN_INVOKE_ANC_INIT},
    {"AncDeinit", PLUGIN_INVOKE_ANC_DEINIT},
    {"AncSetConfig", PLUGIN_INVOKE_ANC_SET_CONFIG},
    {"AncGetConfig", PLUGIN_INVOKE_ANC_GET_CONFIG},
};
static ParamCallback g_haidAdapterCallBack = NULL;

static int32_t AudioAdapterInitPortCapability(const struct AudioHwAdapter *hwAdapter)
{
    CHK_NULL_RETURN(hwAdapter->capability, MEDIA_HAL_INVALID_PARAM, "Input param capability is null");
    CHK_NULL_RETURN(hwAdapter->adapterDescriptor.ports, MEDIA_HAL_INVALID_PARAM, "Input param ports is null");
    CHK_NULL_RETURN(hwAdapter->portPlugin, MEDIA_HAL_INVALID_PARAM, "Input param portPlugin is null");
    CHK_NULL_RETURN(hwAdapter->portPlugin->GetPortPluginCapability, MEDIA_HAL_INVALID_PARAM,
        "Input param GetPortPluginCapability is null");

    int32_t ret;
    uint32_t deviceId = 0;
    struct AudioPort port = {};
    struct AudioPortCapability *capability = NULL;
    for (uint32_t portIndex = 0; portIndex < hwAdapter->adapterDescriptor.portNum; portIndex++) {
        if (hwAdapter->adapterDescriptor.ports[portIndex].dir == PORT_IN) {
            deviceId = hwAdapter->adapterDescriptor.ports[portIndex].portId;
            capability = &(hwAdapter->capability[portIndex]);
            port = hwAdapter->adapterDescriptor.ports[portIndex];
        } else if (hwAdapter->adapterDescriptor.ports[portIndex].dir == PORT_OUT) {
            deviceId = hwAdapter->adapterDescriptor.ports[portIndex].portId;
            capability = &(hwAdapter->capability[portIndex]);
            port = hwAdapter->adapterDescriptor.ports[portIndex];
        } else if (hwAdapter->adapterDescriptor.ports[portIndex].dir == PORT_OUT_IN) {
            deviceId = hwAdapter->adapterDescriptor.ports[portIndex].portId;
            capability = &(hwAdapter->capability[portIndex]);
            port = hwAdapter->adapterDescriptor.ports[portIndex];
        } else {
            MEDIA_HAL_LOGE(MODULE_NAME, "port index %d direction %d is invalid",
                portIndex, hwAdapter->adapterDescriptor.ports[portIndex].dir);
            return MEDIA_HAL_INVALID_PARAM;
        }
        ret = hwAdapter->portPlugin->GetPortPluginCapability(&port, capability);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "audio adapter get port capability failed");
            return ret;
        }
    }

    CHK_NULL_RETURN(capability, MEDIA_HAL_ERR, "audio adapter capability is null");

    capability->deviceId = deviceId;
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioAdapterOpenPort(struct AudioHwAdapter *hwAdapter)
{
    MediaHalMutexLock(hwAdapter->mutex);
    if (hwAdapter->portOpened) {
        MEDIA_HAL_LOGE(MODULE_NAME, "port has opened");
        MediaHalMutexUnLock(hwAdapter->mutex);
        return MEDIA_HAL_OK;
    }

    int32_t ret = hwAdapter->portPlugin->Open(&hwAdapter->portHandle);
    if (ret != MEDIA_HAL_OK || hwAdapter->portHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin Open failed: 0x%x", ret);
        MediaHalMutexUnLock(hwAdapter->mutex);
        return ret;
    }
    hwAdapter->portOpened = true;
    MediaHalMutexUnLock(hwAdapter->mutex);

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterClosePort(struct AudioAdapter *adapter)
{
    CHK_NULL_RETURN(adapter, MEDIA_HAL_INVALID_PARAM, "input param adapter null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    MediaHalMutexLock(hwAdapter->mutex);
    if (!hwAdapter->portOpened) {
        MediaHalMutexUnLock(hwAdapter->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "port not opened");
        return MEDIA_HAL_ERR;
    }

    int32_t ret = hwAdapter->portPlugin->Close(hwAdapter->portHandle);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwAdapter->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin Close failed: 0x%x", ret);
        return ret;
    }
    hwAdapter->portOpened = false;
    MediaHalMutexUnLock(hwAdapter->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterInitAllPorts(struct AudioAdapter *adapter)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter is invalid");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;

    int32_t ret = AudioAdapterInitPortCapability(hwAdapter);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioAdapterInitPortCapability failed");
        return MEDIA_HAL_ERR;
    }

    ret = AudioAdapterOpenPort(hwAdapter);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioAdapterOpenPort failed");
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioAdapterGetFreeChn(struct AudioAdapter *adapter, bool capture, uint32_t *channelId)
{
    CHK_NULL_RETURN(adapter, MEDIA_HAL_INVALID_PARAM, "input param adapter null");
    CHK_NULL_RETURN(channelId, MEDIA_HAL_INVALID_PARAM, "input param channelId null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    MediaHalMutexLock(hwAdapter->mutex);
    if (capture) {
        for (uint32_t i = 0; i < MAX_CAPTURE_INSTANCE_NUM; i++) {
            if (!hwAdapter->capture[i].used) {
                hwAdapter->capture[i].used = true;
                hwAdapter->capture[i].chnId = i;
                *channelId = hwAdapter->capture[i].chnId;
                MediaHalMutexUnLock(hwAdapter->mutex);
                return MEDIA_HAL_OK;
            }
        }
    } else {
        for (uint32_t i = 0; i < MAX_RENDER_INSTANCE_NUM; i++) {
            if (!hwAdapter->render[i].used) {
                hwAdapter->render[i].used = true;
                hwAdapter->render[i].chnId = i;
                *channelId = hwAdapter->render[i].chnId;
                MediaHalMutexUnLock(hwAdapter->mutex);
                return MEDIA_HAL_OK;
            }
        }
    }
    MediaHalMutexUnLock(hwAdapter->mutex);
    MEDIA_HAL_LOGE(MODULE_NAME, "not get %s free channel", capture ? "capture" : "render");
    return MEDIA_HAL_ERR;
}

static int32_t AudioAdapterFreeChn(struct AudioAdapter *adapter, uint32_t chnId, bool capture)
{
    CHK_NULL_RETURN(adapter, MEDIA_HAL_INVALID_PARAM, "input param adapter null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    MediaHalMutexLock(hwAdapter->mutex);
    if (capture) {
        if (chnId >= MAX_CAPTURE_INSTANCE_NUM) {
            MEDIA_HAL_LOGE(MODULE_NAME, "Can't free %s invaild chnId: %d", capture ? "capture" : "render", chnId);
            MediaHalMutexUnLock(hwAdapter->mutex);
            return MEDIA_HAL_ERR;
        }
        hwAdapter->capture[chnId].used = false;
        hwAdapter->capture[chnId].chnId = 0xFFFFFFFF;
        MediaHalMutexUnLock(hwAdapter->mutex);
        return MEDIA_HAL_OK;
    } else {
        if (chnId >= MAX_RENDER_INSTANCE_NUM) {
            MEDIA_HAL_LOGE(MODULE_NAME, "Can't free %s invaild chnId: %d", capture ? "capture" : "render", chnId);
            MediaHalMutexUnLock(hwAdapter->mutex);
            return MEDIA_HAL_ERR;
        }
        hwAdapter->render[chnId].used = false;
        hwAdapter->render[chnId].chnId = 0xFFFFFFFF;
        MediaHalMutexUnLock(hwAdapter->mutex);
        return MEDIA_HAL_OK;
    }
}

static int32_t HwRenderInit(const struct AudioHwAdapter *hwAdapter, struct AudioHwRender *hwRender,
    uint32_t chnId, bool steroToMono)
{
    struct PortPluginAttr portAttr = {};
    portAttr.type = hwRender->attrs.type;
    portAttr.sampleRate = hwRender->attrs.sampleRate;
    portAttr.channelCount = hwRender->attrs.channelCount;
    portAttr.audioFormat = hwRender->attrs.format;

    int32_t ret = hwAdapter->portPlugin->CreateTrack(hwAdapter->portHandle, hwRender->deviceDesc.pins,
        &portAttr, (AudioHandle *)&hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK || hwRender->trackHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin CreateTrack failed: 0x%x", ret);
        return ret;
    }

    hwRender->common.control.Start = AudioRenderStart;
    hwRender->common.control.Stop = AudioRenderStop;
    hwRender->common.control.Pause = AudioRenderPause;
    hwRender->common.control.Resume = AudioRenderResume;
    hwRender->common.control.Flush = AudioRenderFlush;
    hwRender->common.attr.GetFrameSize = AudioRenderGetFrameSize;
    hwRender->common.attr.GetFrameCount = AudioRenderGetFrameCount;
    hwRender->common.attr.SetSampleAttributes = AudioRenderSetSampleAttributes;
    hwRender->common.attr.GetSampleAttributes = AudioRenderGetSampleAttributes;
    hwRender->common.attr.GetCurrentChannelId = AudioRenderGetCurrentChannelId;
    hwRender->common.attr.SetExtraParams = AudioRenderSetExtraParams;
    hwRender->common.attr.GetExtraParams = AudioRenderGetExtraParams;
    hwRender->common.scene.CheckSceneCapability = AudioRenderCheckSceneCapability;
    hwRender->common.scene.SelectScene = AudioRenderSelectScene;
    hwRender->common.volume.SetMute = AudioRenderSetMute;
    hwRender->common.volume.GetMute = AudioRenderGetMute;
    hwRender->common.volume.SetVolume = AudioRenderSetVolume;
    hwRender->common.volume.GetVolume = AudioRenderGetVolume;
    hwRender->common.volume.GetGainThreshold = AudioRenderGetGainThreshold;
    hwRender->common.volume.GetGain = AudioRenderGetGain;
    hwRender->common.volume.SetGain = AudioRenderSetGain;
    hwRender->common.GetLatency = AudioRenderGetLatency;
    hwRender->common.RenderFrame = AudioRenderRenderFrame;
    hwRender->common.GetRenderPosition = AudioRenderGetRenderPosition;
    hwRender->common.SetRenderSpeed = AudioRenderSetRenderSpeed;
    hwRender->common.GetRenderSpeed = AudioRenderGetRenderSpeed;
    hwRender->common.SetChannelMode = AudioRenderSetChannelMode;
    hwRender->common.GetChannelMode = AudioRenderGetChannelMode;
    hwRender->common.RegCallback = AudioRenderRegCallback;

    hwRender->started = false;
    hwRender->paused = false;
    hwRender->steroToMono = steroToMono;
    hwRender->chnId = chnId;
    MediaHalMutexAttr attr = { false };
    hwRender->mutex = MediaHalMutexCreate(&attr);
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterCreateRender(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
    const struct AudioSampleAttributes *attrs, struct AudioRender **render)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter is invalid");
    CHK_NULL_RETURN(desc, MEDIA_HAL_INVALID_PARAM, "input param desc null");
    CHK_NULL_RETURN(attrs, MEDIA_HAL_INVALID_PARAM, "input param attrs null");
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");

    int32_t ret;
    bool steroToMono = false;
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    uint32_t channelId = 0;
    ret = AudioAdapterGetFreeChn(adapter, false, &channelId);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioAdapterGetFreeChn failed");
        return MEDIA_HAL_NO_MEM;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)malloc(sizeof(*hwRender));
    if (hwRender == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc AudioHwRender failed");
        ret = MEDIA_HAL_NO_MEM;
        goto FREE_CHN;
    }
    if (memset_s(hwRender, sizeof(struct AudioHwRender), 0, sizeof(struct AudioHwRender)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }

    if (memcpy_s(&hwRender->deviceDesc, sizeof(struct AudioDeviceDescriptor),
        desc, sizeof(struct AudioDeviceDescriptor)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
    }

    if (memcpy_s(&hwRender->attrs, sizeof(struct AudioSampleAttributes),
        attrs, sizeof(struct AudioSampleAttributes)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
    }
    /* plguin support transform stero to mono  */
    if (hwRender->attrs.channelCount == CONFIG_CHANNEL_COUNT && hwAdapter->capability->channelCount == 1) {
        hwRender->attrs.channelCount = hwAdapter->capability->channelCount;
        steroToMono = true;
    }
    ret = HwRenderInit(hwAdapter, hwRender, channelId, steroToMono);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "InitHwRender failed");
        goto FREE_HWRENDER;
    }
    *render = &hwRender->common;
    return MEDIA_HAL_OK;

FREE_HWRENDER:
    free(hwRender);
FREE_CHN:
    (void)AudioAdapterFreeChn(adapter, channelId, false);
    return ret;
}

int32_t AudioAdapterDestroyRender(struct AudioAdapter *adapter, struct AudioRender *render)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    int32_t ret = AudioAdapterFreeChn(adapter, hwRender->chnId, false);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioAdapterFreeChn failed");
    }
    if (hwRender->started) {
        (void)AudioRenderStop(render);
    }

    ret = hwAdapter->portPlugin->DestroyTrack(hwAdapter->portHandle, (AudioHandle)hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin DestroyTrack failed: 0x%x", ret);
    }
    MediaHalMutexDestroy(&hwRender->mutex);
    free(hwRender);
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t HwCaptureInit(const struct AudioHwAdapter *hwAdapter,
    uint32_t chnId, struct AudioHwCapture *hwCapture)
{
    struct PortPluginAttr portAttr = {};
    portAttr.type = hwCapture->attrs.type;
    portAttr.sampleRate   = hwCapture->attrs.sampleRate;
    portAttr.channelCount = hwCapture->attrs.channelCount;
    portAttr.audioFormat  = hwCapture->attrs.format;

    int32_t ret = hwAdapter->portPlugin->CreateTrack(hwAdapter->portHandle, hwCapture->deviceDesc.pins,
        &portAttr, (AudioHandle *)&hwCapture->trackHandle);
    if (ret != MEDIA_HAL_OK || hwCapture->trackHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin CreateTrack failed: 0x%x", ret);
        return ret;
    }

    hwCapture->common.control.Start = AudioCaptureStart;
    hwCapture->common.control.Stop = AudioCaptureStop;
    hwCapture->common.control.Pause = AudioCapturePause;
    hwCapture->common.control.Resume = AudioCaptureResume;
    hwCapture->common.control.Flush = AudioCaptureFlush;
    hwCapture->common.attr.GetFrameSize = AudioCaptureGetFrameSize;
    hwCapture->common.attr.GetFrameCount = AudioCaptureGetFrameCount;
    hwCapture->common.attr.SetSampleAttributes = AudioCaptureSetSampleAttributes;
    hwCapture->common.attr.GetSampleAttributes = AudioCaptureGetSampleAttributes;
    hwCapture->common.attr.GetCurrentChannelId = AudioCaptureGetCurrentChannelId;
    hwCapture->common.attr.SetExtraParams = AudioCaptureSetExtraParams;
    hwCapture->common.attr.GetExtraParams = AudioCaptureGetExtraParams;
    hwCapture->common.scene.CheckSceneCapability = AudioCaptureCheckSceneCapability;
    hwCapture->common.scene.SelectScene = AudioCaptureSelectScene;
    hwCapture->common.volume.SetMute = AudioCaptureSetMute;
    hwCapture->common.volume.GetMute = AudioCaptureGetMute;
    hwCapture->common.volume.SetVolume = AudioCaptureSetVolume;
    hwCapture->common.volume.GetVolume = AudioCaptureGetVolume;
    hwCapture->common.volume.GetGainThreshold = AudioCaptureGetGainThreshold;
    hwCapture->common.volume.GetGain = AudioCaptureGetGain;
    hwCapture->common.volume.SetGain = AudioCaptureSetGain;
    hwCapture->common.CaptureFrame = AudioCaptureCaptureFrame;
    hwCapture->common.GetCapturePosition = AudioCaptureGetCapturePosition;
    hwCapture->chnId = chnId;
    MediaHalMutexAttr attr = { false };
    hwCapture->mutex = MediaHalMutexCreate(&attr);
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterCreateCapture(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
    const struct AudioSampleAttributes *attrs, struct AudioCapture **capture)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    CHK_NULL_RETURN(desc, MEDIA_HAL_INVALID_PARAM, "input param desc null");
    CHK_NULL_RETURN(attrs, MEDIA_HAL_INVALID_PARAM, "input param attrs null");
    CHK_NULL_RETURN(capture, MEDIA_HAL_INVALID_PARAM, "input param capture null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;

    uint32_t channelId = 0;
    int32_t ret = AudioAdapterGetFreeChn(adapter, true, &channelId);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioAdapterGetFreeChn failed");
        return MEDIA_HAL_NO_MEM;
    }
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)malloc(sizeof(*hwCapture));
    if (hwCapture == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc AudioHwCapture failed");
        ret = MEDIA_HAL_NO_MEM;
        goto FREE_CHN;
    }
    if (memset_s(hwCapture, sizeof(struct AudioHwCapture), 0, sizeof(struct AudioHwCapture)) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }
    if (memcpy_s(&hwCapture->deviceDesc, sizeof(struct AudioDeviceDescriptor),
        desc, sizeof(struct AudioDeviceDescriptor)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
    }
    if (memcpy_s(&hwCapture->attrs, sizeof(struct AudioSampleAttributes),
        attrs, sizeof(struct AudioSampleAttributes)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
    }
    ret = HwCaptureInit(hwAdapter, channelId, hwCapture);
    if (ret != MEDIA_HAL_OK || hwCapture->trackHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "InitHwCapture failed");
        goto FREE_HWCAPTURE;
    }
    *capture = &hwCapture->common;
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
FREE_HWCAPTURE:
    free(hwCapture);
FREE_CHN:
    (void)AudioAdapterFreeChn(adapter, channelId, true);
    return ret;
}

int32_t AudioAdapterDestroyCapture(struct AudioAdapter *adapter, struct AudioCapture *capture)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    CHK_NULL_RETURN(capture, MEDIA_HAL_INVALID_PARAM, "input param capture null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    int32_t ret = AudioAdapterFreeChn(adapter, hwCapture->chnId, true);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioAdapterFreeChn failed");
    }
    if (hwCapture->started) {
        (void)AudioCaptureStop(hwCapture);
    }
    ret = hwAdapter->portPlugin->DestroyTrack(hwAdapter->portHandle, hwCapture->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin DestroyTrack failed: 0x%x", ret);
    }
    MediaHalMutexDestroy(&hwCapture->mutex);
    free(hwCapture);
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterGetPortCapability(struct AudioAdapter *adapter, const struct AudioPort *port,
    struct AudioPortCapability *capability)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "input param port null");
    CHK_NULL_RETURN(port->portName, MEDIA_HAL_INVALID_PARAM, "input param portName null");
    CHK_NULL_RETURN(capability, MEDIA_HAL_INVALID_PARAM, "input param capability null");

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;

    bool validInput = false;
    for (uint32_t portIndex = 0; portIndex < hwAdapter->adapterDescriptor.portNum; portIndex++) {
        if (port->dir == hwAdapter->adapterDescriptor.ports[portIndex].dir &&
            strcmp(port->portName, hwAdapter->adapterDescriptor.ports[portIndex].portName) == 0) {
            *capability = hwAdapter->capability[portIndex];
            validInput = true;
            break;
        }
    }

    if (!validInput) {
        MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio port dir[%d], id[%d], name[%s]", port->dir, port->portId,
            port->portName);
        return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterSetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
    enum AudioPortPassthroughMode mode)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "input param port null");
    CHK_NULL_RETURN(port->portName, MEDIA_HAL_INVALID_PARAM, "input param portName null");

    if (port->dir != PORT_OUT || port->portId != 0 || strcmp(port->portName, "AOP") != 0) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    hwAdapter->mode = mode;
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterGetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
    enum AudioPortPassthroughMode *mode)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "input param port null");
    CHK_NULL_RETURN(port->portName, MEDIA_HAL_INVALID_PARAM, "input param portName null");
    CHK_NULL_RETURN(mode, MEDIA_HAL_INVALID_PARAM, "input param mode null");

    if (port->dir != PORT_OUT || port->portId != 0 || strcmp(port->portName, "AOP") != 0) {
        return MEDIA_HAL_INVALID_PARAM;
    }

    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    *mode = hwAdapter->mode;
    return MEDIA_HAL_OK;
}

static enum PluginInvokeId GetPluginInvokeIdbyInvokeName(const char *nvokeName)
{
    enum PluginInvokeId invokeId = PLUGIN_INVOKE_HAID_INVALID;
    size_t size = sizeof(g_pluginInvokeNameId) / sizeof(struct PluginInvokeNameId);
    for (size_t i = 0; i < size; i++) {
        if (strcmp(nvokeName, g_pluginInvokeNameId[i].funcName) == 0) {
            invokeId = g_pluginInvokeNameId[i].invokeId;
        }
    }
    return invokeId;
}

int32_t AudioAdapterSetExtraParams(struct AudioAdapter* adapter, enum AudioExtParamKey key,
    const char* condition, const char* value)
{
    MEDIA_HAL_UNUSED(key);
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    CHK_NULL_RETURN(hwAdapter->portPlugin, MEDIA_HAL_INVALID_PARAM, "Input param portPlugin is null");
    CHK_NULL_RETURN(condition, MEDIA_HAL_INVALID_PARAM, "Input param InvokeName is null");
    struct PluginInvokeAttr portAttr = {};
    portAttr.value = value;
    enum PluginInvokeId invokeId = GetPluginInvokeIdbyInvokeName(condition);
    int32_t ret = hwAdapter->portPlugin->Invoke(hwAdapter->portHandle, invokeId, &portAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin InvokeId = %d failed: 0x%x", invokeId, ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioAdapterGetExtraParams(struct AudioAdapter* adapter, enum AudioExtParamKey key,
    const char* condition, char* value, uint32_t* length)
{
    MEDIA_HAL_UNUSED(key);
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    CHK_NULL_RETURN(hwAdapter->portPlugin, MEDIA_HAL_INVALID_PARAM, "Input param portPlugin is null");
    CHK_NULL_RETURN(condition, MEDIA_HAL_INVALID_PARAM, "Input param InvokeName is null");
    struct PluginInvokeAttr portAttr = {};
    portAttr.value = value;
    portAttr.length = length;
    enum PluginInvokeId invokeId = GetPluginInvokeIdbyInvokeName(condition);
    int32_t ret = hwAdapter->portPlugin->Invoke(hwAdapter->portHandle, invokeId, &portAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portPlugin InvokeId = %d failed: 0x%x", invokeId, ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t AudioAdapterHaidEventCallBack(int32_t eventType, void *param, void *cookie)
{
    CHK_NULL_RETURN(g_haidAdapterCallBack, MEDIA_HAL_INVALID_PARAM, "adapter call back is null");
    char eventBuff[EVENT_TYPE_BUF_LEN] = { 0 };
    int32_t ret = sprintf_s(eventBuff, EVENT_TYPE_BUF_LEN, "%d", eventType);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sprintf_s event type failed");
        return MEDIA_HAL_ERR;
    }
    return g_haidAdapterCallBack(AUDIO_EXT_PARAM_HAID_MANAGER, NULL, eventBuff, param, cookie);
}

int32_t AudioAdapterRegExtraParamObserver(struct AudioAdapter *adapter, ParamCallback callback, void *cookie)
{
    CHK_FAILED_RETURN(AudioManagerIsValidAdapter(adapter), true, MEDIA_HAL_INVALID_PARAM, "adapter null");
    struct AudioHwAdapter* hwAdapter = (struct AudioHwAdapter*)adapter;
    CHK_NULL_RETURN(hwAdapter->portPlugin, MEDIA_HAL_INVALID_PARAM, "Input param portPlugin is null");
    int32_t ret = hwAdapter->portPlugin->RegCallBack(hwAdapter->portHandle, AudioAdapterHaidEventCallBack, cookie);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portplugin regcallback failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    g_haidAdapterCallBack = callback;
    return MEDIA_HAL_OK;
}