/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightSetItemView
 * Create: 2025-05-18
 */

#ifndef FLASHLIGHTSET_ITEM_VIEW_H
#define FLASHLIGHTSET_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    uint32_t resId;
    std::string label;
    std::string itemLabel;
    std::string viewId;
} FlashLightSetItem;

class FlashLightSetItemView : public UIViewGroup {
public:
    FlashLightSetItemView();
    ~FlashLightSetItemView() override;
    virtual void SetItemInfo(const FlashLightSetItem itemInfo);

private:
    UIImageView sIcon_;
    UILabel sLabel_;
    UILabel itemLabel_;
    UIImageView sNextImage_;
    FlashLightSetItem itemInfo_ = {0, "", "", ""};
};
}
#endif  // FLASHLIGHTSET_ITEM_VIEW_H