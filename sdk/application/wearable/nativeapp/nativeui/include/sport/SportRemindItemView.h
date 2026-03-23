/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindItemView
 * Create: 2025-06-06
 */

#ifndef SPORT_REMIND_ITEM_VIEW_H
#define SPORT_REMIND_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    uint8_t itemId;
    int iconId;
    const char *label;
    const char *viewId;
} SportRemindItem;

class SportRemindItemView : public UIViewGroup {
public:
    SportRemindItemView();
    ~SportRemindItemView() override;

    virtual void SetItemInfo(const SportRemindItem &itemInfo);

private:
    UIImageView bgImage_;
    UIImageView icon_;
    UILabel label_;
    UILabel hits_;
    UIImageView nextImage_;
};
}
#endif