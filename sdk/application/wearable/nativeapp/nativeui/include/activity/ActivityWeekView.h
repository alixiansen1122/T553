/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Activity
 * Author:
 * Create: 2021-09-11
 */

#ifndef ACTIVITY_WEEK_VIEW_H
#define ACTIVITY_WEEK_VIEW_H

#include "components/ui_label.h"
#include "components/ui_canvas.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "components/ui_card_page.h"
#include "activity/ActivityModel.h"

namespace OHOS {
#define DAYS 7

class ActivityWeekView : public UICardPage {
public:
    ActivityWeekView();
    ~ActivityWeekView() override;
    static ActivityWeekView *GetInstance(void);
    int GetDigits(int stepValue);
    void InitSteps();
    void InitWeekView();
    void InitAvgStep();
    void InitStepLabel();
    void InitAvgLabel();
    void InitUICanvas();
    bool InitView();
    void PreLoad(void) override;
    void UnLoad(void) override;
    void RefreshSteps(int step);
    void RefreshTodayView(int step);
private:
    void InitDraggble();
    UIView *CreateUIView(int16_t x, int16_t y);
    UIView *view[DAYS]{nullptr};
    UILabel *weekDayLabel[DAYS]{nullptr};
    UIImageView *imageViewShoe{nullptr};
    UILabel *stepsLabel{nullptr};
    UILabel *avgStepLabel{nullptr};
    UILabel *stepsString{nullptr};
    UILabel *todayString{nullptr};
    UILabel *achievedString{nullptr};
    UILabel *avgString{nullptr};
    UILabel *behindAvgString{nullptr};
    UILabel *chartString{nullptr};
    UICanvas *chartHighest{nullptr};
    UICanvas *chartLowest{nullptr};
    bool viewiInitStatus{false};
    bool resLoadStatus{false};
};
} // namespace OHOS
#endif
