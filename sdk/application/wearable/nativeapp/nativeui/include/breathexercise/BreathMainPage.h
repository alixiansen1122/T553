/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathMainPage
 * Create: 2025-5-13
 */
#ifndef BREATH_MAIN_PAGE_H
#define BREATH_MAIN_PAGE_H

#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_digital_clock.h"
#include "components/ui_image_animator.h"
#include "View.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "BreathView.h"
#include "BreathModel.h"
#include "BreathPresenter.h"
#ifndef _WIN32
#include "time64.h"
#endif

namespace OHOS {

class BreathMainPage : public SlicePage<BreathPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BreathMainPage();
    ~BreathMainPage() override;
    void OnStart(void* data) override;
    void OnResume() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;

private:
    UIViewGroup *group_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UIButton *buttonTime_ = nullptr;
    UIButton *buttonSetting_ = nullptr;
    UIButton *buttonStart_ = nullptr;
    UILabel *lableSetting_ = nullptr;
    UILabel *lableTime_ = nullptr;
    UIImageView *bgImg_ = nullptr;
    UIImageView *center_ = nullptr;
};
}  // namespace OHOS

#endif