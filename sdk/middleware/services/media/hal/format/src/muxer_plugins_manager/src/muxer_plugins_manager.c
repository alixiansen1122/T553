/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: muxer plugins manager
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifdef ENABLE_MUXER

#include "muxer_plugins_manager.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "plugins_manager_common.h"
#include "media_hal_common.h"
#ifndef ENABLE_DL_DEMUXER_PLUGIN
#ifdef ENABLE_PLUGIN_MUXER_WAV
#include "wav_muxer.h"
#endif
#ifdef ENABLE_PLUGIN_MUXER_RAW
#include "raw_muxer.h"
#endif
#ifdef ENABLE_PLUGIN_MUXER_OGG
#include "ogg_muxer.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MODULE_NAME "MuxerPluginManager"

typedef struct {
    const MuxerPluginEntry *plugin;
    const ProtocolFun *protocol;
    const MuxerOutput *cfgParam;
    HalHandle muxerHdl;
} MuxerProbe;

typedef struct {
    bool inited;
    MediaHalMutexHandle mutex;
    int pluginsNum;
    PluginEntry *list;
} MuxerPluginsManager;

static MuxerPluginsManager g_muxerPluginsManager = {
    .inited = false,
    .mutex = NULL,
    .pluginsNum = 0,
    .list = NULL,
};

MuxerPluginEntry *g_allMuxerEntry[] = {
#if (defined(CONFIG_SUPPORT_FILESYSTEM) && defined(PRE_ASIC)) || defined(PRE_FPGA)
#ifdef ENABLE_PLUGIN_MUXER_WAV
    &g_wavMuxerEntry,
#endif
#ifdef ENABLE_PLUGIN_MUXER_RAW
    &g_rawMuxerEntry,
#endif
#ifdef ENABLE_PLUGIN_MUXER_OGG
    &g_oggMuxerEntry,
#endif
#endif
    NULL
};

#ifdef ENABLE_DL_MUXER_PLUGIN
static PluginEntry *GetMuxerPluginEntry(const char *name, int32_t index)
{
    MEDIA_HAL_UNUSED(index);
    if (name == NULL || strstr(name, "libplugin_muxer") == NULL) {
        return NULL;
    }
    void *module = MediaHalDLOpen(name);
    CHK_NULL_RETURN_NO_LOG(module, NULL);
    GetMuxerFun func = MediaHalDLSym(module, "GetMuxer");
    if (func == NULL) {
        MediaHalDLClose(module);
        return NULL;
    }
    const MuxerPluginEntry *entry = func();
    MEDIA_HAL_LOGI(MODULE_NAME, "add lib :%s!", entry->desc.libName);
    PluginEntry *node = PluginFillNode((const PluginDesc *)entry, module);
    if (node == NULL) {
        MediaHalDLClose(module);
    }
    return node;
}
#else
static PluginEntry *GetMuxerPluginEntry(const char *name, int32_t index)
{
    MEDIA_HAL_UNUSED(name);
    if ((uint32_t)index >= sizeof(g_allMuxerEntry) / sizeof(MuxerPluginEntry *)) {
        return NULL;
    }
    const MuxerPluginEntry *entry = g_allMuxerEntry[index];
    if (entry == NULL) {
        return NULL;
    }
    if (entry->desc.libName) {
        MEDIA_HAL_LOGI(MODULE_NAME, "add lib :%s!", entry->desc.libName);
    }
    PluginEntry *node = PluginFillNode((const PluginDesc *)entry, NULL);
    return node;
}
#endif

void MuxerPluginManagerInit(void)
{
    MuxerPluginsManager *manager = &g_muxerPluginsManager;
    if (manager->inited) {
        return;
    }
    bool dynamicLoad = false;
#ifdef ENABLE_DL_MUXER_PLUGIN
    dynamicLoad = true;
#endif
    PluginScanAllNodes(&manager->list, &manager->pluginsNum, dynamicLoad, GetMuxerPluginEntry);
    if (manager->list == NULL) {
        MEDIA_HAL_LOGW(MODULE_NAME, "Warnning!!! No muxer registered");
    }
    manager->inited = true;
    return;
}

