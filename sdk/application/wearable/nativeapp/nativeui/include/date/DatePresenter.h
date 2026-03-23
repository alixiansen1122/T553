/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DatePresenter
 * Create: 2025-03-23
 */

#ifndef DATE_PRESENTER_H
#define DATE_PRESENTER_H

#include "DateModel.h"
#include "Presenter.h"
#include "components/ui_view.h"
namespace OHOS {

class DateView;
class DatePresenter : public Presenter<DateView> {};
}

#endif // DATE_PRESENTER_H