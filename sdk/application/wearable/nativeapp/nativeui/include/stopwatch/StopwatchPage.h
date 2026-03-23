/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchPage
 * Create: 2025-06-06
 */

#ifndef STOPWATCH_PAGE_H
#define STOPWATCH_PAGE_H

#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "components/ui_sweep_clock.h"
#include "animator/animator.h"
#include "UiConfig.h"
#include "SlicePage.h"
#include "NativeRegisterManager.h"
#include "StopwatchPresenter.h"

namespace OHOS {
class StopwatchPresenter;
class StopwatchPage : public SlicePage<StopwatchPresenter>,
                           public UIView::OnClickListener,
                           public UIView::OnDragListener {
public:
    StopwatchPage();
    ~StopwatchPage() override;

    void OnStart(void* data) override;
    void OnStop() override;
    void OnPause() override;
    void OnResume() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
private:
    void TimeOverRange(void);
    void DrawButtonStatusView(bool isRunning);
    void DrawRecordListVisibleView(bool isRecordListVisible);
    void RefreshRecordsList(void);
    class StopwatchAnimatorCallback : public AnimatorCallback {
    public:
        StopwatchAnimatorCallback(StopwatchPage* page) : page_(page) {}
        virtual ~StopwatchAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        StopwatchPage* page_{nullptr};
    };
    class TimeAnimatorCallback : public AnimatorCallback {
    public:
        TimeAnimatorCallback() {}
        virtual ~TimeAnimatorCallback() override {}
        void Callback(UIView* view) override;
    };
    Animator* stopwatchAnimator_{nullptr};
    StopwatchAnimatorCallback* stopwatchAnimatorCallback_{nullptr};
    Animator* timeAnimator_{nullptr};
    TimeAnimatorCallback* timeCallback_{nullptr};
    UISweepClock *sweepClock_{nullptr};
    Image *secondHand_{nullptr};
    UILabel *currentTime_{nullptr};
    UILabel *appName_{nullptr};
    UILabel *elapsedTime_{nullptr};
    UISimpleList *recordsList_{nullptr};
    UILabelButton *startButton_{nullptr};
    UILabelButton *resetButton_{nullptr};
    UIImageView *backGround_{nullptr};
    UIImageView *imgScale_{nullptr};
};
}
#endif  // STOPWATCH_PAGE_H
