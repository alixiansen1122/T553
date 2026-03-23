/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: silk demuxer module file
 * Author: Media Software Group
 * Create: 2022-12-10
 */

#include "silk_demuxer.h"

#include <errno.h>
#include <unistd.h>
#include "securec.h"
#include "demuxer_common_err.h"
#include "demuxer_interface.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "silk_demuxer"

#define SHIFT_24 24
#define SHIFT_16 16
#define SHIFT_8 8
#define MASK_BITS_3 0x07
#define MASK_BYTE 0xFF

#define URL_LEN_MAX 256
#define PROBE_CACHE_BUF_SIZE 16
#define SILK_HEAD_SIZE 9
#define SILK_BLOCK_HEAD_SIZE 6
#define SILK_BLOCK_MODE_MASK 0x3
#define PROBE_SILK_BLOCK_COUNT 3
#define SILK_DEFAULT_DURATION_PER_FRAME 20000 /* 20ms */
#define SILK_SAMPLRATE_8K 8000
#define SILK_SAMPLRATE_12K 12000
#define SILK_SAMPLRATE_24K 24000
#define SILK_DEFAULT_SAMPLRATE_16K 16000 /* Default sampling rate of silk */
#define SILK_DEFAULT_CHANNEL 1 /* Default number of silk channels */
#define SILK_DEMUXER_NAME "silk_v3"
#define SILK_HEAD_INFO "#!SILK_V3"
#define SILK_BLOCK_LENGTH_MASK 0x1FFF /* Mask for obtaining the frame length in silk block mode */
#define SILK_TECENT_MODE_IDENTITY 0x02
#define PROBE_SCORE_PERFECT 100

typedef enum {
    SILK_BLOCK_MODE_NB = 0,
    SILK_BLOCK_MODE_MB,
    SILK_BLOCK_MODE_WB,
    SILK_BLOCK_MODE_SWB,
    SILK_BLOCK_MODE_BUTT,
} SilkBlockMode;

typedef struct {
    DemuxerOpsIntf intf;
    char url[URL_LEN_MAX];
    MediaFileInfo fileInfo;
    uint8_t silkHead[SILK_HEAD_SIZE];
    uint8_t silkPrivateHead;
    char name[SILK_HEAD_SIZE];
    bool eof;
    bool withBlock;
    bool probeSuccessed;
    bool firstFrame;
    int64_t position;
    int32_t sampleRate;
    int32_t channel;
    int32_t readLen;
    int32_t prepared;
    FILE *file;
    int64_t fileSeekStartPosition;
} SilkRawDemuxer;

static HalHandle SilkDemuxerOpen(const char *url)
{
    if (url == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input param uri null");
        return (HalHandle)NULL;
    }

    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)malloc(sizeof(SilkRawDemuxer));
    if (demuxer == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return (HalHandle)NULL;
    }
    if (memset_s(demuxer, sizeof(SilkRawDemuxer), 0, sizeof(SilkRawDemuxer)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE;
    }
    if (strncpy_s(demuxer->url, URL_LEN_MAX, url, URL_LEN_MAX - 1) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strncpy_s failed");
        goto FREE;
    }
    if (snprintf_s(demuxer->name, SILK_HEAD_SIZE, SILK_HEAD_SIZE - 1, SILK_DEMUXER_NAME)  < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "snprintf_s failed");
        goto FREE;
    }

    demuxer->file = fopen(demuxer->url, "rb");
    if (demuxer->file == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "silk file fopen failed, errno:%d", errno);
        goto FREE;
    }
    demuxer->eof = false;
    demuxer->probeSuccessed = false;
    demuxer->withBlock = false;
    demuxer->sampleRate = SILK_DEFAULT_SAMPLRATE_16K;
    demuxer->channel = SILK_DEFAULT_CHANNEL;
    demuxer->prepared = false;
    demuxer->firstFrame = true;
    demuxer->position = 0;
    demuxer->fileSeekStartPosition = 0;
    MEDIA_HAL_LOGI(MODULE_NAME, "buffer Demuxer open");
    return (HalHandle)demuxer;
FREE:
    free(demuxer);
    return NULL;
}

static void SilkDemuxerClose(HalHandle fmtHandle)
{
    if (fmtHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input hanle null");
        return;
    }

    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;

    if (demuxer->fileInfo.programInfo != NULL) {
        if (demuxer->fileInfo.programInfo[0].track != NULL) {
            free(demuxer->fileInfo.programInfo[0].track);
            demuxer->fileInfo.programInfo[0].track = NULL;
        }
        free(demuxer->fileInfo.programInfo);
        demuxer->fileInfo.programInfo = NULL;
    }

    if (demuxer->file != NULL) {
        if (fclose(demuxer->file) != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "fclose falied");
        }
    }

    free(demuxer);
}

