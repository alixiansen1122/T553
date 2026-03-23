/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathExerPage
 * Create: 2025-5-13
 */
#ifndef BREATH_EXER_PAGE_H
#define BREATH_EXER_PAGE_H

#include "layout/grid_layout.h"
#include "components/ui_label.h"
#include "components/ui_digital_clock.h"
#include "components/ui_image_animator.h"
#include "View.h"
#include "SlicePage.h"
#include "graphic_timer.h"
#include "SlicePageFactory.h"
#include "power_display_service.h"
#include "BreathModel.h"
#include "BreathPresenter.h"

namespace OHOS {
int32_t g_StartTime;
class BreathExerPage : public SlicePage<BreathPresenter>,
                                public UIView::OnClickListener,
                                public UIView::OnDragListener {
public:
    BreathExerPage();
    ~BreathExerPage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void SetUpImageAnimator();
    void UpdateTextOpa();
    void UpdateImageRotate();
    class BreathExerPageCallback : public AnimatorCallback {
    public:
        BreathExerPageCallback(BreathExerPage* page) : BreathExerPage(page) {}
        virtual ~BreathExerPageCallback() override {}
        void Callback(UIView* view) override;
    private:
        BreathExerPage* BreathExerPage{nullptr};
    };
    BreathExerPageCallback* animatorCallBack_{nullptr};
    Animator *animator_ = nullptr;
private:
    UIViewGroup *group_ = nullptr;
    UIImageView* mainImg_ = nullptr;
    UIImageView* romataImg_ = nullptr;
    UIImageView* romataImgVirtual_ = nullptr;
    UILabel *breathInhale_ = nullptr;
    UILabel *breathExhale_ = nullptr;
    float scale_ = 0;
    float angle_ = 0;
    bool flag_ = true;
    float rotateStep_ = 0.0f;
};
}  // namespace OHOS

#endif