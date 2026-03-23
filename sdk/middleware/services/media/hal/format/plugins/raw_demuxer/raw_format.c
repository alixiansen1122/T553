/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: raw format module file
 * Author: Media Software Group
 * Create: 2020-05-20
 */

#include "raw_format.h"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "demuxer_common_err.h"
#include "demuxer_interface.h"
#include "media_hal_common.h"
#include "raw_demuxer_intf.h"
#include "securec.h"
#include "silk_demuxer.h"
#include "sbc_demuxer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "raw_format"
#define DEMUXER_NUMBER 0x2 /* Number of included demuxers */

typedef struct {
    bool inited;
    pthread_mutex_t mutex;
    uint32_t count;
    DemuxerOpsIntf *intf[DEMUXER_NUMBER];
} RawDemuxerManager;

static RawDemuxerManager g_rawDemuxerManager = {
    .inited = false,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .count = 0,
    .intf = {NULL}
};

#define URL_LEN_MAX 512
#define PROBE_SCORE_GOOD 80
#define PROBE_SCORE_PERFECT 100

typedef struct {
    DemuxerOpsIntf *ops;
    HalHandle demuxer;
    char url[URL_LEN_MAX];
    MediaFileInfo fileInfo;
    FormatMediaParam selectInfo;
    bool eof;
    int32_t readLen;
    unsigned char *probeCacheData;
    int32_t probeCacheDatalen;
    int32_t probeDataReadoffset;
    DataType flags;
    int32_t prepared;
} RawFormatMember;

static void RawDemuxerManagerInit(void)
{
    RawDemuxerManager *manager = &g_rawDemuxerManager;
    pthread_mutex_lock(&manager->mutex);
    if (manager->inited == true) {
        pthread_mutex_unlock(&manager->mutex);
        return;
    }
    manager->intf[0] = GetSilkDemuxer();
    manager->intf[1] = GetSBCDemuxer();
    manager->count = DEMUXER_NUMBER;
    manager->inited = true;
    pthread_mutex_unlock(&manager->mutex);
    return;
}

static int32_t RawFormatGetDemuxer(RawFormatMember *member, int32_t *score)
{
    DemuxerOpsIntf *ops = NULL;
    HalHandle demuxer = NULL;
    bool found = false;
    int32_t probeScore = 0;
    if (strncmp(member->url, "http", strlen("http")) == 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "http demuxer not suitable");
        return MEDIA_HAL_ERR;
    }
    RawDemuxerManagerInit();
    for (uint32_t i = 0; i < g_rawDemuxerManager.count; i++) {
        if (g_rawDemuxerManager.intf[i] == NULL) {
            continue;
        }
        ops = g_rawDemuxerManager.intf[i];
        demuxer = ops->Open(member->url);
        if (demuxer == NULL) {
            continue;
        }
        int32_t ret = ops->Probe(demuxer, &probeScore);
        if (ret != 0 || probeScore < PROBE_SCORE_GOOD) {
            ops->Close(demuxer);
            continue;
        }
        ret = ops->FindStreamInfo(demuxer);
        if (ret != 0) {
            ops->Close(demuxer);
            continue;
        }
        if (probeScore == PROBE_SCORE_PERFECT) {
            found = true;
            break;
        }
        ops->Close(demuxer);
        ops = NULL;
        demuxer = NULL;
    }
    if (found == false || ops == NULL || demuxer == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "raw demuxer not suitable");
        return MEDIA_HAL_ERR;
    }

    if (score != NULL) {
        *score = probeScore;
    }
    member->ops = ops;
    member->demuxer = demuxer;
    return MEDIA_HAL_OK;
}

