/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerPresenter
 * Create: 2025-06-09
 */

#ifndef TIMER_PRESENTER_H
#define TIMER_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"

namespace OHOS {
class TimerView;
class TimerPresenter : public Presenter<TimerView> {};
}

#endif  // TIMER_PRESENTER_H