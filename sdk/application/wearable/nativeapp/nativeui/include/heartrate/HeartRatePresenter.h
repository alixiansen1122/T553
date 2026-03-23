/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRatePresenter
 * Create: 2025-06
 */

#ifndef HEART_RATE_PRESENTER_H
#define HEART_RATE_PRESENTER_H

#include "components/ui_view.h"
#include "Presenter.h"

namespace OHOS {
class HeartRateView;
class HeartRatePresenter : public Presenter<HeartRateView> {
    HeartRatePresenter *GetInstance();
};
}

#endif  // HEART_RATE_PRESENTER_H