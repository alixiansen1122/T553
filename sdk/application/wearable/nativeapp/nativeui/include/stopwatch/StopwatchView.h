/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchView
 * Create: 2025-06-06
 */

#ifndef STOPWATCH_VIEW_H
#define STOPWATCH_VIEW_H

#include "View.h"
namespace OHOS {
enum STOPWATCH_PAGES {
    STOPWATCH_MAIN_PAGE = 1,
    STOPWATCH_RECORD_PAGE,
};

class StopwatchPresenter;
class StopwatchView : public View<StopwatchPresenter> {};
}
#endif  // STOPWATCH_VIEW_H
