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

#include "ui_test_clock_dial.h"

namespace OHOS {
static UITestClockDial* g_pMainView = nullptr;
static constexpr uint16_t MESLABLE_X_TMP = 177;
static constexpr uint16_t MESLABLE_Y_TMP = 212;
static constexpr uint16_t MESLABLE_WIDTH_TMP = 100;
static constexpr uint16_t MESLABLE_HEIGHT_TMP = 30;
static constexpr uint8_t FONT_SIZE_TMP = 24;
static constexpr uint8_t PULL_MIDDLE_VIEW_OPA = 0;

struct MainViewMapper {
    CardId id;
    AppGroupView* (UITestClockDial::*func)(void);
};

static uint8_t g_modelIndex = 0;

static const MainViewMapper PAGE_MAPPER[] = {
    {CardId::WATCH_FACE, &UITestClockDial::InitWatchFacePage},
    {CardId::ACTIVITY, &UITestClockDial::InitActivityPage},
    {CardId::HEART_RATE, &UITestClockDial::InitHeartRatePage},
    {CardId::COMPASS, &UITestClockDial::InitCompassPage},
};

static const char* g_transitionMapper[] = {
    "1.推拉",
    "2.翻页",
    "3.立方体",
    "4.卡片",
};

void SwipeListener::OnSwipe(UISwipeView& view)
{
    UNUSED(view);
    int page = UITestClockDial::GetInstance()->GetHorCurrentPage();
    UITestClockDial::GetInstance()->PreLoad(page);
    return;
}

UITestClockDial::UITestClockDial()
{
    GRAPHIC_LOGD("MainViewSample::UITestClockDial\n");
    g_pMainView = this;
}

UITestClockDial* UITestClockDial::GetInstance(void)
{
    return g_pMainView;
}

void UITestClockDial::SetUp()
{
    ScreenModel::GetInstance()->InitCardConfig();
    mainSwipeGroup = new UICrossView();
    if (mainSwipeGroup == nullptr) {
        GRAPHIC_LOGE("mainSwipeGroup new fail\n");
        return;
    }

    if (callback == nullptr) {
        SwitchFlipTypeMode(g_modelIndex);
        if (callback == nullptr) {
            delete mainSwipeGroup;
            mainSwipeGroup = nullptr;
            GRAPHIC_LOGE("callback new fail\n");
            return;
        }
    }
    callback->SetContainer(mainSwipeGroup);

    HorizontalViewInit();
    HorViewLoad();
    VerticalViewInit();
    VertViewLoad();
    mainSwipeGroup->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    mainSwipeGroup->SetAnimatorTime(50);  // 50: animator drag time(ms);
    mainSwipeGroup->SetLoopState(true);
    mainSwipeGroup->EnableScreenCap(true);
    mainSwipeGroup->RegisterSwipeCallback(callback);
    mainSwipeGroup->SetHorCurrentPage(mainClockView->GetViewIndex());
}

void UITestClockDial::SwitchFlipTypeMode(uint8_t tmIndex)
{
    EffectType tm = static_cast<EffectType>(tmIndex);
    if (callback != nullptr) {
        delete callback;
        callback = nullptr;
    }
    switch (tm) {
        case EffectType::ZOOM:
            callback = new ZoomEffect();
            break;
        case EffectType::TURN_PAGE:
            callback = new TurnPageEffect();
            break;
        case EffectType::CUBE:
            callback = new CubeEffect();
            break;
        case EffectType::CARD_FLIP:
            callback = new CardFlipEffect();
            break;
        default:
            callback = new ZoomEffect();
    }
    if (callback == nullptr) {
        GRAPHIC_LOGE("callback new fail");
    }
}

void UITestClockDial::TearDown()
{
    dropDownView->ClearBackgroundBlur();
    messageView->ClearBackgroundBlur();

    for (int32_t i = 0; i < maxPage; i++) {
        recordCard[i] = nullptr;
        pageStatus[i] = false;
    }

    messageView->RemoveAll();

    if (mainSwipeGroup != nullptr) {
        delete mainSwipeGroup;
        mainSwipeGroup = nullptr;
    }

    if (mainClockView != nullptr) {
        delete mainClockView;
        mainClockView = nullptr;
    }

    if (activityWeekView != nullptr) {
        delete activityWeekView;
        activityWeekView = nullptr;
    }

    if (heartRateView != nullptr) {
        delete heartRateView;
        heartRateView = nullptr;
    }

    if (dropDownView != nullptr) {
        delete dropDownView;
        dropDownView = nullptr;
    }

    if (messageView != nullptr) {
        delete messageView;
        messageView = nullptr;
    }

    if (messageLabel != nullptr) {
        delete messageLabel;
        messageLabel = nullptr;
    }

    if (compassControlView != nullptr) {
        delete compassControlView;
        compassControlView = nullptr;
    }

    if (listener != nullptr) {
        delete listener;
        listener = nullptr;
    }

    if (callback != nullptr) {
        delete callback;
        callback = nullptr;
    }

    if (controlButton != nullptr) {
        delete controlButton;
        controlButton = nullptr;
    }
}

const UIView* UITestClockDial::GetTestView()
{
    return mainSwipeGroup;
}

bool UITestClockDial::HorizontalViewInit(void)
{
    GRAPHIC_LOGD("UITestClockDial::HorizontalViewInit\n");

    maxPage = ScreenModel::GetInstance()->GetMaxCard();
    GRAPHIC_LOGD("UITestClockDial::HorizontalViewInit1\n");
    for (int16_t i = 0; i < maxPage; i++) {
        recordCard[i] = GreatePage(i);
        if (recordCard[i] == nullptr) {
            GRAPHIC_LOGE("UITestClockDial:: recordCard %d error\n", i);
            return false;
        }
        GRAPHIC_LOGD("UITestClockDial:: recordCard[%d]:%p\n", i, recordCard[i]);
    }
    recordCard[0]->InitView();
    pageStatus[0] = true;
    if (maxPage >= 2) {  // 2 is right card
        recordCard[1]->InitView();
        pageStatus[1] = true;
    }
    if (maxPage >= 3) {  // 3 is left card
        recordCard[maxPage - 1]->InitView();
        pageStatus[maxPage - 1] = true;
    }
    listener = new SwipeListener();
    mainSwipeGroup->SetOnSwipeListener(listener);
    GRAPHIC_LOGD("UITestClockDial::InitPage over\n");
    return true;
}

void UITestClockDial::HorViewLoad(void)
{
    for (int16_t i = 0; i < maxPage; i++) {
        mainSwipeGroup->HorAdd(recordCard[i]);
    }
}

void UITestClockDial::VerticalViewInit(void)
{
    dropDownView = new PullUpDownView();
    if (dropDownView == nullptr) {
        GRAPHIC_LOGE("dropDownView new fail\n");
        return;
    }
    dropDownView->InitView();
    dropDownView->SetViewId("dropDownView");
    dropDownView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    messageView = new UICardPage();
    if (messageView == nullptr) {
        GRAPHIC_LOGE("messageView new fail\n");
        return;
    }
    messageView->SetPosition(0, 0);
    messageView->SetWidth(Screen::GetInstance().GetWidth());
    messageView->SetHeight(Screen::GetInstance().GetHeight());
    messageView->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    messageView->SetViewId("messageView");
    messageLabel = new UILabel();
    if (messageLabel == nullptr) {
        GRAPHIC_LOGE("messageLabel new fail\n");
        return;
    }
    messageLabel->SetPosition(MESLABLE_X_TMP, MESLABLE_Y_TMP, MESLABLE_WIDTH_TMP, MESLABLE_HEIGHT_TMP);
    messageLabel->SetText("无消息");
    messageLabel->SetAlign(TEXT_ALIGNMENT_CENTER);
    messageLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_SIZE_TMP);
    messageLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    messageView->Add(messageLabel);

