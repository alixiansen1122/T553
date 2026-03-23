/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepView
 * Created: 2025-06-05
 */

#ifndef SLEEP_VIEW_H
#define SLEEP_VIEW_H

#include "View.h"
namespace OHOS {
enum SLEEPING_PAGES {
    SLEEPING_MAIN_PAGE = 1, // 0 is invalid
    SLEEPING_DEATIL_PAGE = 2,
    SLEEPING_TARGET_PAGE = 3,
    SLEEPING_SLEEP_PAGE = 4,
};
class SleepPresenter;
class SleepView : public View<SleepPresenter> {
};
}
#endif // SLEEP_VIEW_H