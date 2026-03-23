/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_FILE_DECODER_H
#define LV_FILE_DECODER_H

#include "lv_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_IMG_EXT
typedef struct {
    uint32_t colorMode : 8;
    uint32_t version : 4;
    uint32_t compressMode : 4;
    uint32_t reserved : 16;
    uint16_t width;
    uint16_t height;
} LvImgHeaderExt;

enum LvColorModeExt {
    LV_ARGB8888,
    LV_RGB888,
    LV_RGB565,
};

/**
 * @brief  Register the file decoder functions in LVGL
 */
void LvFileDecoderInit(void);

#endif /* LV_USE_IMG_EXT */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_FILE_DECODER_H */
