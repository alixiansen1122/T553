/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Activity
 * Author:
 * Create: 2021-10-18
 */

#include "activity/ActivityWeekView.h"
#include "activity/ActivityPresenter.h"
#include "common/image_cache_manager.h"

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

ActivityWeekView::ActivityWeekView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::ActivityWeekView");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    g_pActivityWeekView = this;
}

ActivityWeekView::~ActivityWeekView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::~ActivityWeekView");
    if (viewiInitStatus) {
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
    UIView *uiView = new UIView();
    if (uiView == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView uiView is nullptr");
        return nullptr;
    }
    uiView->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x00, 165, 230).full);
    uiView->SetStyle(STYLE_BACKGROUND_OPA, 127); // 127 : background opacity
    uiView->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
    uiView->SetStyle(STYLE_BORDER_OPA, 127); // 127 : border opacity
    uiView->SetStyle(STYLE_BORDER_WIDTH, 0); // 0 : border width
    uiView->SetStyle(STYLE_PADDING_LEFT, 0);
    uiView->SetStyle(STYLE_PADDING_RIGHT, 0);
    uiView->SetStyle(STYLE_PADDING_TOP, 0);
    uiView->SetStyle(STYLE_PADDING_BOTTOM, 0);
    uiView->SetStyle(STYLE_MARGIN_LEFT, 0);
    uiView->SetStyle(STYLE_MARGIN_RIGHT, 0);
    uiView->SetStyle(STYLE_MARGIN_TOP, 0);
    uiView->SetStyle(STYLE_MARGIN_BOTTOM, 0);
    uiView->SetPosition(x, y);
    uiView->SetStyle(STYLE_BORDER_RADIUS, UIVIEW_WIDTH / 2 + 10); // 2 : half width 10 : offset
    return uiView;
}

void ActivityWeekView::InitSteps()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitSteps");
    uint32_t stepValue = 0;
    int weekToday = ActivityModel::GetInstance()->GetWeekToday();
    ActivityModel::GetInstance()->GetStepData((StepDataDistribute)weekToday, &stepValue);
    char stepChar[STEP_CHAR_NUM] = {0};
    (void)sprintf_s(stepChar, STEP_CHAR_NUM, "%d", stepValue);

    if (stepsLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitSteps nullptr");
        return;
    }
    stepsLabel->SetText(stepChar);

    int stepDigits = strnlen(stepChar, STEP_CHAR_NUM);
    int todayStringPosX = CENTER_POSITION_X - (STEP_STRING_TOTAL_LENGTH + stepDigits * NUM_LENGTH) / 2; // 2: half width
    todayString->SetPosition(todayStringPosX, STEPS_STRING_POSITION_Y);
    int stepsLabelPosX = todayStringPosX + TODAY_STRING_LENGTH;
    stepsLabel->SetPosition(stepsLabelPosX, STEPS_COUNT_POSITION_Y);
    int achievedStrPosX = stepsLabelPosX + stepDigits * NUM_LENGTH;
    achievedString->SetPosition(achievedStrPosX, STEPS_STRING_POSITION_Y);
}

void ActivityWeekView::InitWeekView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitWeekView");
    uint32_t weekValue = 0;
    for (int i = MONDAY_STEP_COUNTER; i <= SUNDAY_STEP_COUNTER; ++i) {
        ActivityModel::GetInstance()->GetStepData((StepDataDistribute)i, &weekValue);
        int weekday = i - MONDAY_STEP_COUNTER;
        // 10000:the actual number of steps corresponding to the interval
        int viewHeight = weekValue * (WEEKDAYVIEW_LOWEST_Y - WEEKDAYVIEW_HIGHEST_Y) / 10000;
        if (view[weekday] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitWeekView view[%d] is nullptr", weekday);
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
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitAvgStep");
    uint32_t weekStep = 0;
    uint32_t stepValue = 0;
    int today = ActivityModel::GetInstance()->GetWeekToday();
    int todayType = today + MONDAY_STEP_COUNTER;
    for (int i = MONDAY_STEP_COUNTER; i < todayType; ++i) {
        ActivityModel::GetInstance()->GetStepData((StepDataDistribute)i, &stepValue);
        weekStep += stepValue;
    }
    int avgDays = today;
    uint32_t avgStep = 0;
    if (avgDays != 0) {
        avgStep = weekStep / avgDays;
    }

    char avgChar[AVG_CHAR_NUM] = {0};
    (void)sprintf_s(avgChar, AVG_CHAR_NUM, "%d", avgStep);
    if (avgStepLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitAvgStep nullptr");
        return;
    }
    avgStepLabel->SetText(avgChar);

    int avgDigits = strnlen(avgChar, AVG_CHAR_NUM);
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView imageViewShoe is nullptr");
    }
    imageViewShoe->SetPosition(IMAGE_POSITION_X, STEPS_POSITION_Y, TITLE_FONT_SIZE, TITLE_FONT_SIZE);

    stepsString = new UILabel();
    if (stepsString == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView stepsString is nullptr");
    }
    stepsString->SetPosition(STEPS_POSITION_X, STEPS_POSITION_Y, TITLE_STEP_WIDTH, TITLE_STEP_HEIGHT);
    stepsString->SetText("步数");
    stepsString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_FONT_SIZE);
    stepsString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    todayString = new UILabel();
    if (todayString == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView todayString is nullptr");
    }
    todayString->SetPosition(STEPS_STRING_POSITION_X, STEPS_STRING_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    todayString->SetText("今天");
    todayString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    todayString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    todayString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    achievedString = new UILabel();
    if (achievedString == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView achievedString is nullptr");
    }
    achievedString->SetPosition(STEPS_STRING_POSITION_X, STEPS_STRING_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    achievedString->SetText("/10000步");
    achievedString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    achievedString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    achievedString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    stepsLabel = new UILabel();
    if (stepsLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView stepsLabel is nullptr");
    }
    stepsLabel->SetPosition(STEPS_COUNT_POSITION_X, STEPS_COUNT_POSITION_Y, VALUE_WIDTH, VALUE_HEIGHT);
    stepsLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATA_FONT_SIZE);
    stepsLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
}