static void FillSilkHeadInfo(SilkRawDemuxer *demuxer, uint8_t *data, uint32_t *len)
{
    if (demuxer->silkPrivateHead != 0) {
        data[0] = demuxer->silkPrivateHead;
        *len += 1;
        for (uint32_t i = 0; i < SILK_HEAD_SIZE; i++) {
            data[i + 1] = demuxer->silkHead[i];
        }
    } else {
        for (uint32_t i = 0; i < SILK_HEAD_SIZE; i++) {
            data[i] = demuxer->silkHead[i];
        }
    }
    *len += SILK_HEAD_SIZE;
}

static void FillFrameCommonInfo(FormatPacket *fmtFrame, int64_t timestamp)
{
    fmtFrame->packetType = PACKET_TYPE_AUDIO;
    fmtFrame->trackId = 0;
    fmtFrame->isKeyFrame = true;
    fmtFrame->timestampUs = timestamp;
    fmtFrame->durationUs = SILK_DEFAULT_DURATION_PER_FRAME;
    fmtFrame->frameIndex = 0;
    fmtFrame->itemCnt = 0;
    fmtFrame->item = NULL;
}
static int32_t SilkDemuxerReadFrameSizeAndPlayData(SilkRawDemuxer *demuxer, FormatPacket *fmtFrame,
    uint32_t *len, uint16_t length, uint16_t lengthSize)
{
    uint32_t bufLen = length + lengthSize + SILK_HEAD_SIZE;
    if (demuxer->firstFrame) {
        FillSilkHeadInfo(demuxer, fmtFrame->data, len);
    }
    for (uint16_t i = 0; i < lengthSize; ++i) {
        if ((*len + i) < bufLen) {
            fmtFrame->data[*len + i] = (length >> (SHIFT_8 * i)) & MASK_BYTE;
        }
    }
    *len += lengthSize;
    size_t readLen = fread(fmtFrame->data + *len, 1, length, demuxer->file);
    if (readLen < length) {
        free(fmtFrame->data);
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "fread failed, eof:%d", feof(demuxer->file));
        demuxer->eof = true;
        return RET_FILE_EOF;
    }
    *len += length;
    return MEDIA_HAL_OK;
}
static int32_t SilkDemuxerReadFrameWithBlock(SilkRawDemuxer *demuxer, FormatPacket *fmtFrame)
{
    uint8_t data[SILK_BLOCK_HEAD_SIZE];
    int16_t lengthSize = sizeof(uint16_t);
    size_t readLen = fread(data, 1, SILK_BLOCK_HEAD_SIZE, demuxer->file);
    if (readLen < SILK_BLOCK_HEAD_SIZE) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "fread failed, eof:%d", feof(demuxer->file));
        demuxer->eof = true;
        return RET_FILE_EOF;
    }
    uint16_t length = (*((int16_t *)data)) & SILK_BLOCK_LENGTH_MASK;
    int64_t timestamp = (data[0x2] << SHIFT_24) | (data[0x3] << SHIFT_16) | (data[0x4] << SHIFT_8) | data[0x5];
    int32_t bufLen = length + lengthSize + SILK_HEAD_SIZE;
    fmtFrame->data = (uint8_t*)malloc(bufLen);
    if (fmtFrame->data == NULL) {
        fmtFrame->len = 0;
        fseek(demuxer->file, -SILK_BLOCK_HEAD_SIZE, SEEK_CUR);
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc buffer failed");
        return MEDIA_HAL_NO_MEM;
    }
    uint32_t len = 0;
    int32_t ret = SilkDemuxerReadFrameSizeAndPlayData(demuxer, fmtFrame, &len, length, (uint16_t)lengthSize);
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    fmtFrame->len = len;
    FillFrameCommonInfo(fmtFrame, timestamp);
    demuxer->firstFrame = false;
    demuxer->position += (length + SILK_BLOCK_HEAD_SIZE);
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerReadFrameWithoutBlock(SilkRawDemuxer *demuxer, FormatPacket *fmtFrame)
{
    uint16_t length;
    size_t lengthSize = sizeof(uint16_t);
    size_t readLen = fread(&length, 1, lengthSize, demuxer->file);
    if (readLen < lengthSize) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "fread failed, eof:%d", feof(demuxer->file));
        demuxer->eof = true;
        return RET_FILE_EOF;
    }
    uint32_t bufLen = length + lengthSize + SILK_HEAD_SIZE;
    if (demuxer->silkPrivateHead != 0) {
        bufLen = bufLen + 1;
    }
    fmtFrame->data = (uint8_t*)malloc(bufLen);
    if (fmtFrame->data == NULL) {
        fmtFrame->len = 0;
        fseek(demuxer->file, -lengthSize, SEEK_CUR);
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc buffer failed");
        return MEDIA_HAL_NO_MEM;
    }
    uint32_t len = 0;
    int32_t ret = SilkDemuxerReadFrameSizeAndPlayData(demuxer, fmtFrame, &len, length, lengthSize);
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    fmtFrame->len = len;
    FillFrameCommonInfo(fmtFrame, -1);
    demuxer->firstFrame = false;
    demuxer->position += (int64_t)fmtFrame->len;
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerReadFrame(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    if (demuxer->eof) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        return RET_FILE_EOF;
    }
    if (!demuxer->withBlock) {
        return SilkDemuxerReadFrameWithoutBlock(demuxer, fmtFrame);
    }
    return SilkDemuxerReadFrameWithBlock(demuxer, fmtFrame);
}

