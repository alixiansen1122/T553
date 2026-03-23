/*
 * Copyright (c) @CompanyNameMagicTag. 2020-2021. All rights reserved.
 * Description: ZPlayer adapter is used to adapt AOSP native player framework
 * Author: Media Software Group
 * Create: 2020-10-23
 */

#ifndef DEMUXER_MANAGER_H
#define DEMUXER_MANAGER_H

#include <stdint.h>
#include <format_type.h>

void DemuxerManagerInit(void);
void DemuxerManagerDeInit(void);
int32_t DemuxerCreate(const FormatSource *source, FormatHandle * const handle);
int32_t DemuxerSetParameter(const FormatHandle handle, int32_t trackId, const ParameterItem *metaData,
    int32_t metaDataCnt);
int32_t DemuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *metaData);
int32_t DemuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack);
int32_t DemuxerSetBufferConfig(const FormatHandle handle, const FormatBufferSetting *setting);
int32_t DemuxerGetBufferConfig(const FormatHandle handle, FormatBufferSetting *setting);
int32_t DemuxerPrepare(const FormatHandle handle);
int32_t DemuxerGetFileInfo(const FormatHandle handle, FileInfo *info);
int32_t DemuxerSelectTrack(const FormatHandle handle, int32_t programId, int32_t trackId);
int32_t DemuxerUnselectTrack(const FormatHandle handle, int32_t programId, int32_t trackId);
int32_t DemuxerStart(const FormatHandle handle);
int32_t DemuxerGetSelectedTrack(const FormatHandle handle, int32_t *programId, int32_t trackId[], int32_t *nums);
int32_t DemuxerReadFrame(const FormatHandle handle, FormatFrame *frame, int32_t timeOutMs);
int32_t DemuxerFreeFrame(const FormatHandle handle, FormatFrame *frame);
int32_t DemuxerSeek(const FormatHandle handle, int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode);
int32_t DemuxerStop(const FormatHandle handle);
int32_t DemuxerDestroy(const FormatHandle handle);
#endif  // DEMUXER_MANAGER_H