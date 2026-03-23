/*
 * Copyright (c) @CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ffmpeg wrapper header
 * Author: Media Software Group
 * Create: 2025-08-08
 */

#ifndef FF_WRAPPER_H
#define FF_WRAPPER_H

#include <pthread.h>
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "demuxer_common.h"
#include "demuxer_interface.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define UNUSED_ATTR __attribute__((unused))

/* 30720 = 30 * 1024 = 30k */
#define FF_AAC_BUFFER_LEN 30720
#define HI_FALSE 0
#define HI_TRUE 1

typedef enum {
    HI_FFDEMUX_FORMAT_MP4 = 0,
    HI_FFDEMUX_FORMAT_TS,
    HI_FFDEMUX_FORMAT_WAV,
    HI_FFDEMUX_FORMAT_MP3,
    HI_FFDEMUX_FORMAT_AAC,
    HI_FFDEMUX_FORMAT_HLS,
    HI_FFDEMUX_FORMAT_BUTT
} FfDemuxFormat;

typedef struct {
    pthread_mutex_t mutex;
    char filePath[PATH_MAX + 1];
    int64_t fileSize;

    AVFormatContext *formatContext;
    AVPacket *pkt;
    int64_t lastReadPts; /* unit AV_TIME_BASE */
    const char *formatName;
    FfDemuxFormat formatType;

    AVStream *audioStream;
    enum AVCodecID audioCodeId;
    int32_t audioStreamIndex;
    uint32_t sampleRate;
    uint32_t chnnum;
    uint8_t aacBuf[FF_AAC_BUFFER_LEN]; /* store aac contains adts header */
    AVPacket *pktBak;
    bool cached;
    int64_t cacheLastPts;
} FfDemuxContext;

int32_t FfOpen(FfDemuxContext *demux, const char *fileName, uint32_t fileNameLen);
void FfClean(FfDemuxContext *demux);
void FfClose(FfDemuxContext *demux);
int32_t FfFreePacket(const FfDemuxContext *demux, AVPacket *pkt);
int32_t FfProbe(FfDemuxContext *demux);
void FfDiscardStream(AVStream *stream);
int32_t FfProbeAudio(FfDemuxContext *demux, bool *hasAudio);
int32_t FfGetinfo(const FfDemuxContext *demux, MediaFileInfo *fmtInfo);
bool FfHasAudio(const FfDemuxContext *demux);
int32_t FfReadPacket(const FfDemuxContext *demux, AVPacket *pkt);
void FfGenFrameByAvPacket(FfDemuxContext *demux, FormatPacket *fmtFrame);
int32_t FfSeek(FfDemuxContext *demux, int32_t streamIndex, int64_t toMs, uint32_t flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FF_WRAPPER_H */
