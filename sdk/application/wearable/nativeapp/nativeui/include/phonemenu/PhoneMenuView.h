/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuView
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_VIEW_H
#define PHONE_MENU_VIEW_H
#include "View.h"
#include "PhoneMenuPresenter.h"

namespace OHOS {
enum PhoneMenuPages : uint8_t {
    PHONE_MENU_MAIN_PAGE = 1,
    PHONE_MENU_CONTACTS_PAGE,
    PHONE_MENU_DIAL_PAGE,
    PHONE_MENU_DISCONNECT_PAGE,
    PHONE_MENU_CALLER_LOG_PAGE,
    PHONE_MENU_CALLING_PAGE,
};

class PhoneMenuView : public View<PhoneMenuPresenter> {};
}
#endif  // PHONE_MENU_VIEW_H