/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportHeartRateRangeItemView
 * Create: 2025-06-06
 */

#ifndef SPORT_HEART_RATE_RANGE_ITEM_VIEW_H
#define SPORT_HEART_RATE_RANGE_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_radio_button.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    uint8_t itemId;
    const char *title;
    const char *viewId;
} SportHeartRateRangeItem;

class SportHeartRateRangeItemView : public UIViewGroup {
public:
    SportHeartRateRangeItemView();
    ~SportHeartRateRangeItemView() override;

    virtual void SetItemInfo(const SportHeartRateRangeItem &itemInfo);

private:
    UIImageView bgImage_;
    UILabel title_;
    UILabel range_;
    UIRadioButton button_;
};
}
#endif