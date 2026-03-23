/*
* Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
* Description: raw format
* Author: media develop team
* Create: 2022-12-10
*/

#ifndef RAW_DEMUXER_INTF_H
#define RAW_DEMUXER_INTF_H

#include "demuxer_common.h"
#include "demuxer_interface.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    HalHandle (*Open)(const char *uri);
    int32_t (*Probe)(HalHandle handle, int32_t *score);
    int32_t (*FindStreamInfo)(HalHandle handle);
    int32_t (*GetInfo)(HalHandle handle, MediaFileInfo *info);
    int32_t (*ReadFrame)(HalHandle hdl, FormatPacket *frame);
    int32_t (*FreeFrame)(void *handle, FormatPacket *frame);
    int32_t (*Seek)(HalHandle handle, int64_t toMs, uint32_t flag);
    void (*Close)(HalHandle handle);
} DemuxerOpsIntf;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* RAW_DEMUXER_INTF_H */
