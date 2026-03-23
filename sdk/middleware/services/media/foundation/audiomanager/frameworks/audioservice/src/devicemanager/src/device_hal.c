/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#include <stdlib.h>
#include <stdio.h>
#include "securec.h"
#include "device_config.h"
#include "audio_manager.h"
#include "audio_adapter.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "device_hal.h"

#define LOG_MODULE_NAME "AudioDeviceMgr"

#define MAX_AUDIO_FORMAT_NUM 20
#define MAX_AUDIO_ADAPTER_NUM 3
#define MAX_AUDIO_ADAPTER_PORT_NUM 2

typedef struct {
    struct AudioAdapter *audioAdapter;
    const struct AudioAdapterDescriptor *desc;
    AudioCapability capability;
} AudioAdapterDesc;

typedef struct {
    struct AudioManager *audioManager;
    uint32_t adapterNum;
    AudioAdapterDesc audioAdapterDesc[MAX_AUDIO_ADAPTER_NUM];
} AudioHalDesc;

static AudioHalDesc g_audioHalDesc;

typedef struct {
    int32_t frameworkStreamType;
    int32_t halStreamType;
} StreamTypeMap;

static StreamTypeMap g_mapStreamType[] = {
    { AUDIO_STREAM_MUSIC, AUDIO_IN_MEDIA },
    { AUDIO_STREAM_NOTIFICATION, AUDIO_IN_MEDIA },
    { AUDIO_STREAM_VOICE_ASSISTANT, AUDIO_IN_COMMUNICATION },
    { AUDIO_STREAM_VOICE_RECORD, AUDIO_IN_MEDIA },
    { AUDIO_STREAM_A2DP_MUSIC, AUDIO_IN_MEDIA },
    { AUDIO_STREAM_VOICE_CALL, AUDIO_IN_COMMUNICATION },
    { AUDIO_STREAM_VOICE_CALL_BT_SCO, AUDIO_IN_COMMUNICATION },
    { AUDIO_STREAM_VOICE_CALL_VOLTE, AUDIO_IN_COMMUNICATION },
    { AUDIO_STREAM_VOICE_CALL_VOLTE_SPI, AUDIO_IN_COMMUNICATION },
};

typedef struct {
    char *funcName;
    AudioHaidInvokeId invokeId;
} InvokeNameId;

static InvokeNameId g_deviceInvokeNameId[] = {
    {"HaidCreate", AUDIO_INVOKE_HAID_CREATE},
    {"HaidDestroy", AUDIO_INVOKE_HAID_DESTROY},
    {"HaidSetEnable", AUDIO_INVOKE_HAID_SET_ENABLE},
    {"HaidGetEnable", AUDIO_INVOKE_HAID_GET_ENABLE},
    {"HaidSetEventCallback", AUDIO_INVOKE_HAID_SET_EVENT_CALL_BACK},
    {"HaidSetVolume", AUDIO_INVOKE_HAID_SET_VOLUME},
    {"HaidGetVolume", AUDIO_INVOKE_HAID_GET_VOLUME},
    {"HaidSetHardwareConfig", AUDIO_INVOKE_HAID_SET_HARDWARE_CONFIG},
    {"HaidSetConfig", AUDIO_INVOKE_HAID_SET_CONFIG},
    {"HaidGetConfig", AUDIO_INVOKE_HAID_GET_CONFIG},
    {"HaidSetAllConfig", AUDIO_INVOKE_HAID_SET_ALL_CONFIG},
    {"HaidGetAllConfig", AUDIO_INVOKE_HAID_GET_ALL_CONFIG},
    {"HaidTonePlayForExamine", AUDIO_INVOKE_HAID_TONE_PLAY},
    {"HaidToneStopForExamine", AUDIO_INVOKE_HAID_TONE_STOP},
    {"HaidSetScene", AUDIO_INVOKE_HAID_SET_SCENE},
    {"HaidGetScene", AUDIO_INVOKE_HAID_GET_SCENE},
    {"TwsSetMode", AUDIO_INVOKE_TWS_SET_MODE},
    {"TwsSetRole", AUDIO_INVOKE_TWS_SET_ROLE},
    {"AncInit", AUDIO_INVOKE_ANC_INIT},
    {"AncDeinit", AUDIO_INVOKE_ANC_DEINIT},
    {"AncSetConfig", AUDIO_INVOKE_ANC_SET_CONFIG},
    {"AncGetConfig", AUDIO_INVOKE_ANC_GET_CONFIG},
};
static AudioDeviceHaidEventCallback g_deviceHaidCallBack = NULL;