static int32_t SilkDemuxerFreeFrame(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    if (fmtFrame->data != NULL) {
        free(fmtFrame->data);
        fmtFrame->data = NULL;
    }
    MEDIA_HAL_UNUSED(fmtHandle);
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerSeek(__attribute__((unused)) HalHandle fmtHandle, __attribute__((unused)) int64_t toMs,
    __attribute__((unused)) uint32_t flag)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    if (toMs == 0) {
        int32_t ret = fseek(demuxer->file, demuxer->fileSeekStartPosition, SEEK_SET);
        if (ret != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "failed to seek to the silk file header.");
            return MEDIA_HAL_ERR;
        }
        demuxer->eof = false;
        demuxer->position = demuxer->fileSeekStartPosition;
        MEDIA_HAL_LOGI(MODULE_NAME, "SilkRawDemuxer seek success = %d!!", ftell(demuxer->file));
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetBlockSampleRateAndLength(SilkRawDemuxer *demuxer, int32_t *sampleRate, int16_t *length)
{
    uint8_t data[PROBE_CACHE_BUF_SIZE];
    int32_t sampleTable[SILK_BLOCK_MODE_BUTT] = {
        SILK_SAMPLRATE_8K, SILK_SAMPLRATE_12K, SILK_DEFAULT_SAMPLRATE_16K, SILK_SAMPLRATE_24K
    };
    size_t readLen = fread(data, 1, SILK_BLOCK_HEAD_SIZE, demuxer->file);
    if (readLen < SILK_BLOCK_HEAD_SIZE) {
        MEDIA_HAL_LOGI(MODULE_NAME, "file size < %d", PROBE_CACHE_BUF_SIZE);
        return MEDIA_HAL_ERR;
    }
    int32_t sampleRateIndex = (data[0] >> 0x5) & MASK_BITS_3;
    if (sampleRateIndex >= SILK_BLOCK_MODE_BUTT) {
        return MEDIA_HAL_ERR;
    }
    *sampleRate = sampleTable[sampleRateIndex];
    *length = (*((int16_t *)data)) & SILK_BLOCK_LENGTH_MASK;
    return MEDIA_HAL_OK;
}


static int32_t SilkDemuxerProbeBlock(SilkRawDemuxer *demuxer)
{
    int32_t sampleRate = 0;
    for (int32_t i = 0; i < PROBE_SILK_BLOCK_COUNT; i++) {
        int16_t length = 0;
        if (GetBlockSampleRateAndLength(demuxer, &sampleRate, &length) != MEDIA_HAL_OK) {
            return MEDIA_HAL_ERR;
        }
        if (sampleRate == 0) {
            return MEDIA_HAL_ERR;
        } else {
            demuxer->sampleRate = sampleRate;
        }
        if (fseek(demuxer->file, length, SEEK_CUR) != 0) {
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}
static int32_t ProbeScore(SilkRawDemuxer *demuxer, int32_t *score, int32_t offset)
{
    demuxer->probeSuccessed = true;
    demuxer->position = offset + SILK_HEAD_SIZE;
    demuxer->fileSeekStartPosition = offset + SILK_HEAD_SIZE;
    if (fseek(demuxer->file, offset + SILK_HEAD_SIZE, SEEK_SET) != 0) {
        return MEDIA_HAL_ERR;
    }
    if (score != NULL) {
        *score = PROBE_SCORE_PERFECT;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "withBlock :%d", demuxer->withBlock);
    return MEDIA_HAL_OK;
}
static int32_t SilkDemuxerProbe(HalHandle fmtHandle, int32_t *score)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    CHK_FAILED_RETURN(demuxer->prepared, false, 0, "have prepared");
    CHK_NULL_RETURN(demuxer->file, ERR_DEMUXER_INVALID_HANDLE, "file handle null");
    bool privateMode = false;
    uint8_t data[PROBE_CACHE_BUF_SIZE];
    int32_t offset = 0;
    size_t readLen = fread(data, 1, SILK_HEAD_SIZE, demuxer->file);
    if (readLen < SILK_HEAD_SIZE) {
        MEDIA_HAL_LOGI(MODULE_NAME, "file size < %d", PROBE_CACHE_BUF_SIZE);
        return MEDIA_HAL_ERR;
    }
    data[SILK_HEAD_SIZE] = '\0';
    demuxer->silkPrivateHead = 0;
    if (data[0] == SILK_TECENT_MODE_IDENTITY) {
        offset = 1;
        privateMode = true;
        readLen = fread(&data[SILK_HEAD_SIZE], 1, 1, demuxer->file);
        if (readLen < 1) {
            MEDIA_HAL_LOGI(MODULE_NAME, "file size < %d", PROBE_CACHE_BUF_SIZE);
            return MEDIA_HAL_ERR;
        }
        data[SILK_HEAD_SIZE + 1] = '\0';
        demuxer->silkPrivateHead = data[0];
    }
    if (strcmp((const char*)&data[offset], SILK_HEAD_INFO) != 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "not silk_v3 file");
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(demuxer->silkHead, SILK_HEAD_SIZE, &data[offset], SILK_HEAD_SIZE) != 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "memcpy_s failed");
        return MEDIA_HAL_ERR;
    }
    if (privateMode) {
        demuxer->withBlock = false;
        return ProbeScore(demuxer, score, offset);
    }
    if (SilkDemuxerProbeBlock(demuxer) != MEDIA_HAL_OK) {
        demuxer->withBlock = false;
        return ProbeScore(demuxer, score, offset);
    }
    demuxer->withBlock = true;
    return ProbeScore(demuxer, score, offset);
}

