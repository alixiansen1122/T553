/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: demuxer module header file
 * Author: Media Software Group
 * Create: 2017-12-22
 */

#ifndef DEMUXER_COMMON_H
#define DEMUXER_COMMON_H

#include "plugin_common.h"

/** \addtogroup     PLAYER */
/** @{ */ /** <!-- [PLAYER] */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define DEMUXER_RESOLUTION_CNT     5
#define DEMUXER_NO_MEDIA_STREAM    (-1)

#define DEMUXER_ENTRY_NAME "g_stFormatEntry"

#define RET_FILE_EOF  2
#define RET_NODATA  3


typedef struct {
    int32_t s32VideoStreamIndex; /* the index of the video stream */
    uint32_t u32Width;            /* The height of the media file's resolution */
    uint32_t u32Height;           /* The width of the media file's resolution */
    CodecType enVideoType;
} StreamResolution;

typedef struct {
    char *formatName;  /**< File format name, in the unit of byte. */
    int64_t s64FileSize;  /* File size, in the unit of byte. */
    int64_t s64StartTime; /* the media file begin time */
    int64_t s64Duration;  /* Total duration of a file, in the unit of ms. */
    StreamResolution stSteamResolution[DEMUXER_RESOLUTION_CNT];
    int32_t s32UsedVideoStreamIndex; /* <default used video index> */
    float fFrameRate;            /* the frame rate of the stream */
    uint32_t u32Bitrate;              /* File bit rate, in the unit of bit/s. */
    uint32_t u32AudioChannelCnt;
    uint32_t u32SampleRate;           /* the sample rate of the audio stream */
    int32_t s32UsedAudioStreamIndex; /* the index of the audio stream. one file may have many audio streams */
    uint32_t u32Width;            /* The height of the media file's resolution */
    uint32_t u32Height;           /* The width of the media file's resolution */
    CodecType enVideoType;
    CodecType enAudioType;
} FormatFileInfo;

/** @} */ /** <!-- ==== PLAYER End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
