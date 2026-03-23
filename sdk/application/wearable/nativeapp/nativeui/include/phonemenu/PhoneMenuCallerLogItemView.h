/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallerLogItemView
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_CALLER_LOG_ITEM_VIEW_H
#define PHONE_MENU_CALLER_LOG_ITEM_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "phoneservice/PhoneService.h"

namespace OHOS {
class PhoneMenuCallerLogItemView : public UIViewGroup {
public:
    PhoneMenuCallerLogItemView();
    ~PhoneMenuCallerLogItemView() override;
    void SetItemInfo(const CallLogInfo &itemInfo);
    unsigned char *GetItemPhoneNumber();

private:
    UIViewGroup bg_;
    UIImageView icon_;
    UILabel phoneLabel_;
    UILabel stutasLabel_;
    unsigned char tel_[MAX_PHONE_NUM];
};
}
#endif
