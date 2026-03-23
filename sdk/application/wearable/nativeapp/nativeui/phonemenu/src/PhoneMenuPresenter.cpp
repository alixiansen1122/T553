/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuPresenter
 * Create: 2025-07-12
 */

#include "NativeRegisterManager.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_PHONE_MENU, PhoneMenuView, PhoneMenuPresenter, PHONEMENU_ICON, PHONEMENU_ICON, "通话菜单");
}