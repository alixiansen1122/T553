/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_BARCODE_H
#define LV_BARCODE_H

#include "lvgl.h"
#include "lv_img_buf.h"
#include "lv_img_ext.h"
#include "misc/lv_area.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    LvImgExt img;
    lv_img_dsc_t dsc;
} LvBarcode;

/**
 * @brief  Create a empty barcode object.
 * @param  parent    point to an object where to create the QR code
 * @return pointer to the created QR code object
 */
lv_obj_t* LvBarcodeCreate(lv_obj_t* parent);

/**
 * @brief  Update the data of a barcode object
 * @param  obj    point to a barcode object
 * @param  data    data to display
 * @param  width    barcode width
 * @param  height    barcode height
 * @return whether this operation is executed successfully
 */
bool LvBarcodeUpdate(lv_obj_t* obj, const char* data, lv_coord_t width, lv_coord_t height);
#ifdef __cplusplus
}
#endif
#endif