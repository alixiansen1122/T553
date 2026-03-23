/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathTimeSetPage
 * Create: 2025-5-13
 */
#ifndef BREATH_TIME_SET_PAGE_H
#define BREATH_TIME_SET_PAGE_H

#include <string>
#include "components/ui_label.h"
#include "components/ui_picker.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label_button.h"
#include "components/ui_toggle_button.h"
#include "View.h"
#include "UiConfig.h"
#include "SlicePage.h"
#include "SlicePageFactory.h"
#include "BreathModel.h"
#include "BreathPresenter.h"

namespace OHOS {
class BreathTimeSetPage : public SlicePage<BreathPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BreathTimeSetPage();
    ~BreathTimeSetPage() override;
    void OnStart(void* data) override;
    void OnResume() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
private:
    UILabel *labelTitle_ = nullptr;
    UIPicker *pickerTime_ = nullptr;
    UIButton *buttonOk_ = nullptr;
    UIViewGroup *group_ = nullptr;
    UIImageView *pickerImg_ = nullptr;
};
}  // namespace OHOS

#endif