/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingQrcodeConnModel
 * Created: 2025-06-05
 */

#include "settings/model/SettingQrcodeConnModel.h"

namespace OHOS {
SettingQrcodeConnModel::SettingQrcodeConnModel() {}

SettingQrcodeConnModel::~SettingQrcodeConnModel() {}

SettingQrcodeConnModel &SettingQrcodeConnModel::GetInstance()
{
    static SettingQrcodeConnModel instance;
    return instance;
}
}
