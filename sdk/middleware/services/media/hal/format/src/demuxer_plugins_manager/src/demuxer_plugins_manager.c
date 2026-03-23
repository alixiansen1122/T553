/*
 * Copyright (c) @CompanyNameMagicTag. 2016-2021. All rights reserved.
 * Description: demuxer plugins manager
 * Author: Media Software Group
 * Create: 2016-07-14
 */
#ifdef ENABLE_DEMUXER

#include "demuxer_plugins_manager.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "plugins_manager_common.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"
#ifndef ENABLE_DL_DEMUXER_PLUGIN
#include "hmf_demuxer_plugin.h"
#include "raw_format.h"
#endif
#ifdef ENABLE_FFMPEG_DEMUXER
#include "ff_demuxer_plugin.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MODULE_NAME "DemuxerPluginManager"
#define PLUGIN_PROBE_SCORE_FULL 100
#define PLUGIN_PROBE_SCORE_HIGH 90
#define PLUGIN_PROBE_SCORE_LOW 50
#define M3U8_LENGTH 6
typedef struct {
    const DemuxerPluginEntry *plugin;
    const ProtocolFun *protocol;
    int score;
    const DemuxerSource *source;
    HalHandle demuxerHdl;
} DemuxerProbe;

typedef struct {
    bool inited;
    MediaHalMutexHandle mutex;
    int pluginsNum;
    PluginEntry *list;
} DemuxerPluginsManager;

static DemuxerPluginsManager g_demuxerPluginsManager = {
    .inited = false,
    .mutex = NULL,
    .pluginsNum = 0,
    .list = NULL,
};

#ifndef ENABLE_DL_DEMUXER_PLUGIN
/*
    DemuxerPluginEntry g_ffmpegDemuxerEntry
    DemuxerPluginEntry g_hmfDemuxerEntry
    DemuxerPluginEntry g_rawDemuxerEntry
*/
DemuxerPluginEntry*  g_allDemuxerEntry[] = {
    &g_hmfDemuxerEntry,
    &g_rawFormatDemuxerEntry,
#ifdef ENABLE_FFMPEG_DEMUXER
    &g_ffDemuxerEntry,
#endif
    NULL
};

static PluginEntry *GetDemuxerPluginEntry(const char *name, int32_t index)
{
    MEDIA_HAL_UNUSED(name);
    if ((uint32_t)index >= sizeof(g_allDemuxerEntry) / sizeof(DemuxerPluginEntry*)) {
        return NULL;
    }
    const DemuxerPluginEntry *entry = g_allDemuxerEntry[index];
    if (entry == NULL) {
        return NULL;
    }
    if (entry->desc.libName) {
        MEDIA_HAL_LOGI(MODULE_NAME, "add lib :%s!", entry->desc.libName);
    }
    PluginEntry *node = PluginFillNode((const PluginDesc *)entry, NULL);
    return node;
}
#else
static PluginEntry *GetDemuxerPluginEntry(const char *name, int32_t index)
{
    MEDIA_HAL_UNUSED(index);
    if (name == NULL || strstr(name, "libplugin_demuxer") == NULL) {
        return NULL;
    }
    void *module = MediaHalDLOpen(name);
    CHK_NULL_RETURN_NO_LOG(module, NULL);
    GetDemuxerFun func = MediaHalDLSym(module, "GetDemuxer");
    if (func == NULL) {
        MediaHalDLClose(module);
        return NULL;
    }
    const DemuxerPluginEntry *entry = func();
    MEDIA_HAL_LOGI(MODULE_NAME, "add lib :%s!", entry->desc.libName);
    PluginEntry *node = PluginFillNode((const PluginDesc *)entry, module);
    if (node == NULL) {
        MediaHalDLClose(module);
    }
    return node;
}
#endif

static void DemuxerPluginRegister(PluginEntry **list, int32_t *nodeNum)
{
    bool dynamicLoad = false;
    GetPluginEntry getEntry = GetDemuxerPluginEntry;
#ifdef ENABLE_DL_DEMUXER_PLUGIN
    dynamicLoad = true;
#endif
    PluginScanAllNodes(list, nodeNum, dynamicLoad, getEntry);
}

static void DeleteDemuxerPluginNode(PluginEntry *node)
{
    if (node != NULL) {
#ifdef ENABLE_DL_DEMUXER_PLUGIN
        MediaHalDLClose(node->module);
#endif
        node->module = NULL;
        free(node);
    }
}

static void DemuxerPluginUnRegister(PluginEntry *list)
{
    PluginRemoveAllNodes(list, DeleteDemuxerPluginNode);
}

void DemuxerPluginManagerInit(void)
{
    DemuxerPluginsManager *manager = &g_demuxerPluginsManager;
    if (manager->inited) {
        return;
    }
    DemuxerPluginRegister(&manager->list, &manager->pluginsNum);
    if (manager->list == NULL) {
        MEDIA_HAL_LOGW(MODULE_NAME, "Warnning!!! No demuxer registered");
    }
    manager->inited = true;
    return;
}

void DemuxerPluginManagerDeInit(void)
{
    DemuxerPluginsManager *manager = &g_demuxerPluginsManager;
    if (!manager->inited) {
        return;
    }
    DemuxerPluginUnRegister(manager->list);
    manager->list = NULL;
    manager->pluginsNum = 0;
    manager->inited = false;
    return;
}

bool HasM3U8(const char *url)
{
    if (!url) return false;
    int len = strlen(url);
    if (len < M3U8_LENGTH) return false;
    return strstr(url, ".m3u8") != NULL;
}

