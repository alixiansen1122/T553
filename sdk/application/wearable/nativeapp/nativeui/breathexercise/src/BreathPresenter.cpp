/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathPresenter
 * Create: 2025-5-13
 */
#include "AppViewIDs.h"
#include "NativeRegisterManager.h"
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_BREATH, BreathView, BreathPresenter, BREATH_ICON, BREATH_ICON, "呼吸训练");
}