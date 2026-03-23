/*
 * Copyright (c) @CompanyNameMagicTag. 2020-2021. All rights reserved.
 * Description: demuxer manager
 * Author: Media Software Group
 * Create: 2020-11-08
 */

#include "demuxer_manager.h"
#ifdef ENABLE_DEMUXER
#include <sys/time.h>
#ifdef SUPPORT_DL
#include <dlfcn.h>
#endif
#include <unistd.h>
#include "securec.h"
#include "demuxer_common_err.h"
#include "demuxer_plugins_manager.h"
#include "format_common.h"
#include "format_type.h"
#include "media_hal_common.h"
#include "plugins_manager_common.h"
#include "protocol_plugins_manager.h"
#include "media_hal_thread_adapt.h"

#ifdef ENABLE_BUFFER_MANAGER
#include "demuxer_buffer.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MODULE_NAME "DemuxerManager"
#define SELECTED_AUDIO_TRACK_NUM_MAX 32
#define SELECTED_SUB_TRACK_NUM_MAX 32
#define SELECTED_IMG_TRACK_NUM_MAX 32
#define MAX_PROGRAM_NUM 256
#define MAX_TRACK_NUM_PER_PROGRAM 256
#define MAX_SUB_IMG_NUM_PER_TRACK 1024

typedef struct {
    bool inited;
    MediaHalMutexHandle mutex;
} DemuxerManager;

typedef struct {
    int16_t programId;
    int16_t videoTrackId;
    int16_t audioTrackId[SELECTED_AUDIO_TRACK_NUM_MAX];
    int16_t subTrackId[SELECTED_SUB_TRACK_NUM_MAX];
    int16_t imgTrackId[SELECTED_IMG_TRACK_NUM_MAX];
} SelectTrack;

typedef struct {
    void *innerCtx;
    const DemuxerPluginEntry *demuxer;
    FormatState state;
    MediaHalMutexHandle mutex;
    DemuxerSource source;
    void *demuxerHdl;
    FileInfo info;
    FormatCallback callback;
#ifdef ENABLE_BUFFER_MANAGER
    FormatHandle bufferManager;
    bool enableBuffer;
#endif
    SelectTrack selectTrack;
    FormatAlbumArtInfo albumInfo;
} DemuxerContext;

typedef int32_t (*ReadData)(void *handle, uint8_t *data, int32_t size, int32_t timeOutMs, DataType *flags);

DemuxerManager g_demuxerManager = {
    .inited = false,
    .mutex = NULL,
};

static int32_t TransToPluginSource(DemuxerSource *demuxerSrc, const FormatSource *formatSrc)
{
    if (formatSrc->type == SOURCE_TYPE_FD) {
        demuxerSrc->type = DEMUXER_SOURCE_TYPE_FD;
        demuxerSrc->fd = formatSrc->fd;
        demuxerSrc->offset = formatSrc->offset;
    } else if (formatSrc->type == SOURCE_TYPE_URI) {
        demuxerSrc->type = DEMUXER_SOURCE_TYPE_URI;
        if (memcpy_s(demuxerSrc->url, URL_LEN, formatSrc->url, URL_LEN) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
            return -1;
        }
    } else if (formatSrc->type == SOURCE_TYPE_STREAM) {
        CHK_NULL_RETURN(formatSrc->stream, -1, "formatSrc->stream null");
        CHK_NULL_RETURN(formatSrc->stream->ReadData, -1, "formatSrc->stream null");
        CHK_NULL_RETURN(formatSrc->stream->GetReadableSize, -1, "formatSrc->stream null");
        CHK_NULL_RETURN(formatSrc->stream->handle, -1, "formatSrc->stream null");
        demuxerSrc->type = DEMUXER_SOURCE_TYPE_STREAM;
        demuxerSrc->stream.ReadData = (ReadData)formatSrc->stream->ReadData;
        demuxerSrc->stream.GetReadableSize = formatSrc->stream->GetReadableSize;
        demuxerSrc->stream.handle = formatSrc->stream->handle;
    } else {
        demuxerSrc->type = DEMUXER_SOURCE_TYPE_BUT;
    }
    return 0;
}

