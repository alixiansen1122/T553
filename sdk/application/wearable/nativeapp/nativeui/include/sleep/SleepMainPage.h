/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepMainPage
 * Created: 2025-06-05
 */
#ifndef SLEEP_MAIN_PAGE_H
#define SLEEP_MAIN_PAGE_H

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
class SleepMainPage : public SlicePage<SleepPresenter>,
                             public UIView::OnClickListener,
                             public UIView::OnDragListener {
public:
    SleepMainPage();
    ~SleepMainPage() override;

    void OnStart(void* data) override;
    void InitNoRecordPage();
    void InitRecordPage();
    void LoadActualSleepTime(std::tuple<uint32_t, uint32_t> timeTuple);
    void LoadTargetTimeButton(std::tuple<uint32_t, uint32_t> timeTuple);
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void InitCircleProgress();
    uint32_t GetSleepingScores();
    void InitSleepTimeLabel();
    void ScoreDisplay();
    bool GetRecord();
    void InitScoreDisplayPicture();
private:
    UIScrollView *group_{nullptr};
    UIImageView *imageViewMain_{nullptr};
    // noRecord页面控件
    UILabel *sleepTitleLabel_{nullptr};
    UIImageView *moonNoRecordImg_{nullptr};
    UILabel *noRecordLabel_{nullptr};
    UILabel *tipsLabel_{nullptr};
    // Record页面控件
    UIImageView *moonImg_{nullptr};
    UILabel *sleepScoreTextLabel_{nullptr};
    UILabel *sleepScoresLabel_{nullptr};
    UILabel *scoreUnitLabel_{nullptr};
    UIImageView *scoreStarImg1_{nullptr};
    UIImageView *scoreStarImg2_{nullptr};
    UIImageView *scoreStarImg3_{nullptr};
    UIImageView *scoreStarImg4_{nullptr};
    UIImageView *scoreStarImg5_{nullptr};
    UILabel *targetSleepTimeHoursLabel_{nullptr};
    UILabel *targetSleepTimeHoursUnitLabel_{nullptr};
    UILabel *targetSleepTimeMinutesLabel_{nullptr};
    UILabel *targetSleepTimeMinutesUnitLabel_{nullptr};
    UILabel *targetTimeLabel_{nullptr};
    UILabelButton *targetTimeButton_{nullptr};
    UIImageView *settingImg_{nullptr};
    UICircleProgress* circleProgress_{nullptr};
};
}

#endif // SLEEP_MAIN_PAGE_H