static int32_t StreamTypeFromFrameworkToHal(int32_t stream)
{
    int32_t retStream = AUDIO_IN_OTHERS;
    for (uint32_t i = 0; i < sizeof(g_mapStreamType) / sizeof(g_mapStreamType[0]); i++) {
        if (g_mapStreamType[i].frameworkStreamType == stream) {
            return g_mapStreamType[i].halStreamType;
        }
    }
    return retStream;
}

static uint32_t ConvertChannelMaskFromHalToFramework(int32_t channelMasks)
{
    uint32_t retChannelMask;
    switch (channelMasks) {
        case AUDIO_CHANNEL_FRONT_LEFT:
            retChannelMask = AUDIO_CHANNEL_MODE_FRONT_LEFT;
            break;
        case AUDIO_CHANNEL_FRONT_RIGHT:
            retChannelMask = AUDIO_CHANNEL_MODE_FRONT_RIGHT;
            break;
        case AUDIO_CHANNEL_STEREO:
            retChannelMask = AUDIO_CHANNEL_MODE_STEREO;
            break;
        default:
            retChannelMask = AUDIO_CHANNEL_MODE_NONE;
            break;
    }
    return retChannelMask;
}

typedef struct {
    int32_t frameworkFormat;
    int32_t halFormat;
} FormatMap;

static FormatMap  g_mapFormat[] = {
    {AAC_LC,         AUDIO_FORMAT_AAC_LC},
    {AAC_LD,         AUDIO_FORMAT_AAC_LD},
    {AAC_ELD,        AUDIO_FORMAT_AAC_ELD},
    {AAC_HE_V1,      AUDIO_FORMAT_AAC_HE_V1},
    {AAC_HE_V2,      AUDIO_FORMAT_AAC_HE_V2},
    {MP3,            AUDIO_FORMAT_MP3},
    {mSBC,           AUDIO_FORMAT_mSBC},
    {OPUS,           AUDIO_FORMAT_OPUS},
    {VORBIS,         AUDIO_FORMAT_VORBIS},
    {FLAC,           AUDIO_FORMAT_FLAC},
    {APE,            AUDIO_FORMAT_APE},
    {SILK,           AUDIO_FORMAT_SILK},
    {SBC,            AUDIO_FORMAT_SBC},
    {AMR_WB,         AUDIO_FORMAT_AMR_WB}
};

static int32_t FormatFromHalToFramework(int32_t format)
{
    int32_t retFormat = AUDIO_DEFAULT;
    for (uint32_t i = 0; i < sizeof(g_mapFormat) / sizeof(g_mapFormat[0]); i++) {
        if (g_mapFormat[i].halFormat == format) {
            return g_mapFormat[i].frameworkFormat;
        }
    }
    return retFormat;
}

static uint32_t FormatFromFrameworkToHal(int32_t format, const uint32_t sampleFmt)
{
    if (format == PCM) {
        switch (sampleFmt) {
            case 0: // for bt sco cvsd codec format
                return AUDIO_FORMAT_PCM_16_BIT;
            case 1: // 1: sampleFmt
                return AUDIO_FORMAT_PCM_16_BIT;
            case 2: // 2: sampleFmt
                return AUDIO_FORMAT_PCM_24_BIT;
            default:
                break;
        }
    }

    uint32_t retFormat = AUDIO_FORMAT_PCM_16_BIT;
    for (uint32_t i = 0; i < sizeof(g_mapFormat) / sizeof(g_mapFormat[0]); i++) {
        if (g_mapFormat[i].frameworkFormat == format) {
            return (uint32_t)g_mapFormat[i].halFormat;
        }
    }
    ALOGE("FormatFromFrameworkToHal, format %d not match", format);
    return retFormat;
}