void DemuxerManagerInit(void)
{
    DemuxerManager *manager = &g_demuxerManager;
    MediaHalInitStaticMutexLock(&manager->mutex);
    MediaHalMutexLock(manager->mutex);
    if (manager->inited == true) {
        MediaHalMutexUnLock(manager->mutex);
        return;
    }
    DemuxerPluginManagerInit();
    ProtocolPluginManagerInit();
    manager->inited = true;
    MediaHalMutexUnLock(manager->mutex);
}

void DemuxerManagerDeInit(void)
{
    MediaHalInitStaticMutexLock(&g_demuxerManager.mutex);
    MediaHalMutexLock(g_demuxerManager.mutex);
    g_demuxerManager.inited = false;
    DemuxerPluginManagerDeInit();
    ProtocolPluginManagerDeInit();
    MediaHalMutexUnLock(g_demuxerManager.mutex);
}

#ifdef ENABLE_BUFFER_MANAGER
static void OpenDemuxerBuffer(DemuxerContext *ctx)
{
    if (ctx->source.type != DEMUXER_SOURCE_TYPE_URI) {
        return;
    }
    bool isHttp = (strncasecmp(ctx->source.url, "https://", strlen("https://")) == 0) ||
        (strncasecmp(ctx->source.url, "http://", strlen("http://")) == 0);
    if (!isHttp) {
        return;
    }
    ctx->bufferManager = CreateDemuxerBuffer();
    if (ctx->bufferManager != NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "open demuxer buffer");
        ctx->enableBuffer = true;
    }
    return;
}
#endif

int32_t DemuxerCreate(const FormatSource *source, FormatHandle * const handle)
{
    CHK_NULL_RETURN(handle, -1, "input param null");
    CHK_NULL_RETURN(source, -1, "input param null");

    DemuxerContext *ctx = (DemuxerContext*)malloc(sizeof(DemuxerContext));
    CHK_NULL_RETURN(ctx, -1, "malloc failed");
    if (memset_s(ctx, sizeof(DemuxerContext), 0, sizeof(DemuxerContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE;
    }
    ctx->demuxer = NULL;
    ctx->state = STATE_IDLE;
    if (TransToPluginSource(&ctx->source, source) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "format source trans to plugin source failed");
        goto FREE;
    }
    MediaHalMutexAttr attr = { false };
    ctx->mutex = MediaHalMutexCreate(&attr);
    ctx->selectTrack.programId = -1;
    ctx->selectTrack.videoTrackId = -1;
    int32_t i;
    for (i = 0; i < SELECTED_AUDIO_TRACK_NUM_MAX; i++) {
        ctx->selectTrack.audioTrackId[i] = -1;
    }
    for (i = 0; i < SELECTED_SUB_TRACK_NUM_MAX; i++) {
        ctx->selectTrack.subTrackId[i] = -1;
    }
    for (i = 0; i < SELECTED_IMG_TRACK_NUM_MAX; i++) {
        ctx->selectTrack.imgTrackId[i] = -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    OpenDemuxerBuffer(ctx);
#endif
    ctx->innerCtx = ctx;
    *handle = (FormatHandle)ctx;

    return 0;
FREE:
    free(ctx);
    return -1;
}

int32_t DemuxerSetParameter(const FormatHandle handle, int32_t trackId, const ParameterItem *metaData,
    int32_t metaDataCnt)
{
    CHK_NULL_RETURN(metaData, -1, "input param metaData null");
    int32_t ret = 0;
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    if (metaDataCnt == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "no param to set");
        return -1;
    }

    MediaHalMutexLock(ctx->mutex);
    if (trackId != FORMAT_INVALID_TRACK_ID && ctx->state == STATE_IDLE) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demuxer not inited, not support set track param");
        ret = -1;
    }
    /* 具体需要实现的根据需要扩展 */
    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}

static int32_t CopyAttachPicToMetadata(const DemuxerContext *ctx, ParameterItem *metaData)
{
    AttachPicInfo covrPic = {0};
    int32_t ret = ctx->demuxer->fmt_invoke(ctx->demuxerHdl, DEMUXER_INVOKE_GET_PICTURE, &covrPic);
    if (ret != 0) {
        return ret;
    }
    metaData->value.pValue = covrPic.data;
    metaData->size = covrPic.size;
    return 0;
}

