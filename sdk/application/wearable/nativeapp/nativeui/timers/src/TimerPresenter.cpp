/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerPresenter
 * Create: 2025-06-09
 */

#include "ohos_timer.h"
#include "NativeRegisterManager.h"
#include "timers/TimerView.h"
#include "timers/TimerModel.h"
#include "timers/TimerPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_TIMER, TimerView, TimerPresenter, TIMER_ICON, TIMER_ICON, "计时器");
}