static uint32_t InDeviceFromFrameworkToHal(uint32_t device)
{
    uint32_t retDevice = PIN_NONE;
    if ((device & IN_BUILTIN_MIC) != IN_FLAG) {
        retDevice |= PIN_IN_MIC;
    }
    if ((device & IN_BLUETOOTH_SCO) != IN_FLAG) {
        retDevice |= PIN_IN_BT_SCO;
    }
    if ((device & IN_BLUETOOTH_A2DP) != IN_FLAG) {
        retDevice |= PIN_IN_BT_A2DP;
    }
    if ((device & IN_MODEM) != IN_FLAG) {
        retDevice |= PIN_IN_MODEM;
    }
    if ((device & IN_MODEM_HEADSET) != IN_FLAG) {
        retDevice |= PIN_IN_MODEM_HEADSET;
    }
    return retDevice;
}

static uint32_t OutDeviceFromFrameworkToHal(uint32_t device)
{
    uint32_t retDevice = PIN_NONE;
    if ((device & OUT_BUILTIN_SPEAKER) != 0) {
        retDevice |= PIN_OUT_SPEAKER;
    }
    if ((device & OUT_BLUETOOTH_SCO) != 0) {
        retDevice |= PIN_OUT_BT_SCO;
    }
    if ((device & OUT_BLUETOOTH_A2DP) != 0) {
        retDevice |= PIN_OUT_BT_A2DP;
    }
    if ((device & OUT_MODEM) != 0) {
        retDevice |= PIN_OUT_MODEM;
    }
    if ((device & OUT_MODEM_HEADSET) != 0) {
        retDevice |= PIN_OUT_MODEM_HEADSET;
    }
    return retDevice;
}

static uint32_t DeviceFromFrameworkToHal(uint32_t device)
{
    uint32_t retDevice = PIN_NONE;
    if ((device & IN_FLAG) != 0) {
        return InDeviceFromFrameworkToHal(device);
    } else {
        return OutDeviceFromFrameworkToHal(device);
    }
    return retDevice;
}

static char *GetAudioAdapterNameByDevice(uint32_t device)
{
    int32_t adapterConfigNum = 0;
    int32_t ret;
    AdapterDeviceMap *adapterList = NULL;
    ret = GetAdapterListFromConfig(&adapterList, &adapterConfigNum);
    if (ret != AUDIO_SUCCESS || adapterList == NULL) {
        return NULL;
    }
    for (int32_t i = 0; i < adapterConfigNum && i < MAX_ADAPTER_NUM; i++) {
        if (adapterList[i].isMatch == false) {
            continue;
        }
        for (uint32_t j = 0; j < adapterList[i].supportDeviceCount; j++) {
            if (((uint32_t)adapterList[i].supportDevice[j] & device) != 0 &&
                ((uint32_t)adapterList[i].supportDevice[j] & device) != IN_FLAG) {
                return adapterList[i].adapterName;
            }
        }
    }
    return NULL;
}
static struct AudioAdapter *GetAudioAdapterByDevice(uint32_t device)
{
    char *supportAdapterName = NULL;
    supportAdapterName = GetAudioAdapterNameByDevice(device);
    CHK_NULL_RETURN(supportAdapterName, NULL, "GetAudioAdapterNameByDevice failed, not match");
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (strcmp(supportAdapterName, g_audioHalDesc.audioAdapterDesc[i].desc->adapterName) == 0) {
            return g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        }
    }
    return NULL;
}