static int32_t GetAlbumArtInfoToMetadata(DemuxerContext *ctx, ParameterItem *metaData)
{
    if (ctx->albumInfo.info == NULL) {
        AlbumArtInfo albumInfo = {};
        if (memset_s(&albumInfo, sizeof(AlbumArtInfo), 0, sizeof(AlbumArtInfo)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        int32_t ret = ctx->demuxer->fmt_invoke(ctx->demuxerHdl, DEMUXER_INVOKE_GET_ALBUM_ART_INFO, &albumInfo);
        if (ret != 0 || albumInfo.infoCnt == 0 || albumInfo.info == NULL) {
            return -1;
        }

        FormatAlbumArtInfoItem *info = malloc(albumInfo.infoCnt * sizeof(FormatAlbumArtInfoItem));
        if (info == NULL) {
            return MEDIA_HAL_NO_MEM;
        }

        for (uint32_t i = 0; i < albumInfo.infoCnt; i++) {
            info[i].key = albumInfo.info[i].key;
            info[i].value = albumInfo.info[i].value;
        }

        ctx->albumInfo.infoCnt = albumInfo.infoCnt;
        ctx->albumInfo.info = info;
    }

    metaData->value.pValue = ctx->albumInfo.info;
    metaData->size = (int32_t)ctx->albumInfo.infoCnt;
    return 0;
}

int32_t DemuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *metaData)
{
    CHK_NULL_RETURN(metaData, -1, "input param metaData null");
    int32_t ret = 0;
    int32_t i;
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;
    MediaHalMutexLock(ctx->mutex);
    if ((trackId != FORMAT_INVALID_TRACK_ID) && (ctx->state == STATE_IDLE)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demuxer not inited, not support get track param");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    if (ctx->info.programInfo == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ctx->info.programInfo is null, not support get track param");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    if (trackId >= (int32_t)ctx->info.programInfo[0].trackNum) {
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    switch (metaData->key) {
        case KEY_TYPE_EXTRADATA:
            for (i = 0; i < ctx->info.programInfo[0].track[trackId].itemCnt; i++) {
                if (ctx->info.programInfo[0].track[trackId].item[i].key == OPTIONAL_KEY_TYPE_EXTRADATA) {
                    metaData->value.pValue = ctx->info.programInfo[0].track[trackId].item[i].value.pValue;
                    metaData->size = ctx->info.programInfo[0].track[trackId].item[i].size;
                    break;
                }
            }
            if (i >= ctx->info.programInfo[0].track[trackId].itemCnt) {
                ret = -1;
            }
            break;
        case KEY_TYPE_COVR_PIC:
            ret = CopyAttachPicToMetadata(ctx, metaData);
            break;
        case KEY_TYPE_ALBUM_ART_INFO:
            ret = GetAlbumArtInfoToMetadata(ctx, metaData);
            break;
        default:
            break;
    }
    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}

int32_t DemuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack)
{
    CHK_NULL_RETURN(callBack, -1, "input param callBack null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state >= STATE_START) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demuxer not support set callback after inited");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }

    ctx->callback = *callBack;
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

int32_t DemuxerSetBufferConfig(const FormatHandle handle, const FormatBufferSetting *setting)
{
    CHK_NULL_RETURN(setting, -1, "input param setting null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state != STATE_IDLE) {
        MEDIA_HAL_LOGE(MODULE_NAME, "only support before prepare");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    SetBufferConfig(ctx->bufferManager, setting);
#endif
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

int32_t DemuxerGetBufferConfig(const FormatHandle handle, FormatBufferSetting *setting)
{
    CHK_NULL_RETURN(setting, -1, "input param setting null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
#ifdef ENABLE_BUFFER_MANAGER
    GetBufferConfig(ctx->bufferManager, setting);
#endif
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

int32_t DemuxerPrepare(const FormatHandle handle)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "func in");
    ProtocolFun protocolFunc;
    ProtocolFun *func = NULL;
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->source.type == DEMUXER_SOURCE_TYPE_URI) {
        if (ProtocolPluginManagerFindPlugin(ctx->source.url, strlen(ctx->source.url) + 1, &protocolFunc) == 0) {
            func = &protocolFunc;
        }
    }

    ctx->demuxer = DemuxerPluginManagerFindPlugin(&ctx->source, func, &ctx->demuxerHdl);
    if (ctx->demuxer == NULL || ctx->demuxerHdl == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "prepare failed");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    if (ctx->enableBuffer) {
        int32_t ret = StartBufferThread(ctx->bufferManager, ctx->demuxerHdl, ctx->demuxer, ctx->source.url);
        if (ret != 0) {
            DestoryDemuxerBuffer(ctx->bufferManager);
            ctx->enableBuffer = false;
        }
    }
#endif
    ctx->state = STATE_INIT;
    MEDIA_HAL_LOGD(MODULE_NAME, "func out");
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}
static void FreeTrackInfoItem(TrackInfo *info)
{
    if (info->item != NULL) {
        for (int32_t i = 0; i < info->itemCnt; i++) {
            if (info->item[i].value.pValue != NULL) {
                free((void *)info->item[i].value.pValue);
                info->item[i].value.pValue = NULL;
            }
        }
        free(info->item);
        info->item = NULL;
    }
}
static void FreeFileInfo(DemuxerContext *ctx)
{
    if (ctx->info.programInfo == NULL) {
        return;
    }
    uint32_t i;
    for (i = 0; i < ctx->info.programNum; i++) {
        if (ctx->info.programInfo[i].track == NULL) {
            continue;
        }
        for (uint32_t j = 0; j < ctx->info.programInfo[i].trackNum; j++) {
            TrackInfo *info = &ctx->info.programInfo[i].track[j];
            if (info->trackType == TRACK_TYPE_IMAGE && info->imgTrack.subImageInfo != NULL) {
                free(info->imgTrack.subImageInfo);
                info->imgTrack.subImageInfo = NULL;
            }
            FreeTrackInfoItem(info);
        }
        free(ctx->info.programInfo[i].track);
        ctx->info.programInfo[i].track = NULL;
    }
    free(ctx->info.programInfo);
    ctx->info.programInfo = NULL;
}
static void FillVidTrackInfo(VideoTrackInfo *trackInfo, const VideoTrack *info)
{
    trackInfo->format = CodecTypeToCodecFormat(info->format);
    trackInfo->profile = info->profile;
    trackInfo->width = info->width;
    trackInfo->height = info->height;
    trackInfo->fpsNum = info->fpsNum;
    trackInfo->fpsDen = info->fpsDen;
    trackInfo->bitrate = info->bitrate;
    trackInfo->rotate = info->rotate;
    trackInfo->durationMs = info->durationMs;
}

static void FillAudTrackInfo(AudioTrackInfo *trackInfo, const AudioTrack *info)
{
    trackInfo->format = CodecTypeToCodecFormat(info->format);
    trackInfo->profile = info->profile;
    trackInfo->sampleRate = info->sampleRate;
    trackInfo->sampleFmt = info->sampleFmt;
    trackInfo->channels = info->channels;
    trackInfo->subStreamID = info->subStreamID;
    trackInfo->bitrate = info->bitrate;
    trackInfo->durationMs = info->durationMs;
    if (memcpy_s(trackInfo->language, FORMAT_LANGUAGE_LEN, info->language, LANGUAGE_NUM_MAX) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return;
    }
}

static void FillImgTrackInfo(ImageTrackInfo *trackInfo, const ImageTrack *info)
{
    trackInfo->subImageNum = MIN(info->subImageNum, MAX_SUB_IMG_NUM_PER_TRACK);
    if (trackInfo->subImageNum == 0) {
        trackInfo->subImageInfo = NULL;
        return;
    }
    trackInfo->subImageInfo = (SubImageInfo *)malloc(sizeof(SubImageInfo) * trackInfo->subImageNum);
    if (trackInfo->subImageInfo == NULL) {
        trackInfo->subImageNum = 0;
        return;
    }
    for (int32_t i = 0; i < trackInfo->subImageNum; i++) {
        trackInfo->subImageInfo[i].format = CodecTypeToCodecFormat(info->subImageInfo[i].format);
        trackInfo->subImageInfo[i].width = info->subImageInfo[i].width;
        trackInfo->subImageInfo[i].height = info->subImageInfo[i].height;
        trackInfo->subImageInfo[i].rotate = info->subImageInfo[i].rotate;
        trackInfo->subImageInfo[i].dataLength = info->subImageInfo[i].dataLength;
        trackInfo->subImageInfo[i].thumbnail = info->subImageInfo[i].thumbnail;
    }
}

static void FillSubTrackInfo(SubtitleTrackInfo *trackInfo, const SubtitleTrack *info)
{
    trackInfo->format = SubTypeToSubFormat(info->format);
    trackInfo->charSet = info->charSet;
    trackInfo->originalFrameWidth = info->originalFrameWidth;
    trackInfo->originalFrameHeight = info->originalFrameHeight;
    size_t lenDst = FORMAT_MAX_LANGUAGE_NUM * FORMAT_LANGUAGE_LEN;
    size_t lenSrc = LANGUAGE_NUM_MAX * LANGUAGE_LEN_MAX;
    if (memcpy_s(trackInfo->language, lenDst, info->language, lenSrc) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return;
    }
}

static void FillTrackInfo(TrackInfo *trackInfo, MediaTrackInfo *info)
{
    const TrackType typeTrans[TRACK_TYPE_BUT + 1] = {TRACK_TYPE_VIDEO, TRACK_TYPE_AUDIO, TRACK_TYPE_IMAGE,
        TRACK_TYPE_SUB, TRACK_TYPE_DATA, TRACK_TYPE_BUT};
    trackInfo->trackType = typeTrans[info->trackType];
    trackInfo->trackId = info->trackId;
    switch (trackInfo->trackType) {
        case TRACK_TYPE_VIDEO:
            FillVidTrackInfo(&trackInfo->vidTrack, &info->vidTrack);
            break;
        case TRACK_TYPE_AUDIO:
            FillAudTrackInfo(&trackInfo->audTrack, &info->audTrack);
            break;
        case TRACK_TYPE_IMAGE:
            FillImgTrackInfo(&trackInfo->imgTrack, &info->imgTrack);
            break;
        case TRACK_TYPE_SUB:
            FillSubTrackInfo(&trackInfo->subTrack, &info->subTrack);
            break;
        default:
            break;
    }
    if (info->itemCnt == 0 || info->item == NULL) {
        info->itemCnt = 0;
        info->item = NULL;
    }
    trackInfo->itemCnt = 0;
    trackInfo->item = NULL;
    if (info->item != NULL) {
        trackInfo->item = (ParameterItem *)malloc(sizeof(ParameterItem) * info->itemCnt);
        if (trackInfo->item == NULL) {
            return;
        }
        for (int32_t i = 0; i < info->itemCnt; i++) {
            if (memcpy_s(&trackInfo->item[i], sizeof(ParameterItem), &info->item[i], sizeof(OptionalItem)) != EOK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
                free(trackInfo->item);
                trackInfo->item = NULL;
                trackInfo->itemCnt = 0;
                return;
            }
        }
        trackInfo->itemCnt = info->itemCnt;
    }
}

static int32_t FillProgramInfo(ProgramInfo *programInfo, const MediaProgramInfo *info)
{
    programInfo->programId = info->programId;
    programInfo->durationMs = info->durationMs;
    programInfo->trackNum = (info->trackNum <= MAX_TRACK_NUM_PER_PROGRAM) ? info->trackNum : MAX_TRACK_NUM_PER_PROGRAM;
    if (programInfo->trackNum == 0) {
        programInfo->track = NULL;
        return -1;
    }
    programInfo->track = (TrackInfo *)malloc(sizeof(TrackInfo) * programInfo->trackNum);
    if (programInfo->track == NULL) {
        programInfo->trackNum = 0;
        return -1;
    }
    for (uint32_t i = 0; i < programInfo->trackNum; i++) {
        FillTrackInfo(&programInfo->track[i], &info->track[i]);
    }
    return 0;
}

static int32_t SaveDefaultSelectTrack(DemuxerContext *ctx)
{
    int32_t audioTrackNum = 0;
    int32_t subTrackNum = 0;
    int32_t imgTrackNum = 0;
    SelectTrack *selectTrack = &ctx->selectTrack;
    selectTrack->programId = 0;
    ProgramInfo *info = &ctx->info.programInfo[0];
    for (uint32_t i = 0; i < info->trackNum; i++) {
        if (info->track[i].trackType == TRACK_TYPE_VIDEO) {
            if (selectTrack->videoTrackId == -1) {
                selectTrack->videoTrackId = info->track[i].trackId;
            }
        } else if (info->track[i].trackType == TRACK_TYPE_AUDIO && audioTrackNum < SELECTED_AUDIO_TRACK_NUM_MAX) {
            selectTrack->audioTrackId[audioTrackNum++] = info->track[i].trackId;
        } else if (info->track[i].trackType == TRACK_TYPE_SUB && subTrackNum < SELECTED_SUB_TRACK_NUM_MAX) {
            selectTrack->subTrackId[subTrackNum++] = info->track[i].trackId;
        } else if (info->track[i].trackType == TRACK_TYPE_IMAGE && imgTrackNum < SELECTED_SUB_TRACK_NUM_MAX) {
            selectTrack->imgTrackId[imgTrackNum++] = info->track[i].trackId;
        }
    }
    return 0;
}

int32_t DemuxerGetFileInfo(const FormatHandle handle, FileInfo *info)
{
    uint32_t i;
    int ret;
    MediaFileInfo fileInfo = {};
    CHK_NULL_RETURN(info, -1, "input param info null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state == STATE_IDLE || ctx->demuxer == NULL || ctx->demuxerHdl == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not inited, state:%d, or demuxer plugin null", ctx->state);
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    ret = ctx->demuxer->fmt_getinfo(ctx->demuxerHdl, &fileInfo);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "fmt_getinfo failed, ret:%d", ret);
        MediaHalMutexUnLock(ctx->mutex);
        return ret;
    }
    FreeFileInfo(ctx);
    ctx->info.bitrate = fileInfo.bitrate;
    ctx->info.formatName = fileInfo.formatName;
    ctx->info.programNum = (fileInfo.programNum <= MAX_PROGRAM_NUM) ? fileInfo.programNum : MAX_PROGRAM_NUM;
    if (ctx->info.programNum == 0) {
        ctx->info.programInfo = NULL;
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    ctx->info.programInfo = (ProgramInfo*)malloc(sizeof(ProgramInfo) * ctx->info.programNum);
    if (ctx->info.programInfo == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        ctx->info.programNum = 0;
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    if (memset_s(ctx->info.programInfo, sizeof(ProgramInfo) * ctx->info.programNum, 0,
        sizeof(ProgramInfo) * ctx->info.programNum) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }
    for (i = 0; i < ctx->info.programNum; i++) {
        ret = FillProgramInfo(&ctx->info.programInfo[i], &fileInfo.programInfo[i]);
    }
    *info = ctx->info;
    SaveDefaultSelectTrack(ctx);
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

int32_t DemuxerSelectTrack(const FormatHandle handle, int32_t programId, int32_t trackId)
{
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state == STATE_IDLE) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not inited");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    SelectTrack *selectTrack = &ctx->selectTrack;
    selectTrack->programId = (int16_t)programId;
    selectTrack->videoTrackId = (int16_t)trackId;

    int32_t ret = 0;
    if (ctx->demuxer->fmt_select_track != NULL) {
        ret = ctx->demuxer->fmt_select_track(ctx->demuxerHdl, programId, trackId);
    }
    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}

int32_t DemuxerUnselectTrack(const FormatHandle handle, int32_t programId, int32_t trackId)
{
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state == STATE_IDLE) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not inited");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    (void)programId;
    (void)trackId;
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

int32_t DemuxerStart(const FormatHandle handle)
{
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state != STATE_INIT) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not inited, state:%d", ctx->state);
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    ctx->state = STATE_START;
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

int32_t DemuxerGetSelectedTrack(const FormatHandle handle, int32_t *programId, int32_t trackId[], int32_t *nums)
{
    int32_t i;
    int addTrackCnt = 0;
    CHK_NULL_RETURN(nums, -1, "input param nums null");
    CHK_NULL_RETURN(programId, -1, "input param programId null");
    CHK_NULL_RETURN(trackId, -1, "input param trackId null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state == STATE_IDLE) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not call prepare");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
    *programId = ctx->selectTrack.programId;
    if (ctx->selectTrack.videoTrackId != -1) {
        trackId[addTrackCnt] = ctx->selectTrack.videoTrackId;
        addTrackCnt++;
    }
    for (i = 0; i < SELECTED_AUDIO_TRACK_NUM_MAX; i++) {
        if (ctx->selectTrack.audioTrackId[i] == -1 || addTrackCnt == *nums) {
            break;
        }
        trackId[addTrackCnt++] = ctx->selectTrack.audioTrackId[i];
    }
    for (i = 0; i < SELECTED_SUB_TRACK_NUM_MAX; i++) {
        if (ctx->selectTrack.subTrackId[i] == -1 || addTrackCnt == *nums) {
            break;
        }
        trackId[addTrackCnt++] = ctx->selectTrack.subTrackId[i];
    }
    *nums = addTrackCnt;
    MediaHalMutexUnLock(ctx->mutex);
    return 0;
}

static int32_t ReadFrame(DemuxerContext *ctx, FormatFrame *frame)
{
    int ret;
    FormatPacket packet;
    ret = ctx->demuxer->fmt_read(ctx->demuxerHdl, &packet);
    if ((ret == MEDIA_HAL_OK) && (packet.data != NULL) && (packet.len == 0)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "fmt_read failed frame len:%d", frame->len);
        return MEDIA_HAL_ERR;
    }

    if (ret == MEDIA_HAL_OK) {
        FormatPacketToFrame(&packet, frame);
        packet.item = NULL;
        packet.itemCnt = 0;
    }
    return ret;
}

int32_t DemuxerReadFrame(const FormatHandle handle, FormatFrame *frame, int32_t timeOutMs)
{
    MEDIA_HAL_UNUSED(timeOutMs);
    int ret;
    CHK_NULL_RETURN(frame, -1, "input param null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state != STATE_START) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not started");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    if (ctx->enableBuffer) {
        ret = ReadBufferFrame(ctx->bufferManager, frame);
    } else {
#endif
        ret = ReadFrame(ctx, frame);
#ifdef ENABLE_BUFFER_MANAGER
    }
#endif

    if ((ret == MEDIA_HAL_OK) && (frame->data != NULL) && (frame->len == 0)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "fmt_read failed frame len:%d", frame->len);
        ret = MEDIA_HAL_ERR;
    }
    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}

