/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Muxer Manager interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef MUXER_MANAGER_H
#define MUXER_MANAGER_H

#include "format_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void MuxerManagerInit(void);
void MuxerManagerDeInit(void);
int32_t MuxerCreate(FormatHandle * const handle, const FormatOutputConfig *outputConfig);
int32_t MuxerDestroy(const FormatHandle handle);
int32_t MuxerAddTrack(const FormatHandle handle, const TrackSource *trackSource);
int32_t MuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack);
int32_t MuxerSetOrientation(const FormatHandle handle, int degrees);
int32_t MuxerSetLocation(const FormatHandle handle, int latitude, int longitude);
int32_t MuxerSetMaxFileSize(const FormatHandle handle, int64_t bytes);
int32_t MuxerSetMaxFileDuration(const FormatHandle handle, int64_t durationUs);
int32_t MuxerSetFileSplitDuration(const FormatHandle handle, ManualSplitType type, int64_t timestampUs,
    uint32_t durationS);
int32_t MuxerStart(const FormatHandle handle);
int32_t MuxerWriteFrame(const FormatHandle handle, const FormatFrame *frameData);
int32_t MuxerSetNextOutputFile(const FormatHandle handle, int32_t fd);
int32_t MuxerStop(const FormatHandle handle, bool block);
int32_t MuxerSetParameter(const FormatHandle handle, int32_t trackId, const ParameterItem *item, int32_t itemNum);
int32_t MuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *item, int32_t itemNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // MUXER_MANAGER_H