    controlButton = new UILabelButton();
    controlButton->SetPosition(177, 350, 100, 50);  // 177, 350, 100, 50: left, right, width, height
    controlButton->SetText(g_transitionMapper[g_modelIndex]);
    controlButton->SetDragParentInstead(false);
    controlButton->SetOnClickListener(this);
    messageView->Add(controlButton);
}

bool UITestClockDial::OnClick(UIView& view, const ClickEvent& event)
{
    int length = sizeof(g_transitionMapper) / sizeof(g_transitionMapper[0]);
    g_modelIndex = (g_modelIndex + 1) % length;  // 按钮循环显示文本
    controlButton->SetText(g_transitionMapper[g_modelIndex]);
    SwitchFlipTypeMode(g_modelIndex);
    if (callback == nullptr) {
        GRAPHIC_LOGE("callback new fail\n");
        return false;
    }
    callback->SetContainer(mainSwipeGroup);
    mainSwipeGroup->RegisterSwipeCallback(callback);
    return true;
}

void UITestClockDial::VertViewLoad(void)
{
    mainSwipeGroup->VerAdd(dropDownView, UICrossView::VPage::UP_PAGE);
    mainSwipeGroup->VerAdd(messageView, UICrossView::VPage::DOWN_PAGE);
    GRAPHIC_LOGD("UITestClockDial::PullViewLoad::Pull View Load complete\n");
}

