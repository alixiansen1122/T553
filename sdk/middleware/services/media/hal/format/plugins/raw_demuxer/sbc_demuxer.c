/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: sbc demuxer module file
 * Author: Media Software Group
 * Create: 2023-06-28
 */

#include "sbc_demuxer.h"

#include <errno.h>
#include <unistd.h>
#include "securec.h"
#include "demuxer_common_err.h"
#include "demuxer_interface.h"
#include "media_hal_common.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "sbc_demuxer"

#define URL_LEN_MAX 256
#define SBC_DEMUXER_NAME "sbc_demuxer"
#define SBC_DEMUXER_NAME_SIZE 12
#define SBC_BLOCK_DATA_SIZE 256
#define PROBE_SCORE_PERFECT 100

#define SBC_DEFAULT_SAMPLRATE_16K 16000 /* Default sampling rate of sbc */
#define SBC_DEFAULT_CHANNEL 1 /* Default number of sbc channels */

typedef struct {
    DemuxerOpsIntf intf;
    MediaFileInfo fileInfo;
    char url[URL_LEN_MAX];
    char name[SBC_DEMUXER_NAME_SIZE];
    bool eof;
    bool probeSuccessed;
    int32_t readLen;
    bool prepared;
    FILE *file;
} SBCDemuxer;

static HalHandle SBCDemuxerOpen(const char *url)
{
    CHK_NULL_RETURN(url, NULL, "input param uri null");
    SBCDemuxer *demuxer = (SBCDemuxer *)malloc(sizeof(SBCDemuxer));
    CHK_NULL_RETURN(demuxer, NULL, "malloc failed");
    if (memset_s(demuxer, sizeof(SBCDemuxer), 0, sizeof(SBCDemuxer)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE;
    }
    if (strncpy_s(demuxer->url, URL_LEN_MAX, url, URL_LEN_MAX - 1) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strncpy_s failed");
        goto FREE;
    }
    if (snprintf_s(demuxer->name, SBC_DEMUXER_NAME_SIZE, SBC_DEMUXER_NAME_SIZE - 1, SBC_DEMUXER_NAME)  < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "snprintf_s failed");
        goto FREE;
    }

    demuxer->file = fopen(demuxer->url, "rb");
    if (demuxer->file == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sbc file fopen failed, errno:%d", errno);
        goto FREE;
    }

    demuxer->eof = false;
    demuxer->probeSuccessed = false;
    demuxer->prepared = false;
    MEDIA_HAL_LOGI(MODULE_NAME, "demuxer open success");
    return (HalHandle)demuxer;
FREE:
    free(demuxer);
    return NULL;
}

static void SBCDemuxerClose(HalHandle fmtHandle)
{
    CHK_NULL_RETURN_NONE(fmtHandle, "input hanle null");

    SBCDemuxer *demuxer = (SBCDemuxer *)fmtHandle;
    if (demuxer->file != NULL) {
        if (fclose(demuxer->file) != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "fclose falied");
        }
    }

    if (demuxer->fileInfo.programInfo != NULL) {
        if (demuxer->fileInfo.programInfo[0].track != NULL) {
            free(demuxer->fileInfo.programInfo[0].track);
            demuxer->fileInfo.programInfo[0].track = NULL;
        }
        free(demuxer->fileInfo.programInfo);
        demuxer->fileInfo.programInfo = NULL;
    }

    free(demuxer);
}

static void FillFrameCommonInfo(FormatPacket *fmtFrame, int64_t timestamp)
{
    fmtFrame->packetType = PACKET_TYPE_AUDIO;
    fmtFrame->trackId = 0;
    fmtFrame->isKeyFrame = true;
    fmtFrame->timestampUs = timestamp;
    fmtFrame->durationUs = -1;
    fmtFrame->frameIndex = 0;
    fmtFrame->itemCnt = 0;
    fmtFrame->item = NULL;
}

static int32_t SBCDemuxerReadFrame(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    SBCDemuxer *demuxer = (SBCDemuxer *)fmtHandle;
    if (demuxer->eof) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        return RET_FILE_EOF;
    }

    fmtFrame->data = (uint8_t *)malloc(SBC_BLOCK_DATA_SIZE);
    if (fmtFrame->data == NULL) {
        fmtFrame->len = 0;
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc falied");
        return MEDIA_HAL_NO_MEM;
    }

    uint32_t readLen = fread(fmtFrame->data, 1, SBC_BLOCK_DATA_SIZE, demuxer->file);
    if (readLen == 0) {
        free(fmtFrame->data);
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        demuxer->eof = true;
        return RET_FILE_EOF;
    }
    fmtFrame->packetType = PACKET_TYPE_AUDIO;
    fmtFrame->len = readLen;
    FillFrameCommonInfo(fmtFrame, -1);
    return MEDIA_HAL_OK;
}