void ActivityWeekView::InitAvgLabel()
{
    avgString = new UILabel();
    if (avgString == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView avgString is nullptr");
    }
    avgString->SetPosition(AVG_STEP_POSITION_X, AVG_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    avgString->SetText("周平均");
    avgString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    avgString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    avgString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    behindAvgString = new UILabel();
    if (behindAvgString == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView behindAvgString is nullptr");
    }
    behindAvgString->SetPosition(AVG_STEP_POSITION_X, AVG_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    behindAvgString->SetText("步");
    behindAvgString->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STRING_FONT_SIZE);
    behindAvgString->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    behindAvgString->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    avgStepLabel = new UILabel();
    if (avgStepLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView avgStepLabel is nullptr");
    }
    avgStepLabel->SetPosition(AVG_STEP_POSITION_X, AVG_STEP_POSITION_Y, VALUE_WIDTH, VALUE_HEIGHT);
    avgStepLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATA_FONT_SIZE);
    avgStepLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    chartString = new UILabel();
    if (chartString == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView chartString is nullptr");
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView chartHighest is nullptr");
    }
    chartHighest->SetPosition(WEEKDAYLABEL_POSITION_X, WEEKDAYVIEW_HIGHEST_Y, LINE_WIDTH, LINE_HEIGHT);
    chartHighest->SetStyle(STYLE_BACKGROUND_COLOR,  Color::Gray().full);
    chartHighest->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    chartLowest = new UICanvas();
    if (chartLowest == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView chartLowest is nullptr");
    }
    chartLowest->SetPosition(WEEKDAYLABEL_POSITION_X, WEEKDAYVIEW_LOWEST_Y, LINE_WIDTH, LINE_HEIGHT);
    chartLowest->SetStyle(STYLE_BACKGROUND_COLOR,  Color::GetColorFromRGB(0x00, 165, 230).full);
    chartLowest->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void ActivityWeekView::InitDraggble()
{
    UIView::OnDragListener *earDragListener = (UIView::OnDragListener *)OHOS::ActivityPresenter::GetInstance();
    if (earDragListener == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitDraggble earDragListener GetInstance fail");
        return;
    }
    SetOnDragListener(earDragListener);
    SetViewId(ACTIVITY_WEEK_VIEW);
    SetDraggable(true);
    SetTouchable(true);
}

bool ActivityWeekView::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityWeekView InitView");
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
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView view[%d] is nullptr", i);
            return false;
        }
        weekDayLabel[i] = new UILabel();
        if (weekDayLabel[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityWeekView::InitView weekDayLabel[%d] is nullptr", i);
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
    InitWeekView();
    InitSteps();
    InitAvgStep();

    return true;
}

void ActivityWeekView::PreLoad(void)
{
    if (!viewiInitStatus) {
        if (InitView()) {
            viewiInitStatus = true;
        }
    }

    if (!resLoadStatus) {
        ImageInfo* img = ImageCacheManager::GetInstance().LoadSingleRes(PNG_STEP_SHOE_IMAGE_PATH);
        if (img != nullptr) {
            imageViewShoe->SetSrc(img);
            resLoadStatus = true;
        }
    }
}

void ActivityWeekView::UnLoad(void)
{
    if (resLoadStatus) {
        ImageInfo* img = nullptr;
        imageViewShoe->SetSrc(img);
        ImageCacheManager::GetInstance().UnloadSingleRes(PNG_STEP_SHOE_IMAGE_PATH);
        resLoadStatus = false;
    }
}

void ActivityWeekView::RefreshSteps(int step)
{
    if (stepsLabel == nullptr) {
        return;
    }
    char stepChar[STEP_CHAR_NUM] = {0};

    (void)sprintf_s(stepChar, STEP_CHAR_NUM, "%d", step);
    stepsLabel->SetText(stepChar);

    int stepDigits = strnlen(stepChar, STEP_CHAR_NUM);
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
    int today = ActivityModel::GetInstance()->GetWeekToday();
    if (view[today] == nullptr) {
        return;
    }
    view[today]->SetPosition(WEEKDAYLABEL_POSITION_X + WIDTH_BETWEEN_VIEW * today, WEEKDAYVIEW_LOWEST_Y - todayHeight);
    view[today]->Resize(UIVIEW_WIDTH, todayHeight + HIDDEN_SRC_LENGTH);
    view[today]->Invalidate();
}
}
