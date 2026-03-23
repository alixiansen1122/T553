/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRatePresenter
 * Create: 2025-06
 */

#include "uiservice/ui_service.h"
#include "NativeRegisterManager.h"
#include "wearable_log.h"
#include "heartrate/HeartRateView.h"
#include "heartrate/HeartRatePresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_HEARTRATE, HeartRateView, HeartRatePresenter, HEART_RATE_ICON, HEART_RATE_ICON, "心率");
static HeartRatePresenter *g_pHeartRatePresenter = nullptr;
HeartRatePresenter *HeartRatePresenter::GetInstance()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRatePresenter GetInstance");
    return g_pHeartRatePresenter;
}
}
