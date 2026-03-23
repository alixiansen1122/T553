/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateMessagePage
 * Create: 2025-03-15
 */

#ifndef DATE_MESSAGE_H
#define DATE_MESSAGE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_swipe_view.h"
#include "ui_swipe_view.h"
#include "DatePresenter.h"
#include "DateModel.h"
#include "DateView.h"
#include "SlicePage.h"
#include "UiConfig.h"

namespace OHOS {
class DatePresenter;
class DateMessagePage : public SlicePage<DatePresenter>,
                        public UIView::OnDragListener {
public:
    DateMessagePage();
    ~DateMessagePage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void InitLabel();

private:
    UIScrollView *group_{nullptr};
    UIImageView *bgImage_{nullptr};
    UILabel *labelTitle_{nullptr};
    UILabel *labelMessage_{nullptr};
    UILabel *labelAddress_{nullptr};
    UILabel *labelTime_{nullptr};
};
}
#endif