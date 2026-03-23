/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathView
 * Create: 2025-5-14
 */

#ifndef BREATH_VIEW_H
#define BREATH_VIEW_H

#include "View.h"
namespace OHOS {
enum BREATH_PAGE {
    BREATH_MAIN_VIEW = 1,
    BREATH_DUR_VIEW = 2,
    BREATH_EXER_VIEW = 3,
    BREATH_RHY_VIEW = 4,
    BREATH_START_VIEW = 5,
    BREATH_RESULT_VIEW = 6,
    BREATH_TRAINING_VIEW = 7,
};

class BreathPresenter;
class BreathView : public View<BreathPresenter> {};
}
#endif  // BREATH_VIEW_H