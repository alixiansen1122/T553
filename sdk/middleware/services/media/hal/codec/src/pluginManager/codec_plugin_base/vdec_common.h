/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: video decoder comm
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifndef AV_VDEC_COMMON_H
#define AV_VDEC_COMMON_H

#include "codec_plugin_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_FF_LIB_NAME_LEN  1024

typedef struct {
    uint32_t len;          /* W; stream len */
    int64_t pts;          /* W; time stamp */
    bool endOfFrame;  /* W; is the end of a frame */
    bool endOfStream; /* W; is the end of all stream */
    bool display;     /* W; is the current frame displayed. only valid by VIDEO_MODE_FRAME */
    uint8_t *addr;         /* W; stream address */
} AvVdecStream;

typedef struct {
    uint32_t maxWidth;
    uint32_t maxHeight;
    uint32_t errRecover;
    uint32_t bufSize;
    uint32_t frameBufCnt;
    void *priv;
} AvVdecAttr;

#define VDEC_ERR_BASE  0x1000

#define VDEC_ERR_STREAM_BUF_FULL (VDEC_ERR_BASE + 1)

#define VDEC_ERR_FRAME_BUF_EMPTY (VDEC_ERR_BASE + 2)
#define VDEC_FRAME_BUF_EOS (VDEC_ERR_BASE + 3)

typedef struct {
    uint32_t streamBufsizes;     /* R; stream buf size */
    uint32_t leftStreamBytes;    /* R; left stream bytes waiting for decode */
    uint32_t leftStreamFrames;   /* R; left frames waiting for decode,only valid for H264D_MODE_FRAME */
    uint32_t leftPics;           /* R; pics waiting for output */
    bool startRecvStream;   /* R; had started recv stream? */
    uint32_t recvStreamFrames;   /* R; how many frames of stream has been received. valid when send by frame. */
    uint32_t decodeStreamFrames; /* R; how many frames of stream has been decoded. valid when send by frame. */
} AvVdecStatus;

typedef enum {
    AV_VDEC_CMD_BUTT,
} AvVdecCmd;

typedef struct {
    CodecPluginCommon pluginCommon;
    int32_t (*pfnQueueInput)(void *vdecHdl, const InputInfo *vdecStream, uint32_t timeout);
    int32_t (*pfnDequeueOutput)(void *vdecHdl, OutputInfo *outInfo, uint32_t timeout, int *acquireFd);
    int32_t (*pfnQueueOutput)(void *vdecHdl, OutputInfo *outInfo, int releaseFenceFd);
    int32_t (*pfnGetStatus)(void *vdecHdl, AvVdecStatus *vdecStatus);
} AvVideoDecoder;

void* VDecoderGetImpl(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AV_VDEC_COMMON_H */
