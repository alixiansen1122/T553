/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerMainPage
 * Create: 2025-06-09
 */

#include <string>
#include "SlicePageFactory.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_arc_label.h"
#include "graphic_timer.h"
#include "ohos_timer.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "timers/TimerView.h"
#include "timers/TimerSettingPage.h"
#include "timers/TimerCountDown.h"
#include "timers/TimerPresenter.h"
#include "timers/TimerMainPage.h"

namespace OHOS {
static constexpr uint8_t MAIN_TITLE_FONT_SIZE = 40;
static constexpr int16_t MAIN_TITLE_SIZE_WIDTH = 160;
static constexpr int16_t MAIN_TITLE_SIZE_HEIGHT = 50;
static constexpr int16_t MAIN_TITLE_POSITION_X = 150;
static constexpr int16_t MAIN_TITLE_POSITION_Y = 20;
static constexpr int16_t MAIN_ADDBUTTON_POSITION_X = 111;
static constexpr int16_t MAIN_ADDBUTTON_POSITION_Y = 347;
static constexpr int16_t MAIN_ADDBUTTON_SIZE_WIDTH = 232;
static constexpr int16_t MAIN_ADDBUTTON_SIZE_HEIGHT = 76;
static constexpr int16_t SELECTMIN_POX = 86;
static constexpr int16_t SELECTMIN_POXS = 252;
static constexpr int16_t SELECTMIN_POY = 89;
static constexpr int16_t SELECTMIN_POYS = 220;
static constexpr int16_t SELECTMIN_SIZE = 100;
static constexpr uint16_t MS_ONE_SECONDS = 1000;
static constexpr uint8_t MIN = 60;
static constexpr char *SET_TIME_BUTTON  = "setTimeButton";
static constexpr char *ONE_MINUTE  = "1";
static constexpr char *THREE_MINUTE  = "3";
static constexpr char *FIFTEEN_MINUTE  = "15";
static constexpr char *THIRTY_MINUTE  = "30";
static TimerMainPage *g_pTimerMainPage{nullptr};

REGIST_SLICE_PAGE(VIEW_TIMER, TIMER_PAGES::TIMER_MAIN_PAGE, TimerMainPage, true);

TimerMainPage::TimerMainPage()
{
    g_pTimerMainPage = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "TimerMainPage::TimerMainPage()");
}

TimerMainPage::~TimerMainPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (countDownFragment_ != nullptr) {
        countDownFragment_->DestroyView();
        delete countDownFragment_;
        countDownFragment_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (addButton_ != nullptr) {
        delete addButton_;
        addButton_ = nullptr;
    }

    for (int i = 0; i < SELECT_IMAGE_COUNT; i++) {
        if (selectMin_[i] != nullptr) {
            delete selectMin_[i];
            selectMin_[i] = nullptr;
        }
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(TIMER_IMAGE);
    TimerModel::GetInstance().SetIsShowFragment(false);
    g_pTimerMainPage = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "TimerMainPage::~TimerMainPage()");
}

void TimerMainPage::OnResume()
{
    if (countDownFragment_->GetFragmentView()->IsVisible()) {
        countDownFragment_->OnResume();
    }
}

void TimerMainPage::OnPause()
{
    if (countDownFragment_->GetFragmentView()->IsVisible()) {
        countDownFragment_->OnPause();
    }
}

TimerMainPage* TimerMainPage::GetInstance()
{
    return g_pTimerMainPage;
}

void TimerMainPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group_ fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetVisible(false);

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImg_ fail");
        return;
    }
    LOADIMG::LoadImageViewImg(bgImg_, TIMER_IMAGE, IMAGE_TIMER_BG);
    bgImg_->SetPosition(0, 0, RESOLUTION_WIDTH, RESOLUTION_WIDTH);
    group_->Add(bgImg_);

    countDownFragment_ = new TimerCountDown();
    if (countDownFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "countDownFragment_ fail");
        return;
    }
    countDownFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    countDownFragment_->CreateView();
    countDownFragment_->GetFragmentView()->SetVisible(false);

    InitView();
    LabelTitle();

    AddViewToPageContainer(group_);
    AddViewToPageContainer(countDownFragment_->GetFragmentView());
    // 定时器存在，不展示主页面，直接进入计时页面
    if (TimerModel::GetInstance().GetExistTimer()) {
        countDownFragment_->GetFragmentView()->SetVisible(true);
    } else if (TimerModel::GetInstance().GetIsShowFragment()) {
        TimerModel::GetInstance().SetIsShowFragment(false);
        countDownFragment_->RefreshFragment();
        countDownFragment_->GetFragmentView()->SetVisible(true);
    } else {
        group_->SetVisible(true);
    }
}