static const DemuxerPluginEntry *FindNextDemuxerPlugin(const DemuxerPluginsManager *manager,
    const DemuxerSource *source, const DemuxerPluginEntry *cur, const ProtocolFun *protocol)
{
    PluginEntry *p = manager->list;
    const DemuxerPluginEntry *selectDemuxer = NULL;
    while (p != NULL) {
        if (cur == NULL) {
            break;
        }
        if ((const DemuxerPluginEntry *)p->plugin == cur) {
            p = (PluginEntry *)p->list.next;
            break;
        }
        p = (PluginEntry *)p->list.next;
    }
    while (p != NULL) {
        const DemuxerPluginEntry *plugin = (const DemuxerPluginEntry *)p->plugin;
        if (HasM3U8(source->url)) {
            if (strcmp(plugin->desc.name, "ffmpeg_demuxer") != 0) {
                p = (PluginEntry *)p->list.next;
                continue;
            }
        } else {
            if (strcmp(plugin->desc.name, "ffmpeg_demuxer") == 0) {
                p = (PluginEntry *)p->list.next;
                continue;
            }
        }
        if (plugin != NULL && plugin->fmt_find != NULL) {
            if (plugin->fmt_find(source, protocol) == 0) {
                selectDemuxer = plugin;
                break;
            }
        }
        p = (PluginEntry *)p->list.next;
    }
    return selectDemuxer;
}

static int32_t ProbeInDemuxerPlugin(const DemuxerProbe *ctx, const DemuxerPluginEntry *entry, HalHandle *demuxerHdl,
    int32_t *score)
{
    int32_t ret = 0;
    if (entry->fmt_open != NULL) {
        ret = entry->fmt_open(ctx->source, NULL, demuxerHdl, score);
        if (ret != 0) {
            *demuxerHdl = NULL;
            *score = 0;
            return ret;
        }
    }
    if (entry->fmt_invoke != NULL) {
        // call fmt_invoke function
    }
    if (entry->fmt_find_stream != NULL) {
        ret = (uint32_t)ret | (uint32_t)entry->fmt_find_stream(*demuxerHdl, NULL);
        if (ret != 0 && entry->fmt_invoke != NULL) {
            // call fmt_invoke function
        }
    }

    if (ret != 0) {
        if (entry->fmt_close != NULL) {
            (void)entry->fmt_close(*demuxerHdl);
        }
        *demuxerHdl = NULL;
        *score = 0;
    }
    return ret;
}

static int32_t ProbeDemuxerPlugins(const DemuxerPluginsManager *manager, DemuxerProbe *ctx,
    const ProtocolFun *protocol)
{
    int32_t ret;
    int32_t score;
    const DemuxerPluginEntry *entry = NULL;
    void *demuxerHdl = NULL;

    entry = FindNextDemuxerPlugin(manager, ctx->source, NULL, protocol);
    do {
        ret = -1;
        score = 0;
        if (entry == NULL) {
            break;
        }

        ret = ProbeInDemuxerPlugin(ctx, entry, &demuxerHdl, &score);
        if (ret != 0) {
            entry = FindNextDemuxerPlugin(manager, ctx->source, entry, protocol);
            continue;
        }
        if (score > ctx->score && demuxerHdl != NULL && entry != NULL) {
            ctx->score = score;
            if (ctx->plugin != NULL && ctx->demuxerHdl != NULL) {
                (void)ctx->plugin->fmt_close(ctx->demuxerHdl);
            }
            ctx->plugin = entry;
            ctx->protocol = protocol;
            ctx->demuxerHdl = demuxerHdl;
        }
        if (score == PLUGIN_PROBE_SCORE_FULL) {
            break;
        }
    } while (ret != 0 && entry != NULL);

    return ret;
}

static const DemuxerPluginEntry *AnalyseProbeResult(DemuxerProbe *ctx, HalHandle *demuxerHdl)
{
    if (ctx->score >= PLUGIN_PROBE_SCORE_LOW) {
        *demuxerHdl = ctx->demuxerHdl;
        return ctx->plugin;
    } else {
        if (ctx->plugin != NULL && ctx->demuxerHdl != NULL) {
            (void)ctx->plugin->fmt_close(ctx->demuxerHdl);
            ctx->demuxerHdl = NULL;
        }
        return NULL;
    }
}

const DemuxerPluginEntry *DemuxerPluginManagerFindPlugin(const DemuxerSource *source, const ProtocolFun *protocol,
    HalHandle *demuxerHdl)
{
    int32_t ret;
    DemuxerProbe ctx;
    const DemuxerPluginEntry *plugin = NULL;
    DemuxerPluginsManager *manager = &g_demuxerPluginsManager;
    if (source == NULL || demuxerHdl == NULL) {
        return NULL;
    }

    MediaHalInitStaticMutexLock(&manager->mutex);
    MediaHalMutexLock(manager->mutex);
    if (!manager->inited) {
        MediaHalMutexUnLock(manager->mutex);
        return NULL;
    }
    ctx.plugin = NULL;
    ctx.protocol = NULL;
    ctx.score = 0;
    ctx.demuxerHdl = NULL;
    ctx.source = source;
    ret = ProbeDemuxerPlugins(manager, &ctx, NULL);
    if (ret == 0 && ctx.score >= PLUGIN_PROBE_SCORE_HIGH) {
        *demuxerHdl = ctx.demuxerHdl;
        MediaHalMutexUnLock(manager->mutex);
        return ctx.plugin;
    }

    if (protocol == NULL) {
        plugin = AnalyseProbeResult(&ctx, demuxerHdl);
        MediaHalMutexUnLock(manager->mutex);
        return plugin;
    }

    (void)ProbeDemuxerPlugins(manager, &ctx, protocol);
    plugin = AnalyseProbeResult(&ctx, demuxerHdl);
    MediaHalMutexUnLock(manager->mutex);
    return plugin;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* ENABLE_DEMUXER */
