/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBluetoothItemGroup
 * Created: 2025-06-05
 */
#ifndef SETTING_BLUETOOTH_ITEM_GROUP_H
#define SETTING_BLUETOOTH_ITEM_GROUP_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_canvas.h"
#include "UiConfig.h"
#include "bts_def.h"
#include "bts_br_gap.h"
#include "settings/model/SettingBluetoothModel.h"

namespace OHOS {

class SettingBluetoothItemGroup : public UIViewGroup {
public:
    explicit SettingBluetoothItemGroup();
    ~SettingBluetoothItemGroup() override;
    void SetItemInfo(const BtCaseInfo* itemInfo);
    void SetUpChild();
    BtCaseInfo& GetItem();
private:
    UIImageView background_;
    UIImageView icon_;
    UIImageView conn_;
    UILabel deviceName_;
    unsigned char addr_[BD_ADDR_LEN];
    UILabel connectStatus_;
    uint32_t index_;
    int16_t temp_;
    BtCaseInfo itemInfo_ = {{}, {}, "", 0};
};
}

#endif
