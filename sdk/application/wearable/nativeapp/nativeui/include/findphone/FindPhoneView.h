/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FindPhoneView
 * Create: 2025-03-23
 */

#ifndef FINDPHONE_VIEW_H
#define FINDPHONE_VIEW_H

#include "View.h"
namespace OHOS {
enum FIND_PHONE_PAGES {
    FIND_PHONE_MAIN_PAGE = 1,  // 0 is invalid
};

class FindPhonePresenter;
class FindPhoneView : public View<FindPhonePresenter> {};
}
#endif // FINDPHONE_VIEW_H
