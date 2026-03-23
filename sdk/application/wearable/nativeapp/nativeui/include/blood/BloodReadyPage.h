/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodReadyPage
 * Create: 2025-06
 */

#ifndef BLOOD_READY_PAGE_H
#define BLOOD_READY_PAGE_H

#include <sys/time.h>
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_circle_progress.h"
#include "components/ui_image_animator.h"
#include "components/ui_lite_surface_view.h"
#include "layout/grid_layout.h"
#include "animator/animator.h"
#include "ChangeSliceListener.h"
#include "video_play_wrapper.h"
#include "View.h"
#include "ui_test.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "BloodModel.h"
#include "BloodPresenter.h"
#include "BloodView.h"


namespace OHOS {
class BloodPresenter;
class BloodModel;
class BloodReadyPage : public SlicePage<BloodPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BloodReadyPage();
    ~BloodReadyPage() override;
    void OnStart(void *data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void InitAnimator();
    void SetLabelProgressTime(int value);
    void StopAnimator();
    void ShowOnMeasurement();
    int InitCircleProgress();
    
private:
    UIViewGroup *group_ = nullptr;
    UILabel *labelTips_ = nullptr;
    UILabelButton *buttonStart_ = nullptr;
    UICircleProgress *timeProgress_ = nullptr;
    UILabel *labelProgressTime_ = nullptr;
    class BloodAnimatorCallback : public AnimatorCallback {
    public:
        explicit BloodAnimatorCallback(BloodReadyPage *page) : page_(page) {}
        ~BloodAnimatorCallback() override {}
        void Callback(UIView *view) override;

    private:
        BloodReadyPage *page_{nullptr};
    };
    struct timeval startTime_;
    Animator *animator_ = nullptr;
    BloodAnimatorCallback *callBack_ = nullptr;
};
}
#endif // BLOOD_READY_PAGE_H
