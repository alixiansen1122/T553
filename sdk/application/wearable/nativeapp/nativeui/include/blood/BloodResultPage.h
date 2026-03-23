/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodResultPage
 * Create: 2025-06
 */

#ifndef BLOOD_RESULT_PAGE_H
#define BLOOD_RESULT_PAGE_H

#include "View.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "ui_test.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "BloodView.h"
#include "BloodModel.h"
#include "BloodPresenter.h"

namespace OHOS {
class BloodPresenter;
class BloodModel;
class BloodResultPage : public SlicePage<BloodPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BloodResultPage();
    ~BloodResultPage() override;
    static BloodResultPage *GetInstance();
    void OnStart(void *data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
private:
    UIViewGroup *group_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelData_ = nullptr;
    UILabel *labelBloodOxygen_ = nullptr;
    UILabelButton *buttonMore_ = nullptr;
    UIImageView *mainImg_ = nullptr;
    UIImageView *levelImg_ = nullptr;
};
}
#endif // BLOOD_RESULT_PAGE_H