static int32_t SilkDemuxerFindStreamInfo(HalHandle fmtHandle)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    if (demuxer->prepared != 0) {
        return MEDIA_HAL_OK;
    }
    demuxer->fileInfo.formatName = demuxer->name;
    demuxer->fileInfo.bitrate = 0;
    demuxer->fileInfo.programNum = 1;
    demuxer->fileInfo.programInfo = (MediaProgramInfo *)malloc(sizeof(MediaProgramInfo) * demuxer->fileInfo.programNum);
    if (demuxer->fileInfo.programInfo == NULL) {
        demuxer->fileInfo.programNum = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "silk programInfo malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    demuxer->fileInfo.programInfo[0].programId = 0;
    demuxer->fileInfo.programInfo[0].durationMs = -1;
    demuxer->fileInfo.programInfo[0].trackNum = 1;
    demuxer->fileInfo.programInfo[0].track = (MediaTrackInfo *)malloc(sizeof(MediaTrackInfo) *
        demuxer->fileInfo.programInfo[0].trackNum);
    if (demuxer->fileInfo.programInfo[0].track == NULL) {
        demuxer->fileInfo.programInfo[0].trackNum = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "silk track malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    MediaTrackInfo *track = demuxer->fileInfo.programInfo[0].track;
    track[0].trackType = MEDIA_TRACK_TYPE_AUDIO;
    track[0].trackId = 0;
    track[0].audTrack.format = CODEC_TYPE_SILK;
    track[0].audTrack.profile = 0;
    track[0].audTrack.sampleRate = (uint32_t)demuxer->sampleRate;
    track[0].audTrack.sampleFmt = 1;
    track[0].audTrack.channels = demuxer->channel;
    track[0].audTrack.subStreamID = -1;
    track[0].audTrack.bitrate = 0;
    track[0].audTrack.language[0] = '\0';
    track[0].audTrack.durationMs = -1;
    track[0].itemCnt = 0;
    track[0].item = NULL;

    demuxer->prepared = true;
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerGetInfo(const HalHandle fmtHandle, MediaFileInfo *fmtInfo)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    *fmtInfo = demuxer->fileInfo;
    return MEDIA_HAL_OK;
}

static DemuxerOpsIntf g_silkDemuxerOps = {
    .Open = SilkDemuxerOpen,
    .Probe = SilkDemuxerProbe,
    .FindStreamInfo = SilkDemuxerFindStreamInfo,
    .GetInfo = SilkDemuxerGetInfo,
    .ReadFrame = SilkDemuxerReadFrame,
    .FreeFrame = SilkDemuxerFreeFrame,
    .Seek = SilkDemuxerSeek,
    .Close = SilkDemuxerClose,
};

DemuxerOpsIntf *GetSilkDemuxer(void)
{
    return &g_silkDemuxerOps;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
