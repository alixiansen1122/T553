/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateContentItemView
 * Create: 2025-03-23
 */

#ifndef DATE_CONTENT_ITEM_VIEW_H
#define DATE_CONTENT_ITEM_VIEW_H

#include "DateModel.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "components/ui_canvas.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_view_group.h"

namespace OHOS {
class DateContentItemView : public UIViewGroup {
public:
    explicit DateContentItemView();
    ~DateContentItemView() override;
    virtual void SetItemInfo(const DateItem itemInfo);
    const char *GetViewId();
    bool GetVisiable();

private:
    UILabel labelTitle_;
    UILabel labelDes_;
    UILabel labelAddress_;
    UIImageView signs_;
    UIButton backGround_;
    DateItem itemInfo_ = {nullptr, nullptr, nullptr, nullptr, false};
};
}

#endif
