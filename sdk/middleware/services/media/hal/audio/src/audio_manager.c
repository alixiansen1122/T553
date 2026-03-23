/*
* Copyright (c) @CompanyNameMagicTag. 2020-2020. All rights reserved.
* Description:Audio Manager
* Author: Media Software Group
* Create: 2020-08-11
*/
#include "audio_manager.h"
#include <stdlib.h>
#include <errno.h>
#include "securec.h"
#include "media_hal_common.h"
#include "audio_internal.h"
#include "audio_plugins_manager.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AUDIO_ADAPTER_DESC_INIT { .adapterName = NULL, .portNum = 0, .ports = NULL }
#define AUDIO_ADAPTER_INIT { .count = 0, .loaded = false, .hwAdapter = {}, .adapterLock = NULL }

struct AudioHwAdapterInternal {
    uint32_t count;
    bool loaded;
    struct AudioHwAdapter hwAdapter;
    MediaHalMutexHandle adapterLock;
};

static struct AudioPort g_audioPrimaryPort[] = {
    { PORT_IN, 0, "PrimaryPortIn"},
    { PORT_OUT, 1, "PrimaryPortOut" }
};

static struct AudioPort g_audioBluetoothPort[] = {
    { PORT_OUT_IN, 0, "BluetoothPort" }
};

static struct AudioPort g_audioModemPort[] = {
    { PORT_OUT_IN, 0, "ModemPort" }
};

/* primary/bluetooth/modem adapter */
static struct AudioAdapterDescriptor g_audioAdapterDescs[] = {
    AUDIO_ADAPTER_DESC_INIT,
    AUDIO_ADAPTER_DESC_INIT,
    AUDIO_ADAPTER_DESC_INIT,
};

static struct AudioHwAdapterInternal g_audioAdapters[] = {
    AUDIO_ADAPTER_INIT,
    AUDIO_ADAPTER_INIT,
    AUDIO_ADAPTER_INIT,
};

static bool g_allAdapterLoaded = false;
static MediaHalMutexHandle g_adapterMutex = NULL;

