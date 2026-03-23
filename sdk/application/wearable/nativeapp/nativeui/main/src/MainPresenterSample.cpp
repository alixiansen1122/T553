/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainPresenterSample
 * Author:
 * Create: 2021-10-18
 */

#include "main/MainPresenterSample.h"
#include "common/task_manager.h"
#include "player/PlayersModel.h"
#include "player/PlayersPresenter.h"
#include "NativeRegisterManager.h"
#include "compass/CompassModel.h"
#include "drag_event.h"
#include "graphic_service.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "ui_resource_image.h"
#include <iostream>
#include <map>
#include "main/dial/ModelDialDataFactory.h"
#include "main/dial/DialModelTime.h"
#include "main/dial/DialViewGroup.h"
#include "uiservice/ui_service.h"
#include "PageTransitionMgr.h"
#include "findphone/FindPhoneView.h"
#include "settings/model/SettingCardModel.h"
#include "clock/GradientClockView.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {

REGIST_SLICE(VIEW_MAIN_SAMPLE, MainViewSample, MainPresenterSample);

std::map<std::string, AppViewId> g_jumpView = { { MAIN_CLICK_WEATHER, VIEW_MAIN_SAMPLE },            // 天气界面
                                                { MAIN_CLICK_ACTIVITY, VIEW_MAIN_ACTIVITY },         // 活动记录界面
                                                { MAIN_CLICK_MESSAGE, VIEW_MAIN_SAMPLE },            // 消息app
                                                { DROPDOWN_CLICK_ALARM, VIEW_MAIN_ALARM },           // 闹钟app
                                                { DROPDOWN_CLICK_SETTING, VIEW_SETTING } };          // 设置界面

std::map<std::string, int> g_chanColor = { { DROPDOWN_CLICK_FINDMYPHONE, DROPDOWN_DROPDOWN_FIND_PHONE_B },
                                           { DROPDOWN_CLICK_ALARM, DROPDOWN_DROPDOWN_ALARM_B },
                                           { DROPDOWN_CLICK_SETTING, DROPDOWN_DROPDOWN_SETTING_B } };

static MainPresenterSample *g_pMainPresenterSample = nullptr;

static constexpr uint16_t DROP_X_MAX = 360;
static constexpr uint16_t HORIZONTAL_SWIPE = 1;
static constexpr uint16_t MAX_PERIOD = 1000;

MainPresenterSample::MainPresenterSample()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainPresenterSample :%p", this);
    g_pMainPresenterSample = this;
}

MainPresenterSample::~MainPresenterSample()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~MainPresenterSample");
}

MainPresenterSample *MainPresenterSample::GetInstance(void)
{
    return g_pMainPresenterSample;
}

void MainPresenterSample::OnStart()
{
    mainPresenterState_ = MainPresenterState::START;
}

void MainPresenterSample::OnPause()
{
    // Save watchface content for slide back
    UIView* watchFace = view_->GetViewById(WATCH_FACE)->GetParent();
    uint8_t opaScale = watchFace->GetOpaScale();
    watchFace->SetOpaScale(OPA_OPAQUE);
    PageTransitionMgr::GetInstance().SaveSlideBackSnapShotIfNecessary(watchFace);
    watchFace->SetOpaScale(opaScale);

    uint8_t curCardId = view_->GetHorCurrentCard();
    UICardPage *card = view_->GetViewById(curCardId);
    if (card != nullptr) {
        card->OnPause();
    }
    mainPresenterState_ = MainPresenterState::PAUSE;
    Deinit();
}

void MainPresenterSample::OnResume(void)
{
    mainPresenterState_ = MainPresenterState::RESUME;
    // Set current page again to invoke OnActive method.
    uint8_t curCardId = view_->GetHorCurrentCard();
    view_->SwitchToCard(curCardId);
    Init();
}

void MainPresenterSample::OnStop(void)
{
    mainPresenterState_ = MainPresenterState::STOP;
    if (isCompassSensorOpened_) {
        CompassModel::GetInstance()->CloseCompassSensor();
    }
}

MainPresenterState MainPresenterSample::GetMainPresenterState(void)
{
    return mainPresenterState_;
}

bool MainPresenterSample::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StatusButtonClickListener: %s", view.GetViewId());
    std::string curViewId = view.GetViewId();

    UIImageView *imgView = static_cast<UIImageView *>(&view);
    if (HealthModel::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "health model nullptr");
        return false;
    }
    if (curViewId == DROPDOWN_CLICK_DONOTDISTURB) {
        if (HealthModel::GetInstance()->GetDntDistStatus() == 0) {
            ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, imgView, DROPDOWN_DROPDOWN_DN_DISTURB_B);
            HealthModel::GetInstance()->UpdateDntDistStatus(1);
        } else {
            ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, imgView, DROPDOWN_DROPDOWN_DN_DISTURB);
            HealthModel::GetInstance()->UpdateDntDistStatus(0);
        }
    }
    if (curViewId == DROPDOWN_CLICK_BRIGHTENSCREEN) {
        if (HealthModel::GetInstance()->GetBriScrStatus() == 0) {
            ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, imgView, DROPDOWN_DROPDOWN_BRI_SCREEN_B);
            HealthModel::GetInstance()->UpdateBriScrStatus(1);
            ScreenOffEnable(0);
        } else {
            ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, imgView, DROPDOWN_DROPDOWN_BRI_SCREEN);
            HealthModel::GetInstance()->UpdateBriScrStatus(0);
            ScreenOffEnable(1);
        }
    }
    if (curViewId == DROPDOWN_CLICK_FINDMYPHONE) {
        NativeAbility::GetInstance().SwitchSlice(VIEW_FIND_PHONE, FIND_PHONE_PAGES::FIND_PHONE_MAIN_PAGE);
    }

    if (g_jumpView.count(curViewId) != 0) {
        if (g_jumpView[curViewId] == VIEW_SETTING) {
            NativeAbility::GetInstance().ChangeSlice(g_jumpView[curViewId], TransitionType::TRANSITION_INVALID,
                gSliceDefaultPriority, true);
        } else {
            NativeAbility::GetInstance().ChangeSlice(g_jumpView[curViewId]);
        }
    }
    return true;
}

