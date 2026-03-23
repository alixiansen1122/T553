/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: audio recorder muxer define
 * Author: Media Software Group
 * Create: 2025-03-15
 */

#ifndef AUDIO_RECORDER_MUXER_H
#define AUDIO_RECORDER_MUXER_H

#include <stdint.h>
#include "audio_base_type.h"
#include "format_type.h"
#include "format_interface.h"
#include "parameter_item.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *AudioRecorderMuxer;

typedef enum {
    ERROR_TYPE_CREATE_FILE_FAIL,
    ERROR_TYPE_WRITE_FILE_FAIL,
    ERROR_TYPE_CLOSE_FILE_FAIL,
    ERROR_TYPE_INTERNAL_OPERATION_FAIL,
    EVENT_ERROR_BUTT,
} EventErrorType;

typedef enum {
    MAX_DURATION_APPROACHING,
    MAX_FILESIZE_APPROACHING,
    MAX_DURATION_REACHED,
    MAX_FILESIZE_REACHED,
    NEXT_OUTPUT_FILE_STARTED,
    FILE_SPLIT_FINISHED,
    FILE_START_TIME_MS,
    NEXT_FILE_FD_NOT_SET,
    DEMUXER_PREPARED = 10000,
    DEMUXER_SEEK_COMPLETE,
    DEMUXER_NETWORK_DISCONNECTED,
    DEMUXER_NETWORK_RECONNECTED,
    MAX_MEMORYSIZE_REACHED,
    EVENT_INFO_BUTT,
} EventInfoType;

typedef struct {
    void *handle;
    int32_t (*OnInfoCallback)(void *handle, EventInfoType type, int32_t ext);
    int32_t (*OnErrorCallback)(void *handle, EventErrorType type, int32_t ext);
} MuxerCallback;

int32_t AudioRecorderMuxerCreate(AudioRecorderMuxer *muxerHandle);
int32_t AudioRecorderMuxerSetOutputPath(AudioRecorderMuxer muxerHandle,
    AudioCodecFormat formatType, const char *outPath, const uint32_t outPathLen);
int32_t AudioRecorderMuxerAddTrack(AudioRecorderMuxer muxerHandle, const CapturerInputConfig *info);
int32_t AudioRecorderMuxerSetMaxDuration(AudioRecorderMuxer muxerHandle, int64_t duration);
int32_t AudioRecorderMuxerSetMaxFileSize(AudioRecorderMuxer muxerHandle, int64_t fileSize);
int32_t AudioRecorderMuxerSetCallBack(AudioRecorderMuxer muxerHandle, const MuxerCallback *callBack);
int32_t AudioRecorderMuxerPrepare(AudioRecorderMuxer muxerHandle);
int32_t AudioRecorderMuxerStart(AudioRecorderMuxer muxerHandle);
int32_t AudioRecorderMuxerWriteData(AudioRecorderMuxer muxerHandle, FormatFrame *frame);
int32_t AudioRecorderMuxerSetParameter(AudioRecorderMuxer muxerHandle, const ParameterItem *item, int32_t itemNum);
int32_t AudioRecorderMuxerGetParameter(AudioRecorderMuxer muxerHandle, ParameterItem *item, int32_t itemNum);
int32_t AudioRecorderMuxerStop(AudioRecorderMuxer muxerHandle);
int32_t AudioRecorderMuxerDestroy(AudioRecorderMuxer muxerHandle);

#ifdef __cplusplus
}
#endif

#endif