static uint32_t GetAdapterCount(void)
{
    uint32_t adapterCnt = 0;
    for (uint32_t adapterIndex = 0; adapterIndex < ARRAY_SIZE(g_audioAdapterDescs); adapterIndex++) {
        if (g_audioAdapterDescs[adapterIndex].adapterName != NULL) {
            adapterCnt++;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "adapter count: %d", adapterCnt);
    return adapterCnt;
}

static void InitAudioAdaptMutexLock(void)
{
    uint32_t adapterNum = (uint32_t)ARRAY_SIZE(g_audioAdapters);
    for (uint32_t i = 0; i < adapterNum; i++) {
        MediaHalInitStaticMutexLock(&(g_audioAdapters[i].adapterLock));
    }
}

bool AudioManagerIsValidAdapter(const struct AudioAdapter *adapter)
{
    InitAudioAdaptMutexLock();
    uint32_t adapterNum = (uint32_t)ARRAY_SIZE(g_audioAdapters);
    CHK_NULL_RETURN(adapter, false, "input param adapter is null");
    for (uint32_t adapterIndex = 0; adapterIndex < adapterNum; adapterIndex++) {
        MediaHalMutexLock(g_audioAdapters[adapterIndex].adapterLock);
        if ((&g_audioAdapters[adapterIndex].hwAdapter.common) == adapter) {
            MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
            return true;
        }
        MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
    }
    return false;
}

static uint32_t InitAudioAdapt(void)
{
    uint32_t adapterIndex = 0;
    /* init primary adapter port plugin */
    const AudioPortPlugin *primaryPlugin = AudioPluginManagerGetPlugin(AUDIO_PLUGIN_PRIMARY);
    if (primaryPlugin != NULL) {
        g_audioAdapters[adapterIndex].hwAdapter.portPlugin = primaryPlugin;
        g_audioAdapters[adapterIndex].hwAdapter.portDirection = primaryPlugin->audioPortDir;
        g_audioAdapterDescs[adapterIndex].adapterName = "Primary";
        g_audioAdapterDescs[adapterIndex].portNum = (uint32_t)ARRAY_SIZE(g_audioPrimaryPort);
        g_audioAdapterDescs[adapterIndex].ports = g_audioPrimaryPort;
        adapterIndex++;
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPluginManagerGetPlugin(Primary) failed");
    }

    /* init bluetooth adapter port plugin */
    const AudioPortPlugin *bluetoothPlugin = AudioPluginManagerGetPlugin(AUDIO_PLUGIN_BLUETOOTH);
    if (bluetoothPlugin != NULL) {
        g_audioAdapters[adapterIndex].hwAdapter.portPlugin = bluetoothPlugin;
        g_audioAdapters[adapterIndex].hwAdapter.portDirection = bluetoothPlugin->audioPortDir;
        g_audioAdapterDescs[adapterIndex].adapterName = "Bluetooth";
        g_audioAdapterDescs[adapterIndex].portNum = (uint32_t)ARRAY_SIZE(g_audioBluetoothPort);
        g_audioAdapterDescs[adapterIndex].ports = g_audioBluetoothPort;
        adapterIndex++;
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPluginManagerGetPlugin(Bluetooth) failed");
    }

    /* init modem adapter port plugin */
    const AudioPortPlugin *modemPlugin = AudioPluginManagerGetPlugin(AUDIO_PLUGIN_MODEM);
    if (modemPlugin != NULL) {
        g_audioAdapters[adapterIndex].hwAdapter.portPlugin = modemPlugin;
        g_audioAdapters[adapterIndex].hwAdapter.portDirection = modemPlugin->audioPortDir;
        g_audioAdapterDescs[adapterIndex].adapterName = "Modem";
        g_audioAdapterDescs[adapterIndex].portNum = (uint32_t)ARRAY_SIZE(g_audioModemPort);
        g_audioAdapterDescs[adapterIndex].ports = g_audioModemPort;
        adapterIndex++;
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPluginManagerGetPlugin(Modem) failed");
    }

    return adapterIndex;
}

static int32_t AudioManagerGetAllAdapters(struct AudioManager *manager, struct AudioAdapterDescriptor **descs,
    int32_t *size)
{
    CHK_NULL_RETURN(manager, MEDIA_HAL_INVALID_PARAM, "Input param manager is null");
    CHK_NULL_RETURN(descs, MEDIA_HAL_INVALID_PARAM, "Input param descs is null");
    CHK_NULL_RETURN(size, MEDIA_HAL_INVALID_PARAM, "Input param size is null");

    InitAudioAdaptMutexLock();
    MediaHalInitStaticMutexLock(&g_adapterMutex);
    MediaHalMutexLock(g_adapterMutex);
    if (g_allAdapterLoaded) {
        *descs = g_audioAdapterDescs;
        *size = (int32_t)GetAdapterCount();
        MediaHalMutexUnLock(g_adapterMutex);
        return MEDIA_HAL_OK;
    }

#ifdef AUDIO_HAL_ENABLE_KPI
    int32_t ret = SetMediaHalLogEnabledLevel(MEDIA_HAL_LOG_LEVEL_ERR);
    CHK_FAILED_NO_RETURN(ret, 0, "SetMediaHalLogEnabledLevel failed");
#endif

    AudioPluginManagerInit();
    if (InitAudioAdapt() == 0) {
        MediaHalMutexUnLock(g_adapterMutex);
        return MEDIA_HAL_ERR;
    } else {
        *descs = g_audioAdapterDescs;
        *size = (int32_t)ARRAY_SIZE(g_audioAdapters);
        g_allAdapterLoaded = true;
        MediaHalMutexUnLock(g_adapterMutex);
        return MEDIA_HAL_OK;
    }
}

static int32_t HwAdapterInit(const struct AudioAdapterDescriptor *desc, struct AudioHwAdapter *hwAdapter)
{
    if (memcpy_s(&(hwAdapter->adapterDescriptor),
        sizeof(struct AudioAdapterDescriptor), desc, sizeof(struct AudioAdapterDescriptor)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "adapter descriptor port num: %d", hwAdapter->adapterDescriptor.portNum);

    hwAdapter->capability = (struct AudioPortCapability *)malloc(sizeof(struct AudioPortCapability) *
        hwAdapter->adapterDescriptor.portNum);
    CHK_NULL_RETURN(hwAdapter->capability, MEDIA_HAL_NO_MEM, "malloc failed");
    if (memset_s(hwAdapter->capability, sizeof(struct AudioPortCapability) * hwAdapter->adapterDescriptor.portNum, 0,
        sizeof(struct AudioPortCapability) * hwAdapter->adapterDescriptor.portNum) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        return MEDIA_HAL_ERR;
    }
    hwAdapter->common.InitAllPorts = AudioAdapterInitAllPorts;
    hwAdapter->common.CreateRender = AudioAdapterCreateRender;
    hwAdapter->common.DestroyRender = AudioAdapterDestroyRender;
    hwAdapter->common.CreateCapture = AudioAdapterCreateCapture;
    hwAdapter->common.DestroyCapture = AudioAdapterDestroyCapture;
    hwAdapter->common.GetPortCapability = AudioAdapterGetPortCapability;
    hwAdapter->common.SetPassthroughMode = AudioAdapterSetPassthroughMode;
    hwAdapter->common.GetPassthroughMode = AudioAdapterGetPassthroughMode;
    hwAdapter->common.SetExtraParams = AudioAdapterSetExtraParams;
    hwAdapter->common.GetExtraParams = AudioAdapterGetExtraParams;
    hwAdapter->common.RegExtraParamObserver = AudioAdapterRegExtraParamObserver;
    MediaHalMutexAttr attr = { false };
    hwAdapter->mutex = MediaHalMutexCreate(&attr);
    return MEDIA_HAL_OK;
}

static int32_t AudioManagerLoadAdapter(struct AudioManager *manager, const struct AudioAdapterDescriptor *desc,
    struct AudioAdapter **adapter)
{
    CHK_NULL_RETURN(manager, MEDIA_HAL_INVALID_PARAM, "Input param manager is null");
    CHK_NULL_RETURN(desc, MEDIA_HAL_INVALID_PARAM, "Input param desc is null");
    CHK_NULL_RETURN(desc->adapterName, MEDIA_HAL_INVALID_PARAM, "Input param adapterName is null");
    CHK_NULL_RETURN(desc->ports, MEDIA_HAL_INVALID_PARAM, "Input param ports is null");
    CHK_NULL_RETURN(adapter, MEDIA_HAL_INVALID_PARAM, "Input param adapter is null");

    InitAudioAdaptMutexLock();
    MEDIA_HAL_LOGD(MODULE_NAME, "adapter name: %s", desc->adapterName);
    uint32_t adapterNum = GetAdapterCount();
    uint32_t adapterIndex;
    for (adapterIndex = 0; adapterIndex < adapterNum; adapterIndex++) {
        if (strcmp(desc->adapterName, g_audioAdapterDescs[adapterIndex].adapterName) == 0 &&
            desc->portNum == g_audioAdapterDescs[adapterIndex].portNum) {
            MEDIA_HAL_LOGD(MODULE_NAME, "supported this adapter: %s", desc->adapterName);
            break;
        }
    }
    if (adapterIndex >= adapterNum) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not supported this adapter: %s", desc->adapterName);
        return MEDIA_HAL_INVALID_PARAM;
    }

    MediaHalMutexLock(g_audioAdapters[adapterIndex].adapterLock);
    if (g_audioAdapters[adapterIndex].loaded) {
        *adapter = &(g_audioAdapters[adapterIndex].hwAdapter.common);
        g_audioAdapters[adapterIndex].count++;
        MEDIA_HAL_LOGI(MODULE_NAME, "adapter %s has loaded count: %d",
            desc->adapterName, g_audioAdapters[adapterIndex].count);
        MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
        return MEDIA_HAL_OK;
    }

    struct AudioHwAdapter *hwAdapter = &(g_audioAdapters[adapterIndex].hwAdapter);
    int32_t ret = HwAdapterInit(desc, hwAdapter);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "HwAdapterInit failed:%d", ret);
        return ret;
    }
    *adapter = &(hwAdapter->common);
    g_audioAdapters[adapterIndex].loaded = true;
    g_audioAdapters[adapterIndex].count++;
    MEDIA_HAL_LOGI(MODULE_NAME, "adapter %s load count: %d",
        desc->adapterName, g_audioAdapters[adapterIndex].count);
    MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
    return MEDIA_HAL_OK;
}

