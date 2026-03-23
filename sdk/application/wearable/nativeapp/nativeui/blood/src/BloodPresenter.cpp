/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodPresenter
 * Create: 2025-06
 */

#include "uiservice/ui_service.h"
#include "NativeRegisterManager.h"
#include "blood/BloodView.h"
#include "blood/BloodModel.h"
#include "blood/BloodPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_BLOOD, BloodView, BloodPresenter, BLOOD_ICON, BLOOD_ICON, "血氧");
}
