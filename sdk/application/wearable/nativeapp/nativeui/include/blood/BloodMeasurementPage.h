/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodMeasurementPage
 * Create: 2025-06
 */

#ifndef BLOOD_MEASUREMENT_PAGE_H
#define BLOOD_MEASUREMENT_PAGE_H

#include <sys/time.h>
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_list.h"
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
class BloodMeasurementPage : public SlicePage<BloodPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BloodMeasurementPage();
    ~BloodMeasurementPage() override;
    void OnStart(void *data) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void OnResume() override;
    void OnPause() override;
    void InitImageAnimator();
    void InitAnimator();
    void SetLabelTime(std::string str);
    void StopAnimator();
    void ShowOnMeasurement();
    void StartAnimator();
private:
    UIViewGroup *group_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelTips_ = nullptr;
    UILabel *labelTime_ = nullptr;
    UILiteSurfaceView *surfaceView_ =  nullptr;
    MediaVideoPlay *videoPlay_ = nullptr;
    ColorType colorKey_;
    struct timeval startTime_;
    class BloodAnimatorCallback : public AnimatorCallback {
    public:
        explicit BloodAnimatorCallback(BloodMeasurementPage *page) : page_(page) {}
        ~BloodAnimatorCallback() override {}
        void Callback(UIView *view) override;
    private:
        BloodMeasurementPage *page_{nullptr};
    };
    Animator *animator_ = nullptr;
    BloodAnimatorCallback *callBack_ = nullptr;
};
}
#endif // BLOOD_MEASUREMENT_PAGE_H