/*
* Copyright (c) @CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: ffmpeg demuxer header
* Author: Media Software Group
* Create: 2025-08-08
*/

#ifndef FF_DEMUXER_PLUGIN_H
#define FF_DEMUXER_PLUGIN_H

#include "demuxer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef void *VoidPtr;

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const DemuxerPluginEntry *GetDemuxer(void);
#else
extern DemuxerPluginEntry g_ffDemuxerEntry;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* FF_DEMUXER_PLUGIN_H */
