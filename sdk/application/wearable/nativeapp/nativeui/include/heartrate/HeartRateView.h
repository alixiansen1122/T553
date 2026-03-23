/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateView
 * Create: 2025-06
 */

#ifndef HEART_RATE_ITEM_VIEW_H
#define HEART_RATE_ITEM_VIEW_H

#include "View.h"

namespace OHOS {
enum HeartRateAllView {
    HEARTRATE_MAIN_PAGE = 1,  // 0 is invalid
    HEARTRATE_READY_PAGE = 2,
    HEARTRATE_MEASUREMENT_PAGE = 3,
};

class HeartRatePresenter;
class HeartRateView : public View<HeartRatePresenter> {};
}
#endif  // HEART_RATE_ITEM_VIEW_H
