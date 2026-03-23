/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: raw_muxer define
* Author: Media Software Group
* Create: 2025-09-23
*/

#ifndef RAW_MUXER_H
#define RAW_MUXER_H

#include "muxer_interface.h"

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const MuxerPluginEntry *GetMuxer(void);
#else
extern MuxerPluginEntry g_rawMuxerEntry;
#endif

#endif
