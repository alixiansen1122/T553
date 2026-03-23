/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_EXT_H
#define LV_DEMO_EXT_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* name;
    void (*Init)(lv_obj_t* par);
    void (*Deinit)(lv_obj_t* par);
} LvTestCaseInfo;

void RegisterTestCase(LvTestCaseInfo* info);

void LvDemoExt(void);

#ifdef __cplusplus
}
#endif

#endif