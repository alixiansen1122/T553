/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: wav_muxer define
* Author: Media Software Group
* Create: 2025-03-10
*/

#ifndef WAV_MUXER_H
#define WAV_MUXER_H

#include "muxer_interface.h"
#ifdef MELODY_DEMO_BMIC_TX
#include "feature_config.h"
#endif
#if (defined(CONFIG_SUPPORT_FILESYSTEM) && defined(PRE_ASIC)) || defined(PRE_FPGA)
#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const MuxerPluginEntry *GetMuxer(void);
#else
extern MuxerPluginEntry g_wavMuxerEntry;
#endif

#ifdef __cplusplus
}
#endif
#endif
#endif
