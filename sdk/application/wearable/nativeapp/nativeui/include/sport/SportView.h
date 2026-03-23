/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportView
 * Create: 2025-06-06
 */

#ifndef SPORT_VIEW_H
#define SPORT_VIEW_H

#include "View.h"

namespace OHOS {
enum SportPages : uint8_t {
    SPORT_PAGE_MENU = 1,
    SPORT_PAGE_PREPARE,
    SPORT_PAGE_COUNTDOWN,
    SPORT_PAGE_DATA,
    SPORT_PAGE_PAUSE,
    SPORT_PAGE_PROMPT,
    SPORT_PAGE_SETTINGS,
    SPORT_PAGE_TARGET,
    SPORT_PAGE_TARGET_SETTING,
    SPORT_PAGE_TARGET_PROMPT,
    SPORT_PAGE_REMIND,
    SPORT_PAGE_REMIND_INTERVAL,
    SPORT_PAGE_REMIND_HEARTRATE,
    SPORT_PAGE_HEARTRATE_RANGE,
    SPORT_PAGE_MISTOUCH,
    SPORT_PAGE_MAX,
};

class SportPresenter;
class SportView : public View<SportPresenter> {};
}
#endif  // SPORT_VIEW_H