static void DeleteMuxerPluginNode(PluginEntry *node)
{
    if (node != NULL) {
#ifdef ENABLE_DL_MUXER_PLUGIN
        MediaHalDLClose(node->module);
#endif
        node->module = NULL;
        free(node);
        node = NULL;
    }
}

void MuxerPluginManagerDeInit(void)
{
    MuxerPluginsManager *manager = &g_muxerPluginsManager;
    if (!manager->inited) {
        return;
    }
    PluginRemoveAllNodes(manager->list, DeleteMuxerPluginNode);
    manager->list = NULL;
    manager->pluginsNum = 0;
    manager->inited = false;
    return;
}

static const MuxerPluginEntry *FindNextMuxerPlugin(const MuxerPluginsManager *manager, const MuxerOutput *cfgParam,
    const MuxerPluginEntry *cur, const ProtocolFun *protocol)
{
    PluginEntry *p = manager->list;
    const MuxerPluginEntry *selectMuxer = NULL;
    while (p != NULL) {
        if (cur == NULL) {
            break;
        }
        if ((const MuxerPluginEntry *)p->plugin == cur) {
            p = (PluginEntry *)p->list.next;
            break;
        }
        p = (PluginEntry *)p->list.next;
    }
    while (p != NULL) {
        const MuxerPluginEntry *plugin = (const MuxerPluginEntry *)p->plugin;
        if (plugin != NULL && plugin->fmt_find != NULL) {
            if (plugin->fmt_find(cfgParam, protocol) == 0) {
                selectMuxer = plugin;
                break;
            }
        }
        p = (PluginEntry *)p->list.next;
    }
    return selectMuxer;
}

int32_t ProbeMuxerPlugins(const MuxerPluginsManager *manager, MuxerProbe *ctx, const ProtocolFun *protocol)
{
    int32_t ret = -1;
    const MuxerPluginEntry *entry = NULL;
    void *muxerHdl = NULL;
    entry = FindNextMuxerPlugin(manager, ctx->cfgParam, NULL, protocol);
    while (entry != NULL && ret != 0) {
        ret = entry->fmt_open((const MuxerOutput *)ctx->cfgParam, protocol, &muxerHdl);
        if (ret == 0) {
            ctx->plugin = entry;
            ctx->muxerHdl = muxerHdl;
            ctx->protocol = protocol;
            break;
        }
        ctx->muxerHdl = NULL;
        entry = FindNextMuxerPlugin(manager, ctx->cfgParam, entry, protocol);
    }
    return (ret == 0 && ctx->plugin != NULL) ? 0 : -1;
}

const MuxerPluginEntry *MuxerPluginManagerFindPlugin(const MuxerOutput *cfgParam, const ProtocolFun *protocol,
    HalHandle *muxerHdl)
{
    int32_t ret;
    MuxerProbe ctx;
    const MuxerPluginEntry *plugin = NULL;
    MuxerPluginsManager *manager = &g_muxerPluginsManager;
    MediaHalInitStaticMutexLock(&manager->mutex);
    CHK_NULL_RETURN(cfgParam, NULL, "cfgParam null");
    CHK_NULL_RETURN(muxerHdl, NULL, "muxerHdl null");
    MediaHalMutexLock(manager->mutex);
    if (!manager->inited) {
        MediaHalMutexUnLock(manager->mutex);
        return NULL;
    }
    ctx.plugin = NULL;
    ctx.protocol = NULL;
    ctx.muxerHdl = NULL;
    ctx.cfgParam = cfgParam;
    ret = ProbeMuxerPlugins(manager, &ctx, NULL);
    if (ret == 0 && ctx.plugin != NULL) {
        *muxerHdl = ctx.muxerHdl;
        MediaHalMutexUnLock(manager->mutex);
        return ctx.plugin;
    }
    if (protocol == NULL) {
        MediaHalMutexUnLock(manager->mutex);
        return NULL;
    }
    if (ProbeMuxerPlugins(manager, &ctx, protocol) == 0) {
        *muxerHdl = ctx.muxerHdl;
        plugin = ctx.plugin;
    }
    MediaHalMutexUnLock(manager->mutex);
    return plugin;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* ENABLE_MUXER */