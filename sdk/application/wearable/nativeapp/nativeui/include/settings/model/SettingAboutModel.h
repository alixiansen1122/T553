/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingAboutModel
 * Create: 2025-06-01
 */
#ifndef SETTING_ABOUT_MODEL_H
#define SETTING_ABOUT_MODEL_H

#include <cstdint>
#include <string>
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
class SettingAboutModel : public SettingRefSingleton<SettingAboutModel> {
public:
    std::string GetSystemName();
    std::string GetSystemType();
};
} // OHOS
#endif