/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateView
 * Create: 2025-03-23
 */

#ifndef DATE_VIEW_H
#define DATE_VIEW_H

#include "View.h"
namespace OHOS {
enum DATE_PAGES {
    DATE_MAIN_PAGE = 1,  // 0 is invalid
    DATE_MESSAGE_PAGE = 2,
};

class DatePresenter;
class DateView : public View<DatePresenter> {};
}
#endif // Date_VIEW_H