AppGroupView* UITestClockDial::InitWatchFacePage(void)
{
    mainClockView = new MainClockView();
    if (mainClockView == nullptr) {
        GRAPHIC_LOGE("MainClockView new fail\n");
        return nullptr;
    }
    return mainClockView;
}

AppGroupView* UITestClockDial::InitHeartRatePage(void)
{
    heartRateView = new HeartRateView();
    if (heartRateView == nullptr) {
        GRAPHIC_LOGE("heartRateView new fail\n");
        return nullptr;
    }
    return heartRateView;
}

AppGroupView* UITestClockDial::InitActivityPage(void)
{
    activityWeekView = new ActivityWeekView();
    if (activityWeekView == nullptr) {
        GRAPHIC_LOGE("activityWeekView new fail\n");
        return nullptr;
    }
    return activityWeekView;
}

AppGroupView* UITestClockDial::InitCompassPage(void)
{
    compassControlView = new CompassControlView();
    if (compassControlView == nullptr) {
        GRAPHIC_LOGE("compassControlView new fail\n");
        return nullptr;
    }
    return compassControlView;
}

AppGroupView* UITestClockDial::GreatePage(int32_t streamId)
{
    uint16_t i;
    AppGroupView* view = nullptr;
    int32_t cardId = ScreenModel::GetInstance()->GetCardPage(streamId);
    GRAPHIC_LOGD("GreatePage cardId:%d\n", cardId);
    for (i = 0; i < sizeof(PAGE_MAPPER) / sizeof(PAGE_MAPPER[0]); i++) {
        if (PAGE_MAPPER[i].id == static_cast<CardId>(cardId)) {
            view = (this->*(PAGE_MAPPER[i].func))();
            GRAPHIC_LOGD("GreatePage view:%p\n", view);
            return view;
        }
    }
    return nullptr;
}

void UITestClockDial::PreLoad(uint16_t index)
{
    GRAPHIC_LOGD("PreLoad index: %d\n", index);
    if (index == 0) {
        if (!pageStatus[maxPage - 1]) {
            if (!recordCard[maxPage - 1]->InitView()) {
                GRAPHIC_LOGE("PreLoad PAGE:%d\n", maxPage - 1);
                return;
            }
            pageStatus[maxPage - 1] = true;
        }
    } else {
        if (!pageStatus[index - 1]) {
            if (!recordCard[index - 1]->InitView()) {
                GRAPHIC_LOGE("PreLoad PAGE:%u\n", index - 1);
                return;
            }
            pageStatus[index - 1] = true;
        }
    }
    if (index + 1 > maxPage - 1) {
        return;
    }
    if (!pageStatus[index + 1]) {
        if (!recordCard[index + 1]->InitView()) {
            GRAPHIC_LOGE("PreLoad PAGE:%u\n", index + 1);
            return;
        }
        pageStatus[index + 1] = true;
    }
}

void UITestClockDial::AnimatorStart(int pageIndex)
{
    if (heartRateView != nullptr &&
        pageIndex == ScreenModel::GetInstance()->GetCardPage(static_cast<uint16_t>(CardId::HEART_RATE))) {
        heartRateView->HeartbeatImageAnimatorStart();
    }
}

void UITestClockDial::AnimatorStop(void)
{
    if (heartRateView != nullptr) {
        heartRateView->HeartbeatImageAnimatorStop();
    }
}
}  // namespace OHOS
