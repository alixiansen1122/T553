/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepPresenter
 * Created: 2025-06-05
 */

#include "NativeRegisterManager.h"
#include "sleep/SleepView.h"
#include "sleep/SleepPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_SLEEPING, SleepView, SleepPresenter, SLEEP_ICON, SLEEP_ICON, "睡眠");
}