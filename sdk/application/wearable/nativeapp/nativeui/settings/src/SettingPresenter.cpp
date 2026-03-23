/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPresenter
 * Create: 2025-06-01
 */
#include "NativeRegisterManager.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingView.h"
#include "settings/SettingPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_SETTING, SettingView, SettingPresenter, SETTING_ICON, SETTING_ICON, "设置");

SettingPresenter::~SettingPresenter()
{
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(SETTING_IMAGE);
}
}