bool MainPresenterSample::OnLongPress(UIView& view, const LongPressEvent& event)
{
    UNUSED(event);
    UNUSED(view);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainPresenterSample OnLongPress");
    uint8_t cardId = view_->GetHorCurrentCard();
    if (cardId != WATCH_FACE) {
        SettingCardModel::GetInstance().SetSelectedCard(cardId);
        NativeAbility::GetInstance().SwitchSlice(VIEW_SETTING, SettingPageId::CARD_MGR_PAGE,
            TransitionType::TRANSITION_INVALID, true);
    } else {
        NativeAbility::GetInstance().SwitchSlice(VIEW_SETTING, SettingPageId::DESKTOP_DIAL_PAGE,
            TransitionType::TRANSITION_INVALID, true);
    }
    return true;
}

void MainPresenterSample::SetPage(int8_t id, bool loadAdjacent)
{
    if (view_ != nullptr) {
        view_->SetPage(id, loadAdjacent);
    }
}

void MainPresenterSample::SetTaskPeroid(CardId cardId)
{
    SetLastRun(0); // refresh entering card in time
    PeriodicUpdateInterface* page = dynamic_cast<PeriodicUpdateInterface*>(view_->GetViewById(cardId));
    if (page != nullptr) {
        page->Update();
        SetPeriod(page->GetPeriod());
    } else if (cardId == WATCH_FACE) {
        DialSetting &setting = GetDialSetting();
        if (setting.dialFlag) {
            SetPeriod(dynamic_cast<DialViewGroup*>(view_->GetViewById(WATCH_FACE))->GetPeriod());
        }
    } else {
        SetPeriod(MAX_PERIOD);
    }
}

void MainPresenterSample::OnSwipe(UISwipeView &view)
{
    if (view.GetDirection()) {
        view_->GetVerCurrentCard();
    } else {
        CardId cardId = view_->GetHorCurrentCard();
        SetTaskPeroid(cardId);
        if (cardId == COMPASS) {
            if (!isCompassSensorOpened_) {
                CompassModel::GetInstance()->OpenCompassSensor();
                isCompassSensorOpened_ = true;
            }
        } else if (isCompassSensorOpened_) {
            CompassModel::GetInstance()->CloseCompassSensor();
            isCompassSensorOpened_ = false;
        }
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnSwipe OnSwipe");
    return;
}

void MainPresenterSample::ProcessDataNotify(void)
{
    HealthMsg healthMsg;
    uint16_t len;

    while ((HealthModel::GetInstance()->FetchedHealthData(&healthMsg))) {
        switch (healthMsg.type) {
            case BATTERY_LIFE:
                break;
            default:
                break;
        }
    }
}

void MainPresenterSample::Callback(void)
{
    CardId cardId = view_->GetHorCurrentCard();
    PeriodicUpdateInterface* page = dynamic_cast<PeriodicUpdateInterface*>(view_->GetViewById(cardId));
    if (page != nullptr) {
        page->Update();
    } else if (cardId == WATCH_FACE) {
        DialSetting &setting = GetDialSetting();
        if (setting.dialFlag) {
            DialModelTime::GetInstance()->UpdateTime();
            DialViewGroup* group = dynamic_cast<DialViewGroup*>(view_->GetViewById(WATCH_FACE));
            if (group != nullptr) {
                group->UpdateViewsByPeriodicUpdateData();
            }
        }
    }
}

void MainPresenterSample::InitCardSettings(void)
{
    MainModel::GetInstance().InitCardSettings();
}

uint8_t MainPresenterSample::GetCardSettingCount(void)
{
    return MainModel::GetInstance().GetCardSettingCount();
}

CardId *MainPresenterSample::GetCardSettings(void)
{
    return MainModel::GetInstance().GetCardSettings();
}

void MainPresenterSample::InitDialSettings(void)
{
    return MainModel::GetInstance().InitDialSettings();
}
DialSetting& MainPresenterSample::GetDialSetting()
{
    return MainModel::GetInstance().GetDialSetting();
}

bool MainPresenterSample::IsFromSetCardView(void) const
{
    return MainModel::GetInstance().IsFromSetCardView();
}

void MainPresenterSample::SetFromSetCardView(bool isFrom)
{
    MainModel::GetInstance().SetFromSetCardView(isFrom);
}

uint8_t MainPresenterSample::GetFromCardId(void)
{
    return MainModel::GetInstance().GetFromCardId();
}

}
