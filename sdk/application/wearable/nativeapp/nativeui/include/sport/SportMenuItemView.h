/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportMenuItemView
 * Create: 2025-06-06
 */

#ifndef SPORT_MENU_ITEM_VIEW_H
#define SPORT_MENU_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    int bgId;
    int iconId;
    const char *label;
    const char *viewId;
} SportMenuItem;

class SportMenuItemView : public UIViewGroup {
public:
    SportMenuItemView();
    ~SportMenuItemView() override;

    virtual void SetItemInfo(const SportMenuItem &itemInfo);

private:
    UIImageView bgImage_;
    UIImageView icon_;
    UILabel label_;
    UIImageView settingImage_;
};
}
#endif