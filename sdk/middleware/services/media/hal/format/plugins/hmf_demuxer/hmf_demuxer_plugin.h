/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: hmf demux header file
* Author: media_develop team
* Create: 2021-7-10
*/

#ifndef HMF_DEMUXER_PLUGIN_H
#define HMF_DEMUXER_PLUGIN_H

#include "demuxer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const DemuxerPluginEntry *GetDemuxer(void);
#else
extern  DemuxerPluginEntry g_hmfDemuxerEntry;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HMF_DEMUXER_PLUGIN_H */
