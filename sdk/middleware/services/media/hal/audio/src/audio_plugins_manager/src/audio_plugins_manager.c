/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio plugins manager
* Author: Media Software Group
* Create: 2021-07-21
*/

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "media_hal_common.h"
#include "hal_list.h"
#include "media_hal_thread_adapt.h"
#include "audio_plugins_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MODULE_NAME "AudioPluginManager"

#ifdef ENABLE_DL_AUDIO_PLUGIN
#define AUDIO_PORT_PLUGIN_FUNCS "GetPortPluginFuncs"

#define PRIMARY_PORT_PLUGIN_LIB_PATH "/lib/libaudio_primary_port.so"
#define PRIMARY_PORT_PLUGIN_USR_LIB_PATH "/usr/lib/libaudio_primary_port.so"

#define BLUETOOTH_PORT_PLUGIN_LIB_PATH "/lib/libaudio_bluetooth_port.so"
#define BLUETOOTH_PORT_PLUGIN_USR_LIB_PATH "/usr/lib/libaudio_bluetooth_port.so"

#define MODEM_PORT_PLUGIN_LIB_PATH "/lib/libaudio_modem_port.so"
#define MODEM_PORT_PLUGIN_USR_LIB_PATH "/usr/lib/libaudio_modem_port.so"
#endif

typedef struct {
    AudioPluginType audioPluginType;
    uintptr_t pluginHandle;
#ifdef ENABLE_DL_AUDIO_PLUGIN
    void *pluginDlHandle;
#endif
} AudioPlugin;

typedef struct {
    bool inited;
    MediaHalMutexHandle mutex;
    List plugins;
} AudioPluginsManager;

static AudioPluginsManager g_audioPluginsManager = {
    .inited = false,
    .mutex = NULL,
};

static int32_t CheckPortPlugin(const AudioPortPlugin *plugin)
{
    CHK_NULL_RETURN(plugin->GetPortPluginCapability, MEDIA_HAL_INVALID_PARAM,
        "port plugin GetPortPluginCapability() is null");
    CHK_NULL_RETURN(plugin->Open, MEDIA_HAL_INVALID_PARAM, "port plugin Open() is null");
    CHK_NULL_RETURN(plugin->CreateTrack, MEDIA_HAL_INVALID_PARAM, "port plugin CreateTrack() is null");
    CHK_NULL_RETURN(plugin->DestroyTrack, MEDIA_HAL_INVALID_PARAM, "port plugin DestroyTrack() is null");
    CHK_NULL_RETURN(plugin->Close, MEDIA_HAL_INVALID_PARAM, "port plugin Close() is null");
    return MEDIA_HAL_OK;
}

static int32_t ScanAndLoadPrimaryPortPlugin(AudioPluginsManager *manager)
{
    AudioPlugin primaryPlugin = {};
    primaryPlugin.audioPluginType = AUDIO_PLUGIN_PRIMARY;
    const AudioPortPlugin *primaryPluginImpl = NULL;
#ifdef ENABLE_DL_AUDIO_PLUGIN
    primaryPlugin.pluginDlHandle = MediaHalDLOpen(PRIMARY_PORT_PLUGIN_USR_LIB_PATH);
    if (primaryPlugin.pluginDlHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found primary port plugin in path: %s", PRIMARY_PORT_PLUGIN_USR_LIB_PATH);
        primaryPlugin.pluginDlHandle = MediaHalDLOpen(PRIMARY_PORT_PLUGIN_LIB_PATH);
        if (primaryPlugin.pluginDlHandle == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "not found primary port plugin in path: %s", PRIMARY_PORT_PLUGIN_LIB_PATH);
            return MEDIA_HAL_ERR;
        }
    }
    GetPortPlugin getPluginFuns = (GetPortPlugin)MediaHalDLSym(primaryPlugin.pluginDlHandle, AUDIO_PORT_PLUGIN_FUNCS);
    if (getPluginFuns == NULL) {
        MediaHalDLClose(primaryPlugin.pluginDlHandle);
        MEDIA_HAL_LOGE(MODULE_NAME, "not found symbol: %s", AUDIO_PORT_PLUGIN_FUNCS);
        return MEDIA_HAL_ERR;
    }
    primaryPluginImpl = getPluginFuns(AUDIO_PLUGIN_NONE);
    if (primaryPluginImpl == NULL ||
        CheckPortPlugin(primaryPluginImpl) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found valid primary port plugin");
        MediaHalDLClose(primaryPlugin.pluginDlHandle);
        return MEDIA_HAL_ERR;
    }
#else
    primaryPluginImpl = GetPortPluginFuncs(AUDIO_PLUGIN_PRIMARY);
    if (primaryPluginImpl == NULL ||
        CheckPortPlugin(primaryPluginImpl) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found valid primary port plugin");
        return MEDIA_HAL_ERR;
    }
