/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: EmptyPresenter
 * Create: 2025-04-24
 */

#ifndef EMPTY_PRESENTER_H
#define EMPTY_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
class EmptyView;
class EmptyPresenter : public Presenter<EmptyView> {};
}

#endif  // EMPTY_PRESENTER_H