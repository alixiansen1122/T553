/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
#include "AppGroupView.h"
#include "securec.h"

namespace OHOS {
constexpr int DAYS = 7;

enum class StepDataDistribute {
    MONDAY_STEP_COUNTER = 0,
    TUESDAY_STEP_COUNTER,
    WEDNESDAY_STEP_COUNTER,
    THURSDAY_STEP_COUNTER,
    FRIDAY_STEP_COUNTER,
    SATURDAY_STEP_COUNTER,
    SUNDAY_STEP_COUNTER,
};

struct StepAllData {
    uint32_t weekSteps[DAYS];
};

class ActivityWeekView : public AppGroupView {
public:
    ActivityWeekView();
    ~ActivityWeekView() override;
    ActivityWeekView(const ActivityWeekView &) = delete;
    ActivityWeekView &operator=(const ActivityWeekView &) = delete;
    static ActivityWeekView *GetInstance(void);
    int GetDigits(int stepValue);
    void InitSteps();
    void InitWeekView();
    void InitAvgStep();
    void InitStepLabel();
    void InitAvgLabel();
    void InitUICanvas();
    bool InitView() override;
    void RefreshSteps(int step);
    void RefreshTodayView(int step);
private:
    void InitDraggble();
    int GetWeekToday(void);
    void GetStepData(const StepDataDistribute &type, uint32_t *value);
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
};
} // namespace OHOS
#endif