#endif
    primaryPlugin.pluginHandle = (uintptr_t)primaryPluginImpl;
    if (HalListPushFront(&manager->plugins, (uintptr_t)&primaryPlugin) != LIST_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary port plugin cannot be pushed into the list");
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t ScanAndLoadBluetoothPortPlugin(AudioPluginsManager *manager)
{
    AudioPlugin bluetoothPlugin = {};
    bluetoothPlugin.audioPluginType = AUDIO_PLUGIN_BLUETOOTH;
    const AudioPortPlugin *bluetoothPluginImpl = NULL;
#ifdef ENABLE_DL_AUDIO_PLUGIN
    bluetoothPlugin.pluginDlHandle = MediaHalDLOpen(BLUETOOTH_PORT_PLUGIN_USR_LIB_PATH);
    if (bluetoothPlugin.pluginDlHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found bluetooth port plugin in path: %s", BLUETOOTH_PORT_PLUGIN_USR_LIB_PATH);
        bluetoothPlugin.pluginDlHandle = MediaHalDLOpen(BLUETOOTH_PORT_PLUGIN_LIB_PATH);
        if (bluetoothPlugin.pluginDlHandle == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "not found bluetooth port plugin in path: %s", BLUETOOTH_PORT_PLUGIN_LIB_PATH);
            goto EXIT;
        }
    }
    GetPortPlugin getPluginFuns = (GetPortPlugin)MediaHalDLSym(bluetoothPlugin.pluginDlHandle,
        AUDIO_PORT_PLUGIN_FUNCS);
    if (getPluginFuns == NULL) {
        MediaHalDLClose(bluetoothPlugin.pluginDlHandle);
        MEDIA_HAL_LOGE(MODULE_NAME, "not found symbol: %s", AUDIO_PORT_PLUGIN_FUNCS);
        goto EXIT;
    }
    bluetoothPluginImpl = getPluginFuns(AUDIO_PLUGIN_NONE);
    if (bluetoothPluginImpl == NULL ||
        CheckPortPlugin(bluetoothPluginImpl) != MEDIA_HAL_OK) {
        MediaHalDLClose(bluetoothPlugin.pluginDlHandle);
        MEDIA_HAL_LOGE(MODULE_NAME, "not found valid bluetooth port plugin");
        goto EXIT;
    }
#else
    bluetoothPluginImpl = GetPortPluginFuncs(AUDIO_PLUGIN_BLUETOOTH);
    if (bluetoothPluginImpl == NULL ||
        CheckPortPlugin(bluetoothPluginImpl) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found valid bluetooth port plugin");
        goto EXIT;
    }
#endif
    bluetoothPlugin.pluginHandle = (uintptr_t)bluetoothPluginImpl;
    if (HalListPushFront(&manager->plugins, (uintptr_t)&bluetoothPlugin) != LIST_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth port plugin cannot be pushed into the list");
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
EXIT:
    return MEDIA_HAL_OK;
}

static int32_t ScanAndLoadModemPortPlugin(AudioPluginsManager *manager)
{
    AudioPlugin modemPlugin = {};
    modemPlugin.audioPluginType = AUDIO_PLUGIN_MODEM;
    const AudioPortPlugin *modemPluginImpl = NULL;
#ifdef ENABLE_DL_AUDIO_PLUGIN
    modemPlugin.pluginDlHandle = MediaHalDLOpen(MODEM_PORT_PLUGIN_USR_LIB_PATH);
    if (modemPlugin.pluginDlHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found modem port plugin in path: %s", MODEM_PORT_PLUGIN_USR_LIB_PATH);
        modemPlugin.pluginDlHandle = MediaHalDLOpen(MODEM_PORT_PLUGIN_LIB_PATH);
        if (modemPlugin.pluginDlHandle == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "not found modem port plugin in path: %s", MODEM_PORT_PLUGIN_LIB_PATH);
            goto EXIT;
        }
    }
    GetPortPlugin getPluginFuns = (GetPortPlugin)MediaHalDLSym(modemPlugin.pluginDlHandle,
        AUDIO_PORT_PLUGIN_FUNCS);
    if (getPluginFuns == NULL) {
        MediaHalDLClose(modemPlugin.pluginDlHandle);
        MEDIA_HAL_LOGE(MODULE_NAME, "not found symbol: %s", AUDIO_PORT_PLUGIN_FUNCS);
        goto EXIT;
    }
    modemPluginImpl = getPluginFuns(AUDIO_PLUGIN_NONE);
    if (modemPluginImpl == NULL ||
        CheckPortPlugin(modemPluginImpl) != MEDIA_HAL_OK) {
        MediaHalDLClose(modemPlugin.pluginDlHandle);
        MEDIA_HAL_LOGE(MODULE_NAME, "not found valid modem port plugin");
        goto EXIT;
    }
#else
    modemPluginImpl = GetPortPluginFuncs(AUDIO_PLUGIN_MODEM);
    if (modemPluginImpl == NULL ||
        CheckPortPlugin(modemPluginImpl) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found valid modem port plugin");
        goto EXIT;
    }
#endif
    modemPlugin.pluginHandle = (uintptr_t)modemPluginImpl;
    if (HalListPushFront(&manager->plugins, (uintptr_t)&modemPlugin) != LIST_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "modem port plugin cannot be pushed into the list");
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
EXIT:
    return MEDIA_HAL_OK;
}