static int32_t FindAudioAdapterFromHal(void)
{
    int32_t adapterConfigNum = 0;
    int32_t count = 0;
    int32_t ret;
    AdapterDeviceMap *adapterList = NULL;
    ret = GetAdapterListFromConfig(&adapterList, &adapterConfigNum);
    if (ret != AUDIO_SUCCESS || adapterList == NULL) {
        return AUDIO_ERROR;
    }

    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        for (int32_t j = 0; j < adapterConfigNum; j++) {
            if (strcmp(g_audioHalDesc.audioAdapterDesc[i].desc->adapterName, adapterList[j].adapterName) == 0) {
                adapterList[j].isMatch = true;
                count++;
                break;
            }
        }
    }
    if (count == 0) {
        ALOGE("FindAudioAdapterFromHal failed, not match");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static AudioCodecFormat *ConvertFormatsFromHalToFramework(uint32_t formatNum, const enum AudioFormat *format)
{
    AudioCodecFormat *retFormat = NULL;
    if (formatNum > MAX_AUDIO_FORMAT_NUM) {
        ALOGE("ConvertFormatsFromHalToFramework formatNum:%u is error", formatNum);
        return NULL;
    }
    retFormat = (AudioCodecFormat *)malloc(formatNum * sizeof(AudioCodecFormat));
    CHK_NULL_RETURN(retFormat, NULL, "ConvertFormatsFromHalToFramework malloc failed");
    if (memset_s(retFormat, formatNum * sizeof(AudioCodecFormat), 0, formatNum * sizeof(AudioCodecFormat)) != EOK) {
        ALOGE("memset_s failed");
    }
    for (uint32_t i = 0; i < formatNum; i++) {
        retFormat[i] = (AudioCodecFormat)FormatFromHalToFramework(format[i]);
    }
    return retFormat;
}

static void LoadDevicePortCapability(uint32_t index, const struct AudioPortCapability *capability)
{
    AudioCapability *cap = &g_audioHalDesc.audioAdapterDesc[index].capability;
    cap->channelMasks = ConvertChannelMaskFromHalToFramework(capability->channelMasks);
    cap->bitWidth = 0;
    cap->channel = capability->channelCount;
    cap->sampleRateMasks = capability->sampleRateMasks;
    if (capability->formatNum <= MAX_AUDIO_FORMAT_NUM && capability->formatNum > 0) {
        cap->numFormat = capability->formatNum;
        cap->formats = ConvertFormatsFromHalToFramework(capability->formatNum, capability->formats);
    } else {
        cap->numFormat = 0;
        cap->formats = NULL;
        ALOGI("LoadDevicePortCapability formatNum:%d", capability->formatNum);
    }
}

static int32_t LoadAudioAdapter(const struct AudioAdapterDescriptor *descs, int32_t size)
{
    struct AudioManager *manager = NULL;
    struct AudioAdapter *adapter = NULL;
    uint32_t adapterIndex = 0;

    manager = g_audioHalDesc.audioManager;
    for (int32_t index = 0; index < size; index++) {
        const struct AudioAdapterDescriptor *desc = &descs[index];
        if (desc != NULL && desc->portNum > MAX_AUDIO_ADAPTER_PORT_NUM) {
            ALOGE("Adapter %s has more port, num is %d", desc->adapterName, desc->portNum);
            return AUDIO_ERROR;
        }
    }

    for (int32_t index = 0; index < size && adapterIndex < MAX_AUDIO_ADAPTER_NUM; index++) {
        const struct AudioAdapterDescriptor *desc = &descs[index];
        if (manager->LoadAdapter(manager, desc, &g_audioHalDesc.audioAdapterDesc[adapterIndex].audioAdapter) == 0) {
            adapter = g_audioHalDesc.audioAdapterDesc[adapterIndex].audioAdapter;
            if (adapter->InitAllPorts(adapter) != 0) {
                ALOGE("Adapter name:%s InitAllPorts failed", (desc == NULL) ? "unknow" : desc->adapterName);
                manager->UnloadAdapter(manager, adapter);
                g_audioHalDesc.audioAdapterDesc[adapterIndex].audioAdapter = NULL;
                return AUDIO_ERROR;
            }
            g_audioHalDesc.audioAdapterDesc[adapterIndex].desc = desc;
            adapterIndex++;
        }
    }
    if (adapterIndex == 0) {
        ALOGE("adapter load failed");
        return AUDIO_ERROR;
    }
    g_audioHalDesc.adapterNum = adapterIndex;
    return AUDIO_SUCCESS;
}

int32_t LoadAudioHal(void)
{
    int32_t ret;
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs = NULL;
    struct AudioManager *manager = NULL;
    if (memset_s(&g_audioHalDesc, sizeof(AudioHalDesc), 0, sizeof(AudioHalDesc)) != EOK) {
        ALOGE("memset_s failed");
    }
    g_audioHalDesc.audioManager = GetAudioManagerFuncs();
    CHK_NULL_RETURN(g_audioHalDesc.audioManager, AUDIO_ERROR, "audioManager is NULL");

    manager = g_audioHalDesc.audioManager;
    ret = manager->GetAllAdapters(manager, &descs, &size);
    if (ret != AUDIO_SUCCESS || size > MAX_AUDIO_ADAPTER_NUM) {
        ALOGE("GetAllAdapters return failed, ret:%d, size:%d", ret, size);
        return AUDIO_ERROR;
    }
    if (LoadAudioAdapter(descs, size) != AUDIO_SUCCESS) {
        ALOGE("LoadAudioAdapter failed");
        return AUDIO_ERROR;
    }
    if (FindAudioAdapterFromHal() != AUDIO_SUCCESS) {
        ALOGE("adapter not matach");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t UnLoadAudioHal(void)
{
    struct AudioManager *manager = g_audioHalDesc.audioManager;
    struct AudioAdapter *adapter = NULL;
    CHK_NULL_RETURN_NO_LOG(manager, AUDIO_SUCCESS);
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (g_audioHalDesc.audioAdapterDesc[i].capability.formats) {
            free(g_audioHalDesc.audioAdapterDesc[i].capability.formats);
            g_audioHalDesc.audioAdapterDesc[i].capability.formats = NULL;
        }
        adapter = g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        manager->UnloadAdapter(manager, adapter);
    }
    if (memset_s((void *)&g_audioHalDesc, sizeof(AudioHalDesc), 0, sizeof(AudioHalDesc)) != EOK) {
        ALOGE("memset_s failed");
    }
    return AUDIO_SUCCESS;
}

int32_t GetAdapterCapability(void)
{
    struct AudioPort *port = NULL;
    const struct AudioAdapterDescriptor *desc = NULL;
    struct AudioAdapter *adapter = NULL;
    struct AudioPortCapability capability;
    int32_t ret = 0;
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        desc = g_audioHalDesc.audioAdapterDesc[i].desc;
        adapter = g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        if (memset_s(&capability, sizeof(struct AudioPortCapability), 0, sizeof(struct AudioPortCapability)) != EOK) {
            ALOGE("memset_s failed");
        }
        for (uint32_t j = 0; j < desc->portNum; j++) {
            port = &(desc->ports[j]);
            ret = adapter->GetPortCapability(adapter, port, &capability);
            if (ret != 0) {
                continue;
            } else {
                break;
            }
        }
        LoadDevicePortCapability(i, &capability);
    }
    return AUDIO_SUCCESS;
}

AudioCapability *GetDeviceCapability(AudioDeviceType device)
{
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (adapter != NULL && adapter == g_audioHalDesc.audioAdapterDesc[i].audioAdapter) {
            return &g_audioHalDesc.audioAdapterDesc[i].capability;
        }
    }
    return NULL;
}

