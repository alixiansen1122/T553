/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_BARCODE_H
#define LV_DEMO_BARCODE_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_BARCODE

void LvDemoBarcode(lv_obj_t* par);
void LvRegisterDemoBarcode(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif