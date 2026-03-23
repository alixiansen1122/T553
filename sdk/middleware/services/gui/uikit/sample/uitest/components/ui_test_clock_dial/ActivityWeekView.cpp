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

#include "ActivityWeekView.h"

namespace OHOS {
static constexpr uint16_t CHAR_LENGTH = 10;
static constexpr uint16_t UIVIEW_WIDTH = 30;
static constexpr uint16_t UIVIEW_HEIGHT = 90;
static constexpr uint16_t TITLE_STEP_WIDTH = 80;
static constexpr uint16_t TITLE_STEP_HEIGHT = 40;
static constexpr uint16_t TEXT_WIDTH = 150;
static constexpr uint16_t TEXT_HEIGHT = 35;
static constexpr uint16_t VALUE_WIDTH = 130;
static constexpr uint16_t VALUE_HEIGHT = 38;
static constexpr uint16_t LINE_WIDTH = 360;
static constexpr uint16_t LINE_HEIGHT = 2;
static constexpr uint16_t CHART_STRING_WIDTH = 150;
static constexpr uint16_t CHART_STRING_HEIGHT = 20;
static constexpr uint16_t STEPS_POSITION_X = 220;
static constexpr uint16_t STEPS_POSITION_Y = 40;
static constexpr uint16_t WEEKDAYVIEW_HIGHEST_Y = 210;
static constexpr uint16_t WEEKDAYVIEW_LOWEST_Y = 300;
static constexpr uint16_t CHART_STRING_Y = WEEKDAYVIEW_HIGHEST_Y - 20;
static constexpr uint16_t WEEKDAYLABEL_POSITION_X = 47;
static constexpr uint16_t WEEKDAYLABEL_POSITION_Y = 302;
static constexpr uint16_t AVG_STEP_POSITION_X = 99;
static constexpr uint16_t AVG_STEP_POSITION_Y = 357;
static constexpr uint16_t AVG_POSITION_Y = 360;
static constexpr uint16_t STEPS_STRING_POSITION_X = 144;
static constexpr uint16_t STEPS_STRING_POSITION_Y = 100;
static constexpr uint16_t STEPS_COUNT_POSITION_X = 144;
static constexpr uint16_t STEPS_COUNT_POSITION_Y = 97;
static constexpr uint16_t CENTER_POSITION_X = 227;
static constexpr uint16_t STEP_STRING_TOTAL_LENGTH = 170;
static constexpr uint16_t AVG_STRING_TOTAL_LENGTH = 120;
static constexpr uint16_t NUM_LENGTH = 18;
static constexpr uint16_t TODAY_STRING_LENGTH = 63;
static constexpr uint16_t AVG_STRING_LENGTH = 93;
static constexpr uint16_t HIDDEN_SRC_LENGTH = 20;
static constexpr uint16_t WIDTH_BETWEEN_VIEW = 55;
static constexpr uint16_t IMAGE_POSITION_X = STEPS_POSITION_X - 50;

static constexpr uint16_t TITLE_FONT_SIZE = 35;
static constexpr uint16_t STRING_FONT_SIZE = 30;
static constexpr uint16_t DATA_FONT_SIZE = 33;
static constexpr uint16_t SAMLL_FONT_SIZE = 15;
static constexpr uint16_t COORDINATE_FONT_SIZE = 20;
static constexpr char const *WEEK_DAY[DAYS] = {"一", "二", "三", "四", "五", "六", "日"};
static constexpr uint16_t STEP_CHAR_NUM = 255;
static constexpr uint16_t AVG_CHAR_NUM = 255;
static constexpr uint16_t MAX_STEPS = 65535;

static ActivityWeekView *g_pActivityWeekView = nullptr;

StepAllData g_stepData = {10050, 23421, 12312, 4252, 8993, 12314, 3423};

ActivityWeekView::ActivityWeekView()
{
    GRAPHIC_LOGD("ActivityWeekView::ActivityWeekView\n");
    g_pActivityWeekView = this;
}

ActivityWeekView::~ActivityWeekView()
{
    GRAPHIC_LOGD("ActivityWeekView::~ActivityWeekView\n");
    if (initViewStatus) {
        RemoveAll();
        for (int i = 0; i < DAYS; i++) {
            delete view[i];
            view[i] = nullptr;
            delete weekDayLabel[i];
            weekDayLabel[i] = nullptr;
        }
        delete imageViewShoe;
        imageViewShoe = nullptr;
        delete avgStepLabel;
        avgStepLabel = nullptr;
        delete stepsString;
        stepsString = nullptr;
        delete stepsLabel;
        stepsLabel = nullptr;
        delete todayString;
        todayString = nullptr;
        delete achievedString;
        achievedString = nullptr;
        delete avgString;
        avgString = nullptr;
        delete behindAvgString;
        behindAvgString = nullptr;
        delete chartString;
        chartString = nullptr;
        delete chartHighest;
        chartHighest = nullptr;
        delete chartLowest;
        chartLowest = nullptr;
    }
    g_pActivityWeekView = nullptr;
}

ActivityWeekView *ActivityWeekView::GetInstance(void)
{
    return g_pActivityWeekView;
}

UIView *ActivityWeekView::CreateUIView(int16_t x, int16_t y)
{
    UIView *viewObj = new UIView();
    if (view == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView view is nullptr\n");
        return nullptr;
    }
    viewObj->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x00, 0xa5, 0xe6).full);
    viewObj->SetStyle(STYLE_BACKGROUND_OPA, 127); // 127 : background opacity
    viewObj->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
    viewObj->SetStyle(STYLE_BORDER_OPA, 127); // 127 : border opacity
    viewObj->SetStyle(STYLE_BORDER_WIDTH, 0); // 0 : border width
    viewObj->SetStyle(STYLE_PADDING_LEFT, 0);
    viewObj->SetStyle(STYLE_PADDING_RIGHT, 0);
    viewObj->SetStyle(STYLE_PADDING_TOP, 0);
    viewObj->SetStyle(STYLE_PADDING_BOTTOM, 0);
    viewObj->SetStyle(STYLE_MARGIN_LEFT, 0);
    viewObj->SetStyle(STYLE_MARGIN_RIGHT, 0);
    viewObj->SetStyle(STYLE_MARGIN_TOP, 0);
    viewObj->SetStyle(STYLE_MARGIN_BOTTOM, 0);
    viewObj->SetPosition(x, y);
    viewObj->SetStyle(STYLE_BORDER_RADIUS, UIVIEW_WIDTH / 2 + 10); // 2 : half width 10 : offset
    return viewObj;
}

void ActivityWeekView::InitSteps()
{
    GRAPHIC_LOGD("ActivityWeekView::InitSteps\n");
    uint32_t stepValue = 0;
    int weekToday = GetWeekToday();
    GetStepData(static_cast<StepDataDistribute>(weekToday), &stepValue);
    char stepChar[STEP_CHAR_NUM] = {0};
    (void)sprintf_s(stepChar, STEP_CHAR_NUM, "%u", stepValue);

    if (stepsLabel == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitSteps nullptr\n");
        return;
    }
    stepsLabel->SetText(stepChar);

    int stepDigits = static_cast<int>(strnlen(stepChar, STEP_CHAR_NUM));
    int todayStringPosX = CENTER_POSITION_X - (STEP_STRING_TOTAL_LENGTH + stepDigits * NUM_LENGTH) / 2; // 2: half width
    todayString->SetPosition(todayStringPosX, STEPS_STRING_POSITION_Y);
    int stepsLabelPosX = todayStringPosX + TODAY_STRING_LENGTH;
    stepsLabel->SetPosition(stepsLabelPosX, STEPS_COUNT_POSITION_Y);
    int achievedStrPosX = stepsLabelPosX + stepDigits * NUM_LENGTH;
    achievedString->SetPosition(achievedStrPosX, STEPS_STRING_POSITION_Y);
}

void ActivityWeekView::InitWeekView()
{
    GRAPHIC_LOGD("ActivityWeekView::InitWeekView\n");
    uint32_t weekValue = 0;
    for (int i = static_cast<int>(StepDataDistribute::MONDAY_STEP_COUNTER);
         i <= static_cast<int>(StepDataDistribute::SUNDAY_STEP_COUNTER);
         ++i) {
        GetStepData(static_cast<StepDataDistribute>(i), &weekValue);
        int weekday = i - static_cast<int>(StepDataDistribute::MONDAY_STEP_COUNTER);
        // 10000:the actual number of steps corresponding to the interval
        int viewHeight = static_cast<int>(weekValue * (WEEKDAYVIEW_LOWEST_Y - WEEKDAYVIEW_HIGHEST_Y) / 10000);
        if (view[weekday] == nullptr) {
            GRAPHIC_LOGE("ActivityWeekView::InitWeekView view[%d] is nullptr\n", weekday);
            return;
        }
        view[weekday]->SetPosition(WEEKDAYLABEL_POSITION_X + WIDTH_BETWEEN_VIEW * weekday,
                                   WEEKDAYVIEW_LOWEST_Y - viewHeight);
        view[weekday]->Resize(UIVIEW_WIDTH, viewHeight + HIDDEN_SRC_LENGTH);
        view[weekday]->Invalidate();
    }
}

void ActivityWeekView::InitAvgStep()
{
    GRAPHIC_LOGD("ActivityWeekView::InitAvgStep\n");
    uint32_t weekStep = 0;
    uint32_t stepValue = 0;
    int today = GetWeekToday();
    int todayType = today + static_cast<int>(StepDataDistribute::MONDAY_STEP_COUNTER);
    for (int i = static_cast<int>(StepDataDistribute::MONDAY_STEP_COUNTER); i < todayType; ++i) {
        GetStepData(static_cast<StepDataDistribute>(i), &stepValue);
        weekStep += stepValue;
    }
    int avgDays = today;
    uint32_t avgStep = 0;
    if (avgDays != 0) {
        avgStep = weekStep / static_cast<uint32_t>(avgDays);
    }

    char avgChar[AVG_CHAR_NUM] = {0};
    (void)sprintf_s(avgChar, AVG_CHAR_NUM, "%u", avgStep);
    if (avgStepLabel == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitAvgStep nullptr\n");
        return;
    }
    avgStepLabel->SetText(avgChar);

    int avgDigits = static_cast<int>(strnlen(avgChar, AVG_CHAR_NUM));
    int avgStringPosX = CENTER_POSITION_X - (AVG_STRING_TOTAL_LENGTH  + avgDigits * NUM_LENGTH) / 2; // 2 : half width
    avgString->SetPosition(avgStringPosX, AVG_POSITION_Y);
    int avgStepLabelPosX = avgStringPosX + AVG_STRING_LENGTH;
    avgStepLabel->SetPosition(avgStepLabelPosX, AVG_STEP_POSITION_Y);
    int behindAvgStrPosX = avgStepLabelPosX + avgDigits * NUM_LENGTH;
    behindAvgString->SetPosition(behindAvgStrPosX, AVG_POSITION_Y);
}

void ActivityWeekView::InitStepLabel()
{
    imageViewShoe = new UIImageView();
    if (imageViewShoe == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView imageViewShoe is nullptr\n");
    }
    imageViewShoe->SetPosition(IMAGE_POSITION_X, STEPS_POSITION_Y, TITLE_FONT_SIZE, TITLE_FONT_SIZE);
    imageViewShoe->SetSrc(PNG_STEP_SHOE_IMAGE_PATH);

    stepsString = new UILabel();
    if (stepsString == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView stepsString is nullptr\n");
    }
    stepsString->SetPosition(STEPS_POSITION_X, STEPS_POSITION_Y, TITLE_STEP_WIDTH, TITLE_STEP_HEIGHT);
    stepsString->SetText("步数");
    stepsString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_FONT_SIZE);
    stepsString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    todayString = new UILabel();
    if (todayString == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView todayString is nullptr\n");
    }
    todayString->SetPosition(STEPS_STRING_POSITION_X, STEPS_STRING_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    todayString->SetText("今天");
    todayString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    todayString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    todayString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    achievedString = new UILabel();
    if (achievedString == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView achievedString is nullptr\n");
    }
    achievedString->SetPosition(STEPS_STRING_POSITION_X, STEPS_STRING_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    achievedString->SetText("/10000步");
    achievedString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    achievedString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    achievedString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    stepsLabel = new UILabel();
    if (stepsLabel == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView stepsLabel is nullptr\n");
    }
    stepsLabel->SetPosition(STEPS_COUNT_POSITION_X, STEPS_COUNT_POSITION_Y, VALUE_WIDTH, VALUE_HEIGHT);
    stepsLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATA_FONT_SIZE);
    stepsLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
}

void ActivityWeekView::InitAvgLabel()
{
    avgString = new UILabel();
    if (avgString == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView avgString is nullptr\n");
    }
    avgString->SetPosition(AVG_STEP_POSITION_X, AVG_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    avgString->SetText("周平均");
    avgString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    avgString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    avgString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    behindAvgString = new UILabel();
    if (behindAvgString == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView behindAvgString is nullptr\n");
    }
    behindAvgString->SetPosition(AVG_STEP_POSITION_X, AVG_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    behindAvgString->SetText("步");
    behindAvgString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    behindAvgString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    behindAvgString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    avgStepLabel = new UILabel();
    if (avgStepLabel == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView avgStepLabel is nullptr\n");
    }
    avgStepLabel->SetPosition(AVG_STEP_POSITION_X, AVG_STEP_POSITION_Y, VALUE_WIDTH, VALUE_HEIGHT);
    avgStepLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATA_FONT_SIZE);
    avgStepLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    chartString = new UILabel();
    if (chartString == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView chartString is nullptr\n");
    }
    chartString->SetPosition(WEEKDAYLABEL_POSITION_X, CHART_STRING_Y, CHART_STRING_WIDTH, CHART_STRING_HEIGHT);
    chartString->SetText("10000");
    chartString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SAMLL_FONT_SIZE);
    chartString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    chartString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
}

void ActivityWeekView::InitUICanvas()
{
    chartHighest = new UICanvas();
    if (chartHighest == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView chartHighest is nullptr\n");
    }
    chartHighest->SetPosition(WEEKDAYLABEL_POSITION_X, WEEKDAYVIEW_HIGHEST_Y, LINE_WIDTH, LINE_HEIGHT);
    chartHighest->SetStyle(STYLE_BACKGROUND_COLOR,  Color::Gray().full);
    chartHighest->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    chartLowest = new UICanvas();
    if (chartLowest == nullptr) {
        GRAPHIC_LOGE("ActivityWeekView::InitView chartLowest is nullptr\n");
    }
    chartLowest->SetPosition(WEEKDAYLABEL_POSITION_X, WEEKDAYVIEW_LOWEST_Y, LINE_WIDTH, LINE_HEIGHT);
    chartLowest->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x00, 0xa5, 0xe6).full);
    chartLowest->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void ActivityWeekView::InitDraggble()
{
}

