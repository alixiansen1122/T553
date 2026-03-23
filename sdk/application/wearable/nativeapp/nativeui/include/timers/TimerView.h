/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerView
 * Create: 2025-06-09
 */

#ifndef TIMER_VIEW_H
#define TIMER_VIEW_H

#include "View.h"

namespace OHOS {
enum TIMER_PAGES {
    TIMER_MAIN_PAGE = 1,  // 主页面
    TIMER_SETTING_PAGE,   // 自定义页面
    TIMER_TIMING_PAGE,    // 显示计时页面
    TIMER_ALL_VIEW,
};

class TimerPresenter;
class TimerView : public View<TimerPresenter> {};
}
#endif  // TIMER_VIEW_H