static void AudioManagerUnloadAdapter(struct AudioManager *manager, struct AudioAdapter *adapter)
{
    CHK_NULL_RETURN_NONE(manager, "input param manager null");
    CHK_NULL_RETURN_NONE(adapter, "input param adapter null");

    InitAudioAdaptMutexLock();
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    uint32_t adapterNum = (uint32_t)ARRAY_SIZE(g_audioAdapters);
    uint32_t adapterIndex;
    struct AudioAdapterDescriptor *adapterDescriptor = NULL;
    for (adapterIndex = 0; adapterIndex < adapterNum; adapterIndex++) {
        if (&(g_audioAdapters[adapterIndex].hwAdapter) == hwAdapter) {
            adapterDescriptor = &(g_audioAdapters[adapterIndex].hwAdapter.adapterDescriptor);
            MEDIA_HAL_LOGD(MODULE_NAME, "found this adapter");
            break;
        }
    }
    if (adapterIndex >= adapterNum) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found this adapter");
        return;
    }
    CHK_NULL_RETURN_NONE(adapterDescriptor, "adapterDescriptor is null");
    MediaHalMutexLock(g_audioAdapters[adapterIndex].adapterLock);
    if (!(g_audioAdapters[adapterIndex].loaded)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adapter have unloaded");
        MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
        return;
    }

    g_audioAdapters[adapterIndex].count--;
    MEDIA_HAL_LOGD(MODULE_NAME, "adapter %s unload count: %d",
        hwAdapter->adapterDescriptor.adapterName, g_audioAdapters[adapterIndex].count);
    if (g_audioAdapters[adapterIndex].count > 0) {
        MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
        return;
    }

    (void)AudioAdapterClosePort(adapter);

    if (hwAdapter->capability != NULL) {
        free(hwAdapter->capability);
        hwAdapter->capability = NULL;
    }
    MediaHalMutexDestroy(&hwAdapter->mutex);
    g_audioAdapters[adapterIndex].loaded = false;
    MediaHalMutexUnLock(g_audioAdapters[adapterIndex].adapterLock);
    return;
}

static struct AudioManager g_audioManagerFuncs = {
    .GetAllAdapters = AudioManagerGetAllAdapters,
    .LoadAdapter = AudioManagerLoadAdapter,
    .UnloadAdapter = AudioManagerUnloadAdapter,
};

struct AudioManager *GetAudioManagerFuncs(void)
{
    return &g_audioManagerFuncs;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