int32_t DemuxerFreeFrame(const FormatHandle handle, FormatFrame *frame)
{
    int ret;
    CHK_NULL_RETURN(frame, -1, "input param frame null");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state != STATE_START) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not started");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    if (ctx->enableBuffer) {
        ret = FreeBufferFrame(frame);
    } else {
#endif
        FormatPacket packet;
        FormatFrameToPacket(frame, &packet);
        ret = ctx->demuxer->fmt_free(ctx->demuxerHdl, &packet);
#ifdef ENABLE_BUFFER_MANAGER
    }
#endif
    frame->data = NULL;
    frame->len = 0;
    frame->item = NULL;
    frame->itemCnt = 0;

    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}


int32_t DemuxerSeek(const FormatHandle handle, int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "func in");
    int ret;
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state != STATE_START) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not started");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    if (ctx->enableBuffer) {
        ret = SeekBuffer(ctx->bufferManager, streamIndex, timeStampUs, mode);
    } else {
#endif
        ret = ctx->demuxer->fmt_seek_pts(ctx->demuxerHdl, streamIndex, timeStampUs, mode);
#ifdef ENABLE_BUFFER_MANAGER
    }
#endif
    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}

int32_t DemuxerStop(const FormatHandle handle)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "func in");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
    if (ctx->state != STATE_START) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not started");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }
