/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodPresenter
 * Create: 2025-06
 */

#ifndef BLOOD_PRESENTER_H
#define BLOOD_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"

namespace OHOS {
class BloodView;
class BloodPresenter : public Presenter<BloodView> {};
}
#endif  // BLOOD_PRESENTER_H