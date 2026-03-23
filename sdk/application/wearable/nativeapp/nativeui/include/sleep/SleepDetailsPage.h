/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepDetailsPage
 * Created: 2025-06-05
 */
#ifndef SLEEP_DETAILS_PAGE_H
#define SLEEP_DETAILS_PAGE_H

#include "View.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "components/ui_circle_progress.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "main/LoadImg.h"
#include "ui_resource_sleep.h"
#include "sleep/SleepPresenter.h"
#include "sleep/SleepView.h"
#include "sleep/SleepModel.h"

namespace OHOS {
class SleepPresenter;
class SleepDetailsPage : public SlicePage<SleepPresenter>,
                             public UIView::OnDragListener {
public:
    SleepDetailsPage();
    ~SleepDetailsPage() override;

    void OnStart(void* data) override;
    void InitPage();
    void InitNoDeatilPage();
    void InitDetailPage();
    void InitSoberingTime();
    void InitRapidEyeMovement();
    void InitLightSleepTime();
    void InitDeepSleepTime();
    bool OnDrag(UIView &view, const DragEvent &event) override;
    bool GetRecord();
    std::string SetTimeUnit(std::tuple<uint32_t, uint32_t> timeTuple);
    void InitActualSleepTime(std::tuple<uint32_t, uint32_t> timeTuple);
    void LoadDeatilActualSleepTime();

private:
    UIScrollView *group_{nullptr};
    UILabel *sleepTitleLabel_{nullptr};
    UILabel *actualSleepNorecordTimeLabel_{nullptr}; // norecord
    UILabel *actualSleepTimeHoursLabel_{nullptr};
    UILabel *actualSleepTimeHoursUnitLabel_{nullptr};
    UILabel *actualSleepTimeMinutesLabel_{nullptr};
    UILabel *actualSleepTimeMinutesUnitLabel_{nullptr};

    UIImageView *bgImg_{nullptr};
    UIImageView *grayBgImg_{nullptr};
    UIImageView *redPointImg_{nullptr};
    UILabel *soberingTimeLabel_{nullptr};
    UILabel *soberingTimeUnitLabel_{nullptr};
    UIImageView *orangePointImg_{nullptr};
    UILabel *rapidEyeMovementLabel_ {nullptr};
    UILabel *rapidEyeMovementUnitLabel_{nullptr};
    UIImageView *greenPointImg_{nullptr};
    UILabel *lightSleepTimeLabel_{nullptr};
    UILabel *lightSleepTimeUnitLabel_{nullptr};
    UIImageView *bluePointImg_{nullptr};
    UILabel *deepSleepTimeLabel_{nullptr};
    UILabel *deepSleepTimeUnitLabel_{nullptr};
};
}

#endif // SLEEP_DETAILS_PAGE_H