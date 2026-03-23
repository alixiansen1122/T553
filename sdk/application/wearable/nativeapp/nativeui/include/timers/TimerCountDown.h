/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerCountDown
 * Create: 2025-06-09
 */

#ifndef TIMER_COUNT_DOWN_H
#define TIMER_COUNT_DOWN_H

#include <sys/time.h>
#include "View.h"
#include "components/ui_button.h"
#include "components/ui_label.h"
#include "components/ui_fragment.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_image_view.h"
#include "components/ui_circle_progress.h"
#include "UiConfig.h"
#include "TimerModel.h"
#include "TimerPresenter.h"
#include "SlicePage.h"
#include "ChangeSliceListener.h"
#include "components/ui_sweep_clock.h"
#include "main/LoadImg.h"
#include "ui_resource_timer.h"

namespace OHOS {
class TimerCountDown : public UIFragment,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    void OnResume();
    void OnPause();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    void CreateButton();
    void CreateTimerTime();
    void CircleProgress();
    void SetTimerTime(int64_t durationTime);
    void InitAnimator();
    void SetAnimatorStatus(bool status);
    void RefreshFragment();

private:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    class TimeAnimatorCallback : public AnimatorCallback {
    public:
        TimeAnimatorCallback() {}
        virtual ~TimeAnimatorCallback() override {}
        void Callback(UIView *view) override;
    };

    class TimerCountDownCallback : public AnimatorCallback {
    public:
        explicit TimerCountDownCallback(TimerCountDown *page) : page_(page) {}
        ~TimerCountDownCallback() override {}
        void Callback(UIView *view) override;

    private:
        TimerCountDown *page_{nullptr};
    };

    UILabel *labelTitle_{nullptr};
    UIDigitalClock *timerClock_{nullptr};
    UIImageView *startOrPauseImg_{nullptr};
    UIImageView *resetOrExitImg_{nullptr};
    UICircleProgress *circleProgress_{nullptr};
    UILabel *currentTime_{nullptr};
    TimeAnimatorCallback *currentTimeCallback_{nullptr};
    Animator *currentTimeAnimator_{nullptr};
    TimerCountDownCallback *countDownTimecallBack_ = nullptr;
    Animator *countDownTimeanimator_ = nullptr;
    UIImageView *bgImg_{nullptr};
};
}
#endif // TIMER_TIMING_PAGE_H