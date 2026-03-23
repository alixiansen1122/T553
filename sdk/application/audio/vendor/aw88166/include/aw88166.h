/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: header file for AW88166 driver adapt
 */

#ifndef __AW88166_H__
#define __AW88166_H__

#include "soc_uapi_vendor.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_s32 audio_aw88166_init(const uapi_vendor_codec_attr *attr);
td_s32 audio_aw88166_deinit(td_void);

td_s32 audio_aw88166_start(const uapi_vendor_codec_attr *attr);
td_s32 audio_aw88166_stop(td_void);

td_s32 audio_aw88166_set_aef_profile(uapi_snd_aef_profile aef_profile);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AW88166_H__ */