#ifdef ENABLE_BUFFER_MANAGER
    if (ctx->enableBuffer) {
        StopBufferThread(ctx->bufferManager);
    }
#endif

    ctx->state = STATE_STOP;
    MediaHalMutexUnLock(ctx->mutex);
    MEDIA_HAL_LOGD(MODULE_NAME, "func out");
    return 0;
}

int32_t DemuxerDestroy(const FormatHandle handle)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "func in");
    const DemuxerContext *cur = (const DemuxerContext*)handle;
    CHK_HANDLE_VALID(cur);
    DemuxerContext *ctx = (DemuxerContext *)cur->innerCtx;

    MediaHalMutexLock(ctx->mutex);
#ifdef ENABLE_BUFFER_MANAGER
    if (ctx->enableBuffer) {
        DestoryDemuxerBuffer(&(ctx->bufferManager));
        ctx->enableBuffer = false;
    }
#endif
    if (ctx->state != STATE_IDLE) {
        int ret = ctx->demuxer->fmt_close(ctx->demuxerHdl);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "fmt_close failed, ret:%d", ret);
        }
    }
    ctx->demuxer = NULL;
    ctx->demuxerHdl = NULL;
    if (ctx->albumInfo.info != NULL) {
        free(ctx->albumInfo.info);
        ctx->albumInfo.info = NULL;
        ctx->albumInfo.infoCnt = 0;
    }
    FreeFileInfo(ctx);
    MediaHalMutexUnLock(ctx->mutex);
    MediaHalMutexDestroy(&ctx->mutex);
    ctx->innerCtx = NULL;
    free(ctx);
    MEDIA_HAL_LOGD(MODULE_NAME, "func out");

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* ENABLE_DEMUXER */
