/*
 * Copyright (c) @CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: buffer cache for online play
 * Author: Media Software Group
 * Create: 2025-6-20
 */

#ifndef DEMUXER_BUFFER_H
#define DEMUXER_BUFFER_H

#include <stdint.h>
#include "format_type.h"
#include "demuxer_interface.h"

FormatHandle CreateDemuxerBuffer(void);
int32_t SetBufferConfig(FormatHandle handle, const FormatBufferSetting *setting);
int32_t GetBufferConfig(FormatHandle handle, FormatBufferSetting *setting);
int32_t StartBufferThread(FormatHandle handle, FormatHandle demuxerHdl,
    const DemuxerPluginEntry *demuxer, const char *url);
int32_t ReadBufferFrame(FormatHandle handle, FormatFrame *frame);
int32_t FreeBufferFrame(FormatFrame *frame);
int32_t SeekBuffer(FormatHandle handle, int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode);
int32_t StopBufferThread(FormatHandle handle);
int32_t DestoryDemuxerBuffer(FormatHandle *handle);

#endif
