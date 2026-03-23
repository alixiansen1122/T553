/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindIntervalItemView
 * Create: 2025-06-06
 */

#ifndef SPORT_REMIND_INTERVAL_ITEM_VIEW_H
#define SPORT_REMIND_INTERVAL_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    uint8_t itemId;
    int iconId;
    const char *label;
    const char *viewId;
} SportRemindIntervalItem;

class SportRemindIntervalItemView : public UIViewGroup {
public:
    SportRemindIntervalItemView();
    ~SportRemindIntervalItemView() override;

    virtual void SetItemInfo(const SportRemindIntervalItem &itemInfo);

private:
    UIImageView bgImage_;
    UIImageView icon_;
    UILabel label_;
    UIImageView nextImage_;
};
}
#endif