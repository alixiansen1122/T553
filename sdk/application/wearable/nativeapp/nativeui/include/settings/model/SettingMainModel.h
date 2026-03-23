/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingMainModel
 * Create: 2025-06-01
 */
#ifndef SETTING_MAIN_MODEL_H
#define SETTING_MAIN_MODEL_H

#include <cstdint>
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
class SettingMainModel : public SettingRefSingleton<SettingMainModel> {
public:
    SettingMainModel();
    void InitSettingsData();
    std::vector<SettingUnionItemData>& GetMainUnionData();
    static UIView* OnSimpleListCreate(uint8_t type);
private:
    std::vector<SettingUnionItemData> mainUnionData_;
};
}
#endif
