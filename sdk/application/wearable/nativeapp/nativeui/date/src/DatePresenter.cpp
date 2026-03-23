/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DatePresenter
 * Create: 2025-03-23
 */

#include "NativeRegisterManager.h"
#include "uiservice/ui_service.h"
#include "wearable_log.h"
#include "date/DatePresenter.h"
#include "date/DateModel.h"
#include "date/DateView.h"

namespace OHOS {
REGIST_MENU(VIEW_DATE, DateView, DatePresenter, DATE_ICON, DATE_ICON, "日历");
}
