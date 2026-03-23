/*
* Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
* Description: raw format
* Author: Media Software Group
* Create: 2022-12-10
*/

#ifndef RAW_FORMATH_H
#define RAW_FORMATH_H

#include "demuxer_common.h"
#include "demuxer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const DemuxerPluginEntry* GetDemuxer(void);
#else
extern DemuxerPluginEntry g_rawFormatDemuxerEntry;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* RAW_FORMATH_H */