void TimerMainPage::LabelTitle()
{
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(MAIN_TITLE_POSITION_X, MAIN_TITLE_POSITION_Y);
    labelTitle_->SetText("计时器");
    labelTitle_->Resize(MAIN_TITLE_SIZE_WIDTH, MAIN_TITLE_SIZE_HEIGHT);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, MAIN_TITLE_FONT_SIZE);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    labelTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelTitle_->SetVisible(true);
    group_->Add(labelTitle_);
}

void TimerMainPage::InitView()
{
    addButton_ = new UIImageView();
    if (addButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "addButton_ fail");
        return;
    }
    LOADIMG::LoadImageViewImg(addButton_, TIMER_IMAGE, IMAGE_TIMER_USER);
    addButton_->SetPosition(MAIN_ADDBUTTON_POSITION_X, MAIN_ADDBUTTON_POSITION_Y);
    addButton_->Resize(MAIN_ADDBUTTON_SIZE_WIDTH, MAIN_ADDBUTTON_SIZE_HEIGHT);
    addButton_->SetViewId(SET_TIME_BUTTON);
    addButton_->SetTouchable(true);
    addButton_->SetOnClickListener(this);
    group_->Add(addButton_);

    int posX[SELECT_IMAGE_COUNT] = {SELECTMIN_POX, SELECTMIN_POXS, SELECTMIN_POX, SELECTMIN_POXS};
    int posY[SELECT_IMAGE_COUNT] = {SELECTMIN_POY, SELECTMIN_POY, SELECTMIN_POYS, SELECTMIN_POYS};
    const char *buttonViewId[SELECT_IMAGE_COUNT] = {ONE_MINUTE, THREE_MINUTE, FIFTEEN_MINUTE, THIRTY_MINUTE};
    int buttonImg[SELECT_IMAGE_COUNT] = {IMAGE_TIMER_1, IMAGE_TIMER_3, IMAGE_TIMER_15, IMAGE_TIMER_30};
    // 计时选择时间按钮
    for (int i = 0; i < SELECT_IMAGE_COUNT; i++) {
        selectMin_[i] = new UIImageView();
        if (selectMin_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "selectMin_[%d] fail", i);
            return;
        }
        selectMin_[i]->SetPosition(posX[i], posY[i]);
        selectMin_[i]->Resize(SELECTMIN_SIZE, SELECTMIN_SIZE);
        selectMin_[i]->SetTouchable(true);
        selectMin_[i]->SetOnClickListener(this);
        selectMin_[i]->SetViewId(buttonViewId[i]);
        LOADIMG::LoadImageViewImg(selectMin_[i], TIMER_IMAGE, buttonImg[i]);
        group_->Add(selectMin_[i]);
    }
}

void TimerMainPage::ShowMainPage()
{
    group_->SetVisible(true);
}

bool TimerMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), SET_TIME_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(TIMER_PAGES::TIMER_SETTING_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if ((strcmp(view.GetViewId(), ONE_MINUTE) == 0) ||
               (strcmp(view.GetViewId(), THREE_MINUTE) == 0) ||
               (strcmp(view.GetViewId(), FIFTEEN_MINUTE) == 0) ||
               (strcmp(view.GetViewId(), THIRTY_MINUTE) == 0)) {
        int num = std::stoi(std::string(view.GetViewId()));
        struct timeval start_time;
        gettimeofday(&start_time, nullptr);
        uint64_t s_time =
            (uint64_t)start_time.tv_sec * MS_ONE_SECONDS + start_time.tv_usec / MS_ONE_SECONDS;  // 转换成ms
        TimerModel::GetInstance().SetStartTime(s_time);
        TimerModel::GetInstance().SetEclipseTime(true, 0);
        TimerModel::GetInstance().SetTotalTime(num * MIN);
        countDownFragment_->RefreshFragment();
        countDownFragment_->GetFragmentView()->SetVisible(true);
    }
    return true;
}

bool TimerMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}
}