static uintptr_t DupPlugin(const AudioPlugin *audioPlugin)
{
    AudioPortPlugin *plugin = NULL;
    plugin = (AudioPortPlugin *)malloc(sizeof(AudioPortPlugin));
    if (plugin == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return 0;
    }
    *plugin = *(AudioPortPlugin *)audioPlugin->pluginHandle;
    return (uintptr_t)plugin;
}

static uintptr_t PluginListDup(uintptr_t ptr)
{
    if (ptr == 0) {
        return 0;
    }
    AudioPlugin *audioPluginSrc = (AudioPlugin *)ptr;
    AudioPlugin *audioPluginDst = (AudioPlugin *)malloc(sizeof(AudioPlugin));
    if (audioPluginDst == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return 0;
    }
    if (memcpy_s(audioPluginDst, sizeof(AudioPlugin), audioPluginSrc,
                 sizeof(AudioPlugin)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s  failed");
        free(audioPluginDst);
        return 0;
    }
    audioPluginDst->pluginHandle = DupPlugin(audioPluginSrc);
    if (audioPluginDst->pluginHandle == 0) {
        free(audioPluginDst);
        return 0;
    }
    return (uintptr_t)audioPluginDst;
}

static void FreePlugin(AudioPlugin *audioPlugin)
{
    AudioPortPlugin *plugin = (AudioPortPlugin *)audioPlugin->pluginHandle;
    if (plugin != NULL) {
        free(plugin);
        audioPlugin->pluginHandle = 0;
    }
}

static void PluginListFree(uintptr_t ptr)
{
    if (ptr == 0) {
        return;
    }
    AudioPlugin *audioPlugin = (AudioPlugin *)ptr;
#ifdef ENABLE_DL_AUDIO_PLUGIN
    if (audioPlugin->pluginDlHandle != NULL) {
        MediaHalDLClose(audioPlugin->pluginDlHandle);
    }
#endif
    FreePlugin(audioPlugin);
    free(audioPlugin);
}

void AudioPluginManagerInit(void)
{
    MediaHalInitStaticMutexLock(&(g_audioPluginsManager.mutex));
    AudioPluginsManager *manager = &g_audioPluginsManager;
    MediaHalMutexLock(manager->mutex);
    if (manager->inited) {
        MediaHalMutexUnLock(manager->mutex);
        return;
    }
    DupFreeFuncPair dataFunc;
    dataFunc.dupFunc = PluginListDup;
    dataFunc.freeFunc = PluginListFree;
    HalListInit(&manager->plugins, &dataFunc);

    if (ScanAndLoadPrimaryPortPlugin(manager) != MEDIA_HAL_OK) {
        goto LOAD_FAILED;
    }
    if (ScanAndLoadBluetoothPortPlugin(manager) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "ScanAndLoadBluetoothPortPlugin failed");
    }
    if (ScanAndLoadModemPortPlugin(manager) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "ScanAndLoadModemPortPlugin failed");
    }
    manager->inited = true;
    MediaHalMutexUnLock(manager->mutex);
    return;
LOAD_FAILED:
    HalListDeinit(&manager->plugins);
    MediaHalMutexUnLock(manager->mutex);
    return;
}

const void *AudioPluginManagerGetPlugin(AudioPluginType pluginType)
{
    MediaHalInitStaticMutexLock(&(g_audioPluginsManager.mutex));
    AudioPluginsManager *manager = &g_audioPluginsManager;
    MediaHalMutexLock(manager->mutex);
    if (!manager->inited) {
        MediaHalMutexUnLock(manager->mutex);
        return NULL;
    }
    List *pluginListHead = &manager->plugins;
    MEDIA_HAL_LOGI(MODULE_NAME, "get audio plugin type: %d", pluginType);
    for (ListIterator it = HalListIterBegin(pluginListHead); it != HalListIterEnd(pluginListHead);
        it = HalListIterNext(pluginListHead, it)) {
        AudioPlugin *audioPlugin = (AudioPlugin *)HalListIterData(it);
        if (audioPlugin != NULL && audioPlugin->audioPluginType == pluginType) {
            MediaHalMutexUnLock(manager->mutex);
            return (const void *)audioPlugin->pluginHandle;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "get audio plugin failed: %d", pluginType);
    MediaHalMutexUnLock(manager->mutex);
    return NULL;
}

void AudioPluginManagerDeInit(void)
{
    MediaHalInitStaticMutexLock(&(g_audioPluginsManager.mutex));
    AudioPluginsManager *manager = &g_audioPluginsManager;
    MediaHalMutexLock(manager->mutex);
    if (!manager->inited) {
        MediaHalMutexUnLock(manager->mutex);
        return;
    }
    HalListDeinit(&manager->plugins);
    manager->inited = false;
    MediaHalMutexUnLock(manager->mutex);
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
