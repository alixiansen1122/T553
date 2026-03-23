/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuMainItemView
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_MAIN_ITEM_VIEW_H
#define PHONE_MENU_MAIN_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    int iconResId;
    const char *label;
    const char *viewId;
} PhoneMenuItem;

class PhoneMenuMainItemView : public UIViewGroup {
public:
    PhoneMenuMainItemView();
    ~PhoneMenuMainItemView() override;
    virtual void SetItemInfo(const PhoneMenuItem &itemInfo);

private:
    UIImageView icon_;
    UILabel label_;
    UIViewGroup bg_;
};
}
#endif  // PHONE_MENU_MAIN_ITEM_VIEW_H