static int32_t RawFormatOpen(
    const DemuxerSource *source, const ProtocolFun *protocol, HalHandle *fmtHandle, int32_t *score)
{
    MEDIA_HAL_UNUSED(protocol);
    CHK_NULL_RETURN(fmtHandle, ERR_DEMUXER_NULL_PTR, "input param fmtHandle null");
    CHK_NULL_RETURN(source, ERR_DEMUXER_NULL_PTR, "input param source null");
    CHK_FAILED_RETURN(source->type, DEMUXER_SOURCE_TYPE_URI, -1, "only support url type");

    RawFormatMember *member = (RawFormatMember *)malloc(sizeof(RawFormatMember));
    CHK_NULL_RETURN(member, ERR_DEMUXER_MEM_MALLOC, "malloc BufferRawFormatMember failed");
    if (memset_s(member, sizeof(RawFormatMember), 0, sizeof(RawFormatMember)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s RawFormatMember failed");
        goto FREE;
    }
    if (strncpy_s(member->url, URL_LEN_MAX, source->url, sizeof(source->url)) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strncpy_s url failed");
        goto FREE;
    }

    int32_t ret = RawFormatGetDemuxer(member, score);
    if (ret != MEDIA_HAL_OK) {
        goto FREE;
    }

    member->flags = DATA_TYPE_NONE;
    member->prepared = false;

    *fmtHandle = (HalHandle)member;
    MEDIA_HAL_LOGI(MODULE_NAME, "open success");
    return MEDIA_HAL_OK;
FREE:
    free(member);
    return MEDIA_HAL_ERR;
}


static int32_t RawFormatRead(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    RawFormatMember *member = (RawFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    CHK_NULL_RETURN(fmtFrame, ERR_DEMUXER_ILLEGAL_PARAM, "input param fmtFrame null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");
    CHK_NULL_RETURN(member->ops, ERR_DEMUXER_NULL_PTR, "ops null");
    CHK_NULL_RETURN(member->demuxer, ERR_DEMUXER_NULL_PTR, "demuxer null");
    CHK_NULL_RETURN(member->ops->ReadFrame, ERR_DEMUXER_NULL_PTR, "ReadFrame null");

    return member->ops->ReadFrame(member->demuxer, fmtFrame);
}

static int32_t RawFormatClose(HalHandle fmtHandle)
{
    CHK_NULL_RETURN(fmtHandle, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    RawFormatMember *member = (RawFormatMember *)fmtHandle;
    if (member->ops != NULL && member->demuxer != NULL) {
        member->ops->Close(member->demuxer);
    }

    free(member);
    return MEDIA_HAL_OK;
}

static int32_t RawFormatFree(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    RawFormatMember *member = (RawFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    CHK_NULL_RETURN(fmtFrame, ERR_DEMUXER_ILLEGAL_PARAM, "input param fmtFrame null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");
    CHK_NULL_RETURN(member->ops, ERR_DEMUXER_NULL_PTR, "ops null");
    CHK_NULL_RETURN(member->demuxer, ERR_DEMUXER_NULL_PTR, "demuxer null");
    CHK_NULL_RETURN(member->ops->FreeFrame, ERR_DEMUXER_NULL_PTR, "ReadFrame null");

    return member->ops->FreeFrame(member->demuxer, fmtFrame);
}

static int32_t RawFormatSeek(__attribute__((unused)) HalHandle fmtHandle, __attribute__((unused))int32_t streamIndex,
    __attribute__((unused)) int64_t toMs, __attribute__((unused)) uint32_t flag)
{
    RawFormatMember *member = (RawFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");
    CHK_NULL_RETURN(member->ops, ERR_DEMUXER_NULL_PTR, "ops null");
    CHK_NULL_RETURN(member->demuxer, ERR_DEMUXER_NULL_PTR, "demuxer null");
    CHK_NULL_RETURN(member->ops->Seek, ERR_DEMUXER_NULL_PTR, "Seek null");
    return member->ops->Seek(member->demuxer, toMs, flag);
}

static int32_t RawFormatSeekPos(__attribute__((unused)) HalHandle fmtHandle, int64_t pos, uint32_t whence)
{
    MEDIA_HAL_LOGE(MODULE_NAME, "not support");
    MEDIA_HAL_LOGI(MODULE_NAME, "pos:%lld, whence:%d", pos, whence);
    return ERR_DEMUXER_NOT_SUPPORT;
}

static int32_t RawFormatProbe(HalHandle fmtHandle, void* arg)
{
    MEDIA_HAL_UNUSED(arg);
    RawFormatMember *member = (RawFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    CHK_FAILED_RETURN(member->prepared, false, 0, "have prepared");
    CHK_NULL_RETURN(member->ops, ERR_DEMUXER_NULL_PTR, "ops null");
    CHK_NULL_RETURN(member->demuxer, ERR_DEMUXER_NULL_PTR, "demuxer null");
    CHK_NULL_RETURN(member->ops->FindStreamInfo, ERR_DEMUXER_NULL_PTR, "FindStreamInfo null");

    int32_t ret = member->ops->FindStreamInfo(member->demuxer);
    member->prepared = (ret == 0) ? true : false;
    MEDIA_HAL_LOGI(MODULE_NAME, "get out fmt_probe, ret:%d", ret);
    return ret;
}

static int32_t RawFormatGetInfo(const HalHandle fmtHandle, MediaFileInfo *fmtInfo)
{
    RawFormatMember *member = (RawFormatMember *)fmtHandle;

    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    CHK_NULL_RETURN(fmtInfo, ERR_DEMUXER_ILLEGAL_PARAM, "input param fmtInfo null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");
    CHK_NULL_RETURN(member->ops, ERR_DEMUXER_NULL_PTR, "ops null");
    CHK_NULL_RETURN(member->demuxer, ERR_DEMUXER_NULL_PTR, "demuxer null");
    CHK_NULL_RETURN(member->ops->GetInfo, ERR_DEMUXER_NULL_PTR, "GetInfo null");

    int32_t ret = member->ops->GetInfo(member->demuxer, fmtInfo);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetInfo failed, ret:%d", ret);
    }
    return ret;
}

static int32_t RawFormatFind(const DemuxerSource *source, const ProtocolFun *protocol)
{
    if (source == NULL) {
        return ERR_DEMUXER_ILLEGAL_PARAM;
    }
    if (source->type != DEMUXER_SOURCE_TYPE_URI) {
        MEDIA_HAL_LOGI(MODULE_NAME, "only support url source type");
        return ERR_DEMUXER_NOT_SUPPORT;
    }
    if (protocol != NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "not support extra protocol");
        return ERR_DEMUXER_NOT_SUPPORT;
    }
    return MEDIA_HAL_OK;
}

static int32_t RawFormatInvoke(const HalHandle fmtHandle, uint32_t invokeId, void* arg)
{
    CHK_NULL_RETURN(fmtHandle, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    MEDIA_HAL_LOGE(MODULE_NAME, "not support invoke now, invokeId:%d, arg is null:%d", invokeId, (arg == NULL));
    return ERR_DEMUXER_NOT_SUPPORT;
}

DemuxerPluginEntry g_rawFormatDemuxerEntry = {
    .desc = {
        .libName = "libplugin_demuxer_rawstream.so",
        .name = "raw_demuxer",
        .version = "version 1.0.0",
        .description = "silk,sbc",
        .supportCapability = "silk,sbc",
        .priority = PROBE_SCORE_PERFECT,
    },
    .fmt_find = RawFormatFind,
    .fmt_open = RawFormatOpen,
    .fmt_find_stream = RawFormatProbe,
    .fmt_read = RawFormatRead,
    .fmt_free = RawFormatFree,
    .fmt_seek_pts = RawFormatSeek,
    .fmt_seek_pos = RawFormatSeekPos,
    .fmt_getinfo = RawFormatGetInfo,
    .fmt_invoke = RawFormatInvoke,
    .fmt_close = RawFormatClose,
};
#ifdef ENABLE_DL_DEMUXER_PLUGIN
const DemuxerPluginEntry *GetDemuxer(void)
{
    return &g_rawFormatDemuxerEntry;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
