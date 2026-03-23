/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightPage
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_PAGE_H
#define FLASHLIGHT_PAGE_H

#include <string>
#include "View.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_list.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "components/ui_image_view.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "SlicePage.h"
#include "FlashLightPresenter.h"
#include "animator.h"

namespace OHOS {
class FlashLightPage : public SlicePage<FlashLightPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    FlashLightPage();
    ~FlashLightPage();

    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    class TimeAnimatorCallback : public AnimatorCallback {
    public:
        TimeAnimatorCallback() {}
        virtual ~TimeAnimatorCallback() override {}
        void Callback(UIView* view) override;
    };
    TimeAnimatorCallback* timeCallback_ = nullptr;
    Animator* timeAnimator_ = nullptr;
    UILabelButton* flashLightButtonOn_ = nullptr;
    UILabel* systemTime_ = nullptr;
    UILabel* tipsLabel_ = nullptr;
    UIImageView* imageArrow_ = nullptr;
    UIScrollView *container_ = nullptr;
};
}
#endif  // FLASHLIGHT_PAGE_H
