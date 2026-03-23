/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateMeasurePage
 * Create: 2025-06
 */

#ifndef HEART_RATE_MEASURE_PAGE_H
#define HEART_RATE_MEASURE_PAGE_H

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
#include "View.h"
#include "ui_test.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "video_play_wrapper.h"
#include "HeartRateModel.h"
#include "HeartRatePresenter.h"
#include "HeartRateView.h"
#include "HeartRateResultGroup.h"

namespace OHOS {
class HeartRatePresenter;
class HeartRateMeasurePage : public SlicePage<HeartRatePresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    HeartRateMeasurePage();
    ~HeartRateMeasurePage() override;
    void OnStart(void *data) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void InitAnimator();
    void StopAnimator();
    void InitVideoView();
    void SetTimeAndResultText(std::string str);
    void SetTestStatus(bool value);
    void OnResume() override;
    void OnPause() override;
private:

    // 心率线型图结果展示页面，通过setvisible进行显示隐藏操作
    // 作用：线型图退出之后不会重新进入心率测量状态
    HeartRateResultGroup *chartLineResultGroup_ = nullptr;
    UIViewGroup *group_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelTimeOrResult_ = nullptr;
    UILabel *labelTips_ = nullptr;
    UILiteSurfaceView *surfaceView_ =  nullptr;
    MediaVideoPlay *videoPlay_ = nullptr;
    ColorType colorKey_;
    struct timeval startTime_;
    bool measureStatus_;
    class HeartAnimatorCallback : public AnimatorCallback {
    public:
        explicit HeartAnimatorCallback(HeartRateMeasurePage *page) : page_(page) {}
        ~HeartAnimatorCallback() override {}
        void Callback(UIView *view) override;
    private:
        HeartRateMeasurePage *page_{nullptr};
    };
    Animator *animator_ = nullptr;
    HeartAnimatorCallback *callBack_ = nullptr;
};
}
#endif // HEART_RATE_MEASURE_PAGE_H