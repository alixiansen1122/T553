/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchItemView
 * Create: 2025-06-06
 */

#ifndef STOPWATCH_ITEM_VIEW_H
#define STOPWATCH_ITEM_VIEW_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"

namespace OHOS {
typedef struct {
    uint8_t index;
    std::string record;
    bool isLatest;
} StopwatchItem;

class StopwatchItemView : public UIViewGroup {
public:
    explicit StopwatchItemView();
    virtual ~StopwatchItemView() override;
    void SetItemInfo(const StopwatchItem &itemInfo);

private:
    UILabel nameLabel_;
    UILabel recordLabel_;
    UILabelButton itemBg_;
};
}

#endif