bool ActivityWeekView::InitView()
{
    InitStepLabel();
    Add(imageViewShoe);
    Add(stepsString);
    Add(todayString);
    Add(achievedString);
    Add(stepsLabel);

    InitAvgLabel();
    Add(avgString);
    Add(behindAvgString);
    Add(avgStepLabel);
    Add(chartString);

    InitUICanvas();
    Add(chartHighest);
    for (int i = 0; i < DAYS; i++) {
        view[i] = CreateUIView(WEEKDAYLABEL_POSITION_X + WIDTH_BETWEEN_VIEW * i, WEEKDAYVIEW_HIGHEST_Y);
        if (view[i] == nullptr) {
            GRAPHIC_LOGE("ActivityWeekView::InitView view[%d] is nullptr\n", i);
            return false;
        }
        weekDayLabel[i] = new UILabel();
        if (weekDayLabel[i] == nullptr) {
            GRAPHIC_LOGE("ActivityWeekView::InitView weekDayLabel[%d] is nullptr\n", i);
            return false;
        }
        weekDayLabel[i]->SetPosition(WEEKDAYLABEL_POSITION_X + WIDTH_BETWEEN_VIEW * i, WEEKDAYLABEL_POSITION_Y,
                                     UIVIEW_WIDTH, UIVIEW_WIDTH);
        weekDayLabel[i]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        weekDayLabel[i]->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        weekDayLabel[i]->SetText(WEEK_DAY[i]);
        weekDayLabel[i]->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
        weekDayLabel[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, COORDINATE_FONT_SIZE);
        weekDayLabel[i]->SetAlign(TEXT_ALIGNMENT_CENTER);
        Add(view[i]);
        Add(weekDayLabel[i]);
    }
    Add(chartLowest);
    InitDraggble();
    initViewStatus = true;
    InitWeekView();
    InitSteps();
    InitAvgStep();

    return true;
}

void ActivityWeekView::RefreshSteps(int step)
{
    if (stepsLabel == nullptr) {
        return;
    }
    char stepChar[STEP_CHAR_NUM] = {0};

    (void)sprintf_s(stepChar, STEP_CHAR_NUM, "%d", step);
    stepsLabel->SetText(stepChar);

    int stepDigits = static_cast<int>(strnlen(stepChar, STEP_CHAR_NUM));
    // 2 : half width
    int todayStringPosX = CENTER_POSITION_X - (STEP_STRING_TOTAL_LENGTH + stepDigits * NUM_LENGTH) / 2;
    todayString->SetPosition(todayStringPosX, STEPS_STRING_POSITION_Y);
    todayString->Invalidate();
    int stepsLabelPosX = todayStringPosX + TODAY_STRING_LENGTH;
    stepsLabel->SetPosition(stepsLabelPosX, STEPS_COUNT_POSITION_Y);
    stepsLabel->Invalidate();
    int achievedStrPosX = stepsLabelPosX + stepDigits * NUM_LENGTH;
    achievedString->SetPosition(achievedStrPosX, STEPS_STRING_POSITION_Y);
    achievedString->Invalidate();
}

void ActivityWeekView::RefreshTodayView(int step)
{
    // 10000:the actual number of steps corresponding to the interval
    int todayHeight = step * (WEEKDAYVIEW_LOWEST_Y - WEEKDAYVIEW_HIGHEST_Y) / 10000;
    int today = GetWeekToday();
    if (view[today] == nullptr) {
        return;
    }
    view[today]->SetPosition(WEEKDAYLABEL_POSITION_X + WIDTH_BETWEEN_VIEW * today, WEEKDAYVIEW_LOWEST_Y - todayHeight);
    view[today]->Resize(UIVIEW_WIDTH, todayHeight + HIDDEN_SRC_LENGTH);
    view[today]->Invalidate();
}

int ActivityWeekView::GetWeekToday(void)
{
    return static_cast<int>(StepDataDistribute::THURSDAY_STEP_COUNTER);
}

void ActivityWeekView::GetStepData(const StepDataDistribute &type, uint32_t *value)
{
    *value = g_stepData.weekSteps[static_cast<int>(type)];
    return;
}
}
