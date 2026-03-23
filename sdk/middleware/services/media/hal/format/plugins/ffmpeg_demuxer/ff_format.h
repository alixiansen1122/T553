/*
 * Copyright (c) @CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ffmpeg foramt function
 * Author: Media Software Group
 * Create: 2025-08-08
 */

#ifndef FF_FORMAT_H
#define FF_FORMAT_H

#include "ff_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

int32_t FfFormatFree(FfDemuxContext *demux, const FormatPacket *fmtFrame);
void FfFormatClose(FfDemuxContext *demux);
int32_t FfFormatProbe(FfDemuxContext *demux);
int32_t FfFormatRead(FfDemuxContext *demux, FormatPacket *fmtFrame);
int32_t FfFormatSeek(FfDemuxContext *demux, int32_t streamIndex, int64_t toMs, uint32_t flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FF_FORMAT_H */
