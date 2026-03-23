/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodView
 * Create: 2025-06
 */

#ifndef BLOOD_VIEW_H
#define BLOOD_VIEW_H

#include "View.h"
namespace OHOS {
enum BLOOD_PAGES {
    BLOOD_MAIN_PAGE = 1,  // 0 is invalid
    BLOOD_READY_PAGE = 2,
    BLOOD_MEASUREMENT_PAGE = 3,
    BLOOD_RESULT_PAGE = 4,
    BLOOD_CHART_PAGE = 5,
    BLOOD_EXPLAIN_PAGE = 6,
};
class BloodPresenter;
class BloodView : public View<BloodPresenter> {};
}
#endif  // BLOOD_VIEW_H