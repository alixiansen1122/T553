/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FindPhonePresenter
 * Create: 2025-03-23
 */

#include "NativeRegisterManager.h"
#include "uiservice/ui_service.h"
#include "findphone/FindPhoneModel.h"
#include "findphone/FindPhoneView.h"
#include "findphone/FindPhonePresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_FIND_PHONE, FindPhoneView, FindPhonePresenter, FINDPHONE_ICON, FINDPHONE_ICON, "查找手机");
}
