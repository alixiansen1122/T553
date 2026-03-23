/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateButtonItemView
 * Create: 2025-03-23
 */

#ifndef DATE_BUTTON_ITEM_VIEW_H
#define DATE_BUTTON_ITEM_VIEW_H

#include "DateModel.h"
#include "UiConfig.h"
#include "components/ui_canvas.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "main/LoadImg.h"

namespace OHOS {
class DateButtonItemView : public UIViewGroup {
public:
    explicit DateButtonItemView();
    ~DateButtonItemView() override;
    virtual void SetItemInfo(const DateButtonItem itemInfo);
    DateButtonItem GetItem();

private:
    UILabelButton labelButton_;
    DateButtonItem itemInfo_ = {"", nullptr, false, false};
};
}

#endif