int32_t OpenInputStream(uint32_t device, AudioStreamConfig attr, Handle *inputStreamHandle)
{
    int32_t ret;
    if (g_audioHalDesc.adapterNum == 0 || inputStreamHandle == NULL) {
        return AUDIO_ERROR;
    }
    struct AudioSampleAttributes param;
    param.type = (enum AudioCategory)StreamTypeFromFrameworkToHal(attr.type);
    param.sampleRate = attr.sampleRate;
    param.format = FormatFromFrameworkToHal(attr.format, attr.sampleFmt);
    param.channelCount = attr.channelCount;
    param.interleaved = false;
    
    struct AudioCapture **capture = (struct AudioCapture **)inputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        struct AudioDeviceDescriptor deviceDesc = {};
        deviceDesc.portId = 0;
        deviceDesc.pins = DeviceFromFrameworkToHal(device);
        deviceDesc.desc = NULL;
        ret = adapter->CreateCapture(adapter, &deviceDesc, &param, capture);
        if (ret != 0 || capture == NULL) {
            ALOGE("openInputStream, ret:%d", ret);
            return AUDIO_ERROR;
        }
    } else {
        ALOGE("no adapter support device 0x%x", device);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t CloseInputStream(uint32_t device, Handle inputStreamHandle)
{
    if (g_audioHalDesc.adapterNum == 0) {
        return AUDIO_ERROR;
    }
    struct AudioCapture *capture = (struct AudioCapture *)inputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        adapter->DestroyCapture(adapter, capture);
    } else {
        ALOGE("no suitable adapter");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t OpenOutputStream(uint32_t device, AudioStreamConfig attr, Handle *outputStreamHandle)
{
    int32_t ret;
    if (g_audioHalDesc.adapterNum == 0 || outputStreamHandle == NULL) {
        return AUDIO_ERROR;
    }
    struct AudioSampleAttributes param;
    param.type = (enum AudioCategory)StreamTypeFromFrameworkToHal(attr.type);
    param.sampleRate = attr.sampleRate;
    param.format = FormatFromFrameworkToHal(attr.format, attr.sampleFmt);
    param.channelCount = attr.channelCount;
    param.interleaved = false;

    struct AudioRender **render = (struct AudioRender **)outputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        struct AudioDeviceDescriptor deviceDesc = {};
        deviceDesc.portId = 0;
        deviceDesc.pins = DeviceFromFrameworkToHal(device);
        deviceDesc.desc = NULL;
        ret = adapter->CreateRender(adapter, &deviceDesc, &param, render);
        if (ret != 0 || render == NULL) {
            ALOGE("openOutputStream, ret:%d", ret);
            return AUDIO_ERROR;
        }
    } else {
        ALOGE("no adapter support device 0x%x", device);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t CloseOutputStream(uint32_t device, Handle outputStreamHandle)
{
    if (g_audioHalDesc.adapterNum == 0) {
        return AUDIO_ERROR;
    }
    struct AudioRender *render = (struct AudioRender *)outputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        adapter->DestroyRender(adapter, render);
    } else {
        ALOGE("no suitable adapter");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static struct AudioAdapter *GetAudioPrimaryAdapter(void)
{
    char *supportAdapterName = "Primary";
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (strcmp(supportAdapterName, g_audioHalDesc.audioAdapterDesc[i].desc->adapterName) == 0) {
            return g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        }
    }
    return NULL;
}

static char *GetInvokeNameByHaidInvokeId(AudioHaidInvokeId audioHaidInvokeId)
{
    char *retValue = NULL;
    size_t size = sizeof(g_deviceInvokeNameId) / sizeof(InvokeNameId);
    for (size_t i = 0; i < size; i++) {
        if (g_deviceInvokeNameId[i].invokeId == audioHaidInvokeId) {
            retValue = g_deviceInvokeNameId[i].funcName;
            break;
        }
    }
    return retValue;
}

int32_t DeviceHalInvoke(AudioHaidInvokeId audioHaidInvokeId, char *param, uint32_t *paramlength)
{
    struct AudioAdapter *primaryAdapter = GetAudioPrimaryAdapter();
    CHK_NULL_RETURN(primaryAdapter, AUDIO_INVALID_PARAMS, "get adapter failed, not match");
    char *invokeName = GetInvokeNameByHaidInvokeId(audioHaidInvokeId);
    CHK_NULL_RETURN(invokeName, AUDIO_INVALID_PARAMS, "get invoke name failed, not found ");
    switch (audioHaidInvokeId) {
        case AUDIO_INVOKE_HAID_CREATE:
        case AUDIO_INVOKE_HAID_DESTROY:
        case AUDIO_INVOKE_HAID_SET_ENABLE:
        case AUDIO_INVOKE_HAID_SET_VOLUME:
        case AUDIO_INVOKE_HAID_TONE_PLAY:
        case AUDIO_INVOKE_HAID_SET_SCENE:
        case AUDIO_INVOKE_TWS_SET_MODE:
        case AUDIO_INVOKE_TWS_SET_ROLE:
        case AUDIO_INVOKE_ANC_INIT:
        case AUDIO_INVOKE_ANC_DEINIT:
        case AUDIO_INVOKE_HAID_TONE_STOP: {
            int32_t ret = primaryAdapter->SetExtraParams(primaryAdapter, AUDIO_EXT_PARAM_KEY_NONE,
                invokeName, param);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("set ExtraParams failed!, audioHaidInvokeId = %d, ret:%d", audioHaidInvokeId, ret);
                return ret;
            }
            break;
        }
        case AUDIO_INVOKE_ANC_SET_CONFIG:
        case AUDIO_INVOKE_ANC_GET_CONFIG:
        case AUDIO_INVOKE_HAID_GET_VOLUME:
        case AUDIO_INVOKE_HAID_SET_HARDWARE_CONFIG:
        case AUDIO_INVOKE_HAID_SET_CONFIG:
        case AUDIO_INVOKE_HAID_GET_CONFIG:
        case AUDIO_INVOKE_HAID_GET_SCENE:
        case AUDIO_INVOKE_HAID_SET_ALL_CONFIG:
        case AUDIO_INVOKE_HAID_GET_ALL_CONFIG:
        case AUDIO_INVOKE_HAID_GET_ENABLE: {
            int32_t ret = primaryAdapter->GetExtraParams(primaryAdapter, AUDIO_EXT_PARAM_KEY_NONE,
                invokeName, param, paramlength);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("get ExtraParams failed!, audioHaidInvokeId = %d, ret:%d", audioHaidInvokeId, ret);
                return ret;
            }
            break;
        }
        default:
            ALOGE("not support invokeId!, invokeId = %d", audioHaidInvokeId);
            return AUDIO_INVALID_PARAMS;
    }
    return AUDIO_SUCCESS;
}

static int32_t DeviceHaidEventCallback(enum AudioExtParamKey key, const char *condition, const char *value,
    void *reserved, void *cookie)
{
    AUDIO_UNUSED(condition);
    CHK_NULL_RETURN(g_deviceHaidCallBack, AUDIO_INVALID_PARAMS, "haid call back is null!");
    CHK_NULL_RETURN(value, AUDIO_INVALID_PARAMS, "value is null!");
    if (key != AUDIO_EXT_PARAM_HAID_MANAGER) {
        ALOGE("currently, only the callback function in hearing aid mode is supported.");
        return AUDIO_ERROR;
    }
    int32_t eventType = (int32_t)strtol(value, NULL, 10);
    return g_deviceHaidCallBack(eventType, reserved, cookie);
}

int32_t DeviceHalRegCallBack(AudioDeviceHaidEventCallback callBack, void *context)
{
    struct AudioAdapter* primaryAdapter = GetAudioPrimaryAdapter();
    CHK_NULL_RETURN(primaryAdapter, AUDIO_INVALID_PARAMS, "get adapter failed, not match");
    int32_t ret = primaryAdapter->RegExtraParamObserver(primaryAdapter, DeviceHaidEventCallback, context);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("reg ExtraParams call back failed!, ret:%d", ret);
        return ret;
    }
    g_deviceHaidCallBack = callBack;
    return AUDIO_SUCCESS;
}