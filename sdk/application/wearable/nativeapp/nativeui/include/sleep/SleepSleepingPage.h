/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepSleepingPage
 * Created: 2025-06-05
 */
#ifndef SLEEP_SLEEPING_PAGE_H
#define SLEEP_SLEEPING_PAGE_H

#include <sys/time.h>
#include "View.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "components/ui_circle_progress.h"
#include "components/ui_view_group.h"
#include "components/ui_circle_progress.h"
#include "components/ui_scroll_view.h"
#include "ui_resource_sleep.h"
#include "main/LoadImg.h"
#include "sleep/SleepTargetTime.h"
#include "sleep/SleepPresenter.h"
#include "sleep/SleepView.h"
#include "sleep/SleepModel.h"


namespace OHOS {
class SleepPresenter;
class SleepSleepingPage : public SlicePage<SleepPresenter>,
                             public UIView::OnClickListener,
                             public UIView::OnDragListener {
public:
    SleepSleepingPage();
    ~SleepSleepingPage() override;

    void OnStart(void* data) override;
    void OnPause() override;
    void InitCircleProgress();
    void InitSleepTimeLabel();
    void LoadTargetTimeForHourAndMin(std::tuple<uint32_t, uint32_t> timeTuple);
    void StopAnimator();
private:
    UIScrollView *group_{nullptr};
    UIImageView *bgImg_{nullptr};
    UIImageView *moonImg_{nullptr};
    UILabel *sleepTitleLabel_{nullptr};
    UILabel *sleepingLabel_{nullptr};
    UILabel *targetSleepTimeHoursLabel_{nullptr};
    UILabel *targetSleepTimeHoursUnitLabel_{nullptr};
    UILabel *targetSleepTimeMinutesLabel_{nullptr};
    UILabel *targetSleepTimeMinutesUnitLabel_{nullptr};
    UIImageView *sleepTimeBgImg_{nullptr};
    UICircleProgress* circleProgress_{nullptr};

class SleepAnimatorCallback : public AnimatorCallback {
public:
    explicit SleepAnimatorCallback(SleepSleepingPage* page) : page_(page) {}
    ~SleepAnimatorCallback() override {}
    void Callback(UIView *view) override;

private:
    SleepSleepingPage* page_{nullptr};
};
    Animator *animator_ = nullptr;
    SleepAnimatorCallback *callBack_ = nullptr;
    struct timeval startTime_ = {0};
};
}

#endif // SLEEP_SLEEPING_PAGE_H
