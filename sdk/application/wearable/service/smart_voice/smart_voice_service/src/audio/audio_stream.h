/*
 * Copyright (c) CompanyNameMagicTag 2021. All rights reserved.
 * Description: audio stream api header.
 */

#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include "td_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef td_void (*ActInterruptCb)(td_s32 type, td_s32 hint);

td_s32 AudioStreamInit(td_void);

td_s32 AudioManagerActInterrupt(ActInterruptCb cb);

td_s32 AudioStreamCreate(td_void);

td_s32 AudioStreamStart(td_void);

td_s32 AudioStreamGetChannelId(td_u32 *channelId);

td_s32 AudioStreamStop(td_void);

td_s32 AudioStreamDestroy(td_void);

td_s32 AudioStreamDeactInterrupt(td_void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_STREAM_H