static int32_t SBCDemuxerFreeFrame(const HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    MEDIA_HAL_UNUSED(fmtHandle);
    if (fmtFrame->data != NULL) {
        free(fmtFrame->data);
        fmtFrame->data = NULL;
    }
    return MEDIA_HAL_OK;
}

static int32_t SBCDemuxerSeek(HalHandle fmtHandle, int64_t toMs, uint32_t flag)
{
    SBCDemuxer *demuxer = (SBCDemuxer *)fmtHandle;
    MEDIA_HAL_UNUSED(flag);
    if (toMs == 0) {
        int32_t ret = fseek(demuxer->file, toMs, SEEK_SET);
        if (ret != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "failed to seek to the sbc file header.");
            return MEDIA_HAL_ERR;
        }
        demuxer->eof = false;
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t SBCDemuxerProbe(HalHandle fmtHandle, int32_t *score)
{
    SBCDemuxer *demuxer = (SBCDemuxer *)fmtHandle;
    CHK_FAILED_RETURN(demuxer->prepared, false, 0, "have prepared");
    CHK_NULL_RETURN(demuxer->file, ERR_DEMUXER_INVALID_HANDLE, "file handle null");

    char *extension = strrchr(demuxer->url, '.');
    if (extension != NULL && strstr(extension + 1, "sbc") != NULL) {
        *score = PROBE_SCORE_PERFECT;
    }

    demuxer->probeSuccessed = true;
    return MEDIA_HAL_OK;
}

static int32_t SBCDemuxerFindStreamInfo(HalHandle fmtHandle)
{
    SBCDemuxer *demuxer = (SBCDemuxer *)fmtHandle;
    if (demuxer->prepared) {
        return MEDIA_HAL_OK;
    }

    demuxer->fileInfo.formatName = demuxer->name;
    demuxer->fileInfo.bitrate = 0;
    demuxer->fileInfo.programNum = 1;
    demuxer->fileInfo.programInfo = (MediaProgramInfo *)malloc(sizeof(MediaProgramInfo) * demuxer->fileInfo.programNum);
    if (demuxer->fileInfo.programInfo == NULL) {
        demuxer->fileInfo.programNum = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "sbc programInfo malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    demuxer->fileInfo.programInfo[0].programId = 0;
    demuxer->fileInfo.programInfo[0].durationMs = -1;
    demuxer->fileInfo.programInfo[0].trackNum = 1;
    demuxer->fileInfo.programInfo[0].track = (MediaTrackInfo *)malloc(sizeof(MediaTrackInfo) *
        demuxer->fileInfo.programInfo[0].trackNum);
    if (demuxer->fileInfo.programInfo[0].track == NULL) {
        demuxer->fileInfo.programInfo[0].trackNum = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "sbc track malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    MediaTrackInfo *track = demuxer->fileInfo.programInfo[0].track;
    track[0].trackType = MEDIA_TRACK_TYPE_AUDIO;
    track[0].trackId = 0;
    track[0].audTrack.profile = 0;
    track[0].audTrack.format = CODEC_TYPE_SBC;
    track[0].audTrack.sampleRate = SBC_DEFAULT_SAMPLRATE_16K;
    track[0].audTrack.sampleFmt = 1;
    track[0].audTrack.channels = SBC_DEFAULT_CHANNEL;
    track[0].audTrack.subStreamID = -1;
    track[0].audTrack.bitrate = 0;
    track[0].audTrack.language[0] = '\0';
    track[0].audTrack.durationMs = -1;
    track[0].itemCnt = 0;
    track[0].item = NULL;

    demuxer->prepared = true;
    return MEDIA_HAL_OK;
}

static int32_t SBCDemuxerGetInfo(const HalHandle fmtHandle, MediaFileInfo *fmtInfo)
{
    SBCDemuxer *demuxer = (SBCDemuxer *)fmtHandle;
    *fmtInfo = demuxer->fileInfo;
    return MEDIA_HAL_OK;
}

static DemuxerOpsIntf g_sbcDemuxerOps = {
    .Open = SBCDemuxerOpen,
    .Probe = SBCDemuxerProbe,
    .FindStreamInfo = SBCDemuxerFindStreamInfo,
    .GetInfo = SBCDemuxerGetInfo,
    .ReadFrame = SBCDemuxerReadFrame,
    .FreeFrame = SBCDemuxerFreeFrame,
    .Seek = SBCDemuxerSeek,
    .Close = SBCDemuxerClose,
};

DemuxerOpsIntf *GetSBCDemuxer(void)
{
    return &g_sbcDemuxerOps;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
