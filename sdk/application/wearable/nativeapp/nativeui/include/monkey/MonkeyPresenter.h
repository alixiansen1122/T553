/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MonkeyPresenter
 * Create: 2025-04-24
 */
#ifndef MONKEY_PRESENTER_H
#define MONKEY_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
class MonkeyView;
class MonkeyPresenter : public Presenter<MonkeyView> {};
}

#endif  // MONKEY_PRESENTER_H