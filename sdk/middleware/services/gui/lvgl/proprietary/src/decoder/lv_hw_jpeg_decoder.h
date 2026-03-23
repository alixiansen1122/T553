/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_HW_JPEG_DECODER_H
#define LV_HW_JPEG_DECODER_H

#include "lv_conf.h"

#if LV_USE_IMG_EXT & LV_USE_HW_JPEG_DECODER

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Register hardware jpeg decoder in LVGL
 */
void LvHwJpegDecoderInit(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_USE_IMG_EXT & LV_USE_HW_JPEG_DECODER */

#endif /* LV_HW_JPEG_DECODER_H */