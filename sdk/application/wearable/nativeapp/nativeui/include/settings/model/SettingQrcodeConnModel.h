/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingQrcodeConnModel
 * Create: 2025-06-01
 */
#ifndef SETTING_QRCODE_CONN_MODEL_H
#define SETTING_QRCODE_CONN_MODEL_H

#include <cstdint>
#include <map>
#include "AppViewIDs.h"
#include "settings/common/SettingRefSingleton.h"
#include "UiConfig.h"

namespace OHOS {
class SettingQrcodeConnModel : public SettingRefSingleton<SettingQrcodeConnModel> {
public:
    static SettingQrcodeConnModel &GetInstance(void);
private:
    SettingQrcodeConnModel();
    SettingQrcodeConnModel(const SettingQrcodeConnModel &);
    SettingQrcodeConnModel &operator=(const SettingQrcodeConnModel &);
    virtual ~SettingQrcodeConnModel();
};
}

#endif  // SETTING_QRCODE_CONN_MODEL_H