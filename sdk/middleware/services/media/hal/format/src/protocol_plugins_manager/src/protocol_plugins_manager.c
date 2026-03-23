/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: protocol plugins manager
 * Author: Media Software Group
 * Create: 2017-12-22
 */

#ifdef ENABLE_PROTOCOL_PLUGIN

#include "protocol_plugins_manager.h"
#include <sys/time.h>
#include <sys/types.h>
#ifdef SUPPORT_DL
#include <dlfcn.h>
#include <dirent.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "plugins_manager_common.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define BASE_PATH "usr/lib/"
#define DNAME_TYPE_FILE 8
#define PATH_LEN 1024
#define MODULE_NAME "ProtocolManager"

typedef struct {
    bool inited;
    MediaHalMutexHandle mutex;
    int pluginsNum;
    PluginEntry *list;
} ProtocolPluginsManager;

ProtocolPluginsManager g_protocolPluginsManager = {
    .inited = false,
    .mutex = NULL,
    .pluginsNum = 0,
    .list = NULL,
};

#ifdef ENABLE_DL_PROTOCOL_PLUGIN
static PluginEntry *GetProtocolPluginEntry(const char *name, int32_t index)
{
    MEDIA_HAL_UNUSED(index);
    if (name == NULL || strstr(name, "libplugin_protocol") == NULL) {
        return NULL;
    }
    void *module = MediaHalDLOpen(name);
    CHK_NULL_RETURN_NO_LOG(module, NULL);
    GetProtocolFun func = MediaHalDLSym(module, "GetProtocol");
    if (func == NULL) {
        MediaHalDLClose(module);
        return NULL;
    }
    const ProtocolPluginEntry *entry = func();
    MEDIA_HAL_LOGI(MODULE_NAME, "add lib :%s!", entry->desc.libName);
    PluginEntry *node = PluginFillNode((const PluginDesc *)entry, module);
    if (node == NULL) {
        MediaHalDLClose(module);
    }
    return node;
}
#else
static PluginEntry *GetProtocolPluginEntry(const char *name, int32_t index)
{
    MEDIA_HAL_UNUSED(name);
    MEDIA_HAL_UNUSED(index);
    MEDIA_HAL_LOGW(MODULE_NAME, "not support get static protocol plugin entry !");
    return NULL;
}
#endif

void ProtocolPluginManagerInit(void)
{
    ProtocolPluginsManager *manager = &g_protocolPluginsManager;
    if (manager->inited) {
        return;
    }
    bool dynamicLoad = false;
#ifdef ENABLE_DL_PROTOCOL_PLUGIN
    dynamicLoad = true;
#endif
    PluginScanAllNodes(&manager->list, &manager->pluginsNum, dynamicLoad, GetProtocolPluginEntry);
    if (manager->list == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "Warnning!!! No protocol registered");
    }
    manager->inited = true;
    return;
}

static void DeleteProtocolPluginNode(PluginEntry *node)
{
    if (node != NULL) {
#ifdef ENABLE_DL_PROTOCOL_PLUGIN
        MediaHalDLClose(node->module);
#endif
        node->module = NULL;
        free(node);
    }
}

void ProtocolPluginManagerDeInit(void)
{
    ProtocolPluginsManager *manager = &g_protocolPluginsManager;
    if (!manager->inited) {
        return;
    }
    PluginRemoveAllNodes(manager->list, DeleteProtocolPluginNode);
    manager->list = NULL;
    manager->pluginsNum = 0;
    manager->inited = false;
    return;
}

int32_t ProtocolPluginManagerFindPlugin(const char *url, int32_t len, ProtocolFun *func)
{
    MEDIA_HAL_UNUSED(len);
    int32_t ret;
    int32_t protocolMerit = 0;
    ProtocolPluginsManager *manager = &g_protocolPluginsManager;
    PluginEntry *p = manager->list;
    const ProtocolPluginEntry *selectProtocol = NULL;
    CHK_NULL_RETURN(func, -1, "func NULL");
    CHK_NULL_RETURN(url, -1, "url NULL");
    MediaHalInitStaticMutexLock(&manager->mutex);
    MediaHalMutexLock(manager->mutex);
    if (!manager->inited) {
        MediaHalMutexUnLock(manager->mutex);
        return -1;
    }
    while (p != NULL) {
        ProtocolPluginEntry *plugin = (ProtocolPluginEntry *)p->plugin;
        if (plugin != NULL && plugin->protocol_find != NULL) {
            ret = plugin->protocol_find(url);
            if ((ret == 0) && (plugin->desc.priority > protocolMerit)) {
                selectProtocol = (const ProtocolPluginEntry *)p->plugin;
                protocolMerit = plugin->desc.priority;
            }
        }
        p = (PluginEntry *)p->list.next;
    }
    if (selectProtocol != NULL) {
        func->protocol_find = selectProtocol->protocol_find;
        func->protocol_open = selectProtocol->protocol_open;
        func->protocol_seek = selectProtocol->protocol_seek;
        func->protocol_seek_stream = selectProtocol->protocol_seek_stream;
        func->protocol_read = selectProtocol->protocol_read;
        func->protocol_write = selectProtocol->protocol_write;
        func->protocol_invoke = selectProtocol->protocol_invoke;
        func->protocol_close = selectProtocol->protocol_close;
        ret = 0;
    } else {
        ret = -1;
    }
    MediaHalMutexUnLock(manager->mutex);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* ENABLE_PROTOCOL_PLUGIN */
