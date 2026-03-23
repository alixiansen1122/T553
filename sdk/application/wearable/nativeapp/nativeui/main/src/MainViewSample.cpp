/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainViewSample
 * Author:
 * Create: 2021-10-18
 */
#include "wearable_log.h"
#include "main/MainViewSample.h"
#include "graphic_service.h"
#include "common/image_cache_manager.h"
#include "swipe_effect/turn_page_effect.h"
#include "swipe_effect/push_press_effect.h"
#include "swipe_effect/cube_effect.h"
#include "swipe_effect/windmill_effect.h"
#include "swipe_effect/card_flip_effect.h"
#include "swipe_effect/zoom_effect.h"
#include "settings/model/SettingCardModel.h"
#include "clock/GradientClockView.h"
#include "UIWatchDialFactory.h"

namespace OHOS {
static MainViewSample *g_pMainView = nullptr;
static constexpr uint16_t MESLABLE_X_TMP = 177;
static constexpr uint16_t MESLABLE_Y_TMP = 212;
static constexpr uint16_t MESLABLE_WIDTH_TMP = 100;
static constexpr uint16_t MESLABLE_HEIGHT_TMP = 30;
static constexpr uint8_t FONT_SIZE_TMP = 24;
static constexpr uint8_t PULL_MIDDLE_VIEW_OPA = 0;
static constexpr uint8_t PAGE_ID = 2;

struct MainViewMapper {
    CardId id;
    UICardPage *(MainViewSample::*func)(void);
};

static const MainViewMapper CardMapper[] = {
    {WATCH_FACE, &MainViewSample::InitWatchFacePage},
    {COMPASS, &MainViewSample::InitCompassPage},
    {ACTIVITY, &MainViewSample::InitActivityPage},
    {MUSIC_PLAYER, &MainViewSample::InitMusicPlayerPage},
    {VIDEO_CARD1, &MainViewSample::InitVideoCardPage1},
    {VIDEO_CARD2, &MainViewSample::InitVideoCardPage2},
    {VIDEO_CARD3, &MainViewSample::InitVideoCardPage3},
};

MainViewSample::MainViewSample()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainViewSample::MainViewSample");
    g_pMainView = this;
}

MainViewSample::~MainViewSample()
{
    dropDownView->ClearBackgroundBlur();
    messageView->ClearBackgroundBlur();

    for (int32_t i = 0; i < MAX_CARD; i++) {
        recordCard[i] = nullptr;
        pageStatus[i] = false;
    }

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

    if (playersView != nullptr) {
        delete playersView;
        playersView = nullptr;
    }

    bool hasVideo = false;
    if (videoCard1 != nullptr) {
        hasVideo = true;
        delete videoCard1;
        videoCard1 = nullptr;
    }

    if (videoCard2 != nullptr) {
        hasVideo = true;
        delete videoCard2;
        videoCard2 = nullptr;
    }

    if (videoCard3 != nullptr) {
        hasVideo = true;
        delete videoCard3;
        videoCard3 = nullptr;
    }

    if (hasVideo) {
        ImageCacheManager::GetInstance().UnloadSingleRes(PNG_PREVIEW_CARD_VIDEO);
    }

    if (dropDownView != nullptr) {
        delete dropDownView;
        dropDownView = nullptr;
    }

    if (messageView != nullptr) {
        messageView->RemoveAll();
        delete messageView;
        messageView = nullptr;
    }

    if (compassControlView != nullptr) {
        delete compassControlView;
        compassControlView = nullptr;
    }

    if (callback != nullptr) {
        delete callback;
        callback = nullptr;
    }

    g_pMainView = nullptr;
}

MainViewSample *MainViewSample::GetInstance(void)
{
    return g_pMainView;
}

bool MainViewSample::SetCardEffect(CardEffectID style)
{
    if (mainSwipeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainSwipeGroup is nullptr");
        return false;
    }
    if (callback != nullptr) {
        delete callback;
        callback = nullptr;
    }

    std::map<CardEffectID, std::function<OHOS::CardSwipe*()>> effectMap = {
        {CardEffectID::UI_CARD_FLIP, []{ return new CardFlipEffect(); }},
        {CardEffectID::UI_ZOOM, []{ return new ZoomEffect(); }},
        {CardEffectID::UI_CUBE, []{ return new CubeEffect(); }},
        {CardEffectID::UI_TURN_PAGE, []{ return new TurnPageEffect(); }},
        {CardEffectID::UI_PUSH_PRESS, []{ return new PushPressEffect(); }},
        {CardEffectID::UI_WINDMILL, []{ return new WindmillEffect(); }},
    };
    auto it = effectMap.find(style);
    if (it != effectMap.end()) {
        callback = it->second();
    } else {
        callback = new ZoomEffect(); // ZoomEffect is used by default
    }

    if (callback == nullptr) {
        delete mainSwipeGroup;
        mainSwipeGroup = nullptr;
        GRAPHIC_LOGE("Effect new fail\n");
        return false;
    }

    callback->SetContainer(mainSwipeGroup);
    mainSwipeGroup->RegisterSwipeCallback(callback);
    return true;
}

void MainViewSample::OnStart()
{
    mainSwipeGroup = new UICrossView();
    if (mainSwipeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainSwipeGroup new fail");
        return;
    }

    SettingCardModel& swipeEffectModel = SettingCardModel::GetInstance();
    if (!SetCardEffect(swipeEffectModel.GetSwipeEffectStyle())) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SetCardEffect fail");
        return;
    }

    HorizontalViewInit();
    HorViewLoad();
    VerticalViewInit();
    VertViewLoad();
    mainSwipeGroup->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    mainSwipeGroup->SetAnimatorTime(50); // 50: animator drag time(ms);
    mainSwipeGroup->SetLoopState(true);
    mainSwipeGroup->EnableScreenCap(true);
    if (presenter_->IsFromSetCardView()) {
        presenter_->SetFromSetCardView(false);
        SwitchToCard(presenter_->GetFromCardId(), false);
    } else {
        SwitchToClockPage(false);
    }
    AddViewToRootContainer(mainSwipeGroup);
}

bool MainViewSample::HorizontalViewInit(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainViewSample::HorizontalViewInit");
    presenter_->InitDialSettings();
    presenter_->InitCardSettings();
    uint8_t cardSettingNum = presenter_->GetCardSettingCount();
    CardId *cardSettings = presenter_->GetCardSettings();
    uint8_t cardMapNumber = sizeof(CardMapper) / sizeof(CardMapper[0]);
    for (int16_t i = 0; i < cardSettingNum; i++) {
        for (uint8_t j = 0; j < cardMapNumber; j++) {
            if (cardSettings[i] == CardMapper[j].id) {
                recordCard[j] = (this->*(CardMapper[j].func))();
                if (recordCard[j] == nullptr) {
                    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                                  "MainViewSample:: recordCard %d error", j);
                    return false;
                }
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
                              "MainViewSample:: recordCard[%d]:%p", j, recordCard[j]);
                break;
            }
        }
    }
    mainSwipeGroup->SetOnSwipeListener(presenter_);
    mainSwipeGroup->SetOnLongPressListener(presenter_);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainViewSample::InitPage over");
    return true;
}

void MainViewSample::HorViewLoad(void)
{
    uint8_t cardSettingNum = presenter_->GetCardSettingCount();
    CardId *cardSettings = presenter_->GetCardSettings();
    for (int16_t i = 0; i < cardSettingNum; i++) {
        CardId cardId = cardSettings[i];
        if (recordCard[cardId] != nullptr) {
            mainSwipeGroup->HorAdd(recordCard[cardId]);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainViewSample:: HorViewLoad cardid = %d ViewIndex = %d",
                          cardId, recordCard[cardId]->GetViewIndex());
        }
    }
}

void MainViewSample::VerticalViewInit(void)
{
    dropDownView = new PullUpDownView();
    if (dropDownView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "dropDownView new fail");
        return;
    }
    dropDownView->PreLoad();
    dropDownView->SetViewId("dropDownView");
    dropDownView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    dropDownView->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    messageView = new MessageMainPage();
    if (messageView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "messageView new fail");
        return;
    }
    messageView->PreLoad();
    messageView->SetPosition(0, 0);
    messageView->SetWidth(Screen::GetInstance().GetWidth());
    messageView->SetHeight(Screen::GetInstance().GetHeight());
    messageView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    messageView->SetViewId("messageView");
    messageView->SetTouchable(true);
    messageView->SetIntercept(true);
}

void MainViewSample::VertViewLoad(void)
{
    mainSwipeGroup->VerAdd(dropDownView, UICrossView::VPage::UP_PAGE);
    mainSwipeGroup->VerAdd(messageView, UICrossView::VPage::DOWN_PAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainViewSample::PullViewLoad::Pull View Load complete");
}

void MainViewSample::OnStop()
{
    messageView->RemoveAll();
    return;
}

UICardPage *MainViewSample::InitWatchOffDial(DialSetting &setting)
{
    DialViewGroup *dial = new DialViewGroup();
    dial->SetDial(setting.dialFullName);
    return dial;
}

UICardPage *MainViewSample::InitWatchDial(DialSetting &setting)
{
    return dynamic_cast<UICardPage*>(UIWatchDialFactory::GetInstance().CreateNormalDial(setting.dialId));
}

UICardPage *MainViewSample::InitWatchFacePage(void)
{
    DialSetting &setting = presenter_->GetDialSetting();
    if (setting.dialFlag) {
        mainClockView = InitWatchOffDial(setting);
    } else {
        mainClockView = InitWatchDial(setting);
    }

    if (mainClockView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainClockView new fail");
        return nullptr;
    }
    return mainClockView;
}

UICardPage *MainViewSample::InitActivityPage(void)
{
    activityWeekView = new ActivityWeekView();
    if (activityWeekView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "activityWeekView new fail");
        return nullptr;
    }
    return activityWeekView;
}

UICardPage *MainViewSample::InitCompassPage(void)
{
    compassControlView = new CompassControlView();
    if (compassControlView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "compassControlView new fail");
        return nullptr;
    }
    return compassControlView;
}

UICardPage *MainViewSample::InitMusicPlayerPage(void)
{
    playersView = new PlayersView();
    if (playersView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playersView new fail");
        return nullptr;
    }
    return playersView;
}

UICardPage *MainViewSample::InitVideoCardPage1(void)
{
    videoCard1 = dynamic_cast<VideoCard*>(InitVideoCardPage(APP_VIDEO_PATH"/noaudio_05.mp4", PNG_PREVIEW_CARD_VIDEO));
    return videoCard1;
}

UICardPage *MainViewSample::InitVideoCardPage2(void)
{
    videoCard2 = dynamic_cast<VideoCard*>(InitVideoCardPage(APP_VIDEO_PATH"/noaudio_05.mp4", PNG_PREVIEW_CARD_VIDEO));
    return videoCard2;
}

UICardPage *MainViewSample::InitVideoCardPage3(void)
{
    videoCard3 = dynamic_cast<VideoCard*>(InitVideoCardPage(APP_VIDEO_PATH"/noaudio_05.mp4", PNG_PREVIEW_CARD_VIDEO));
    return videoCard3;
}

UICardPage *MainViewSample::InitVideoCardPage(const std::string &video, const std::string &preview)
{
    VideoCard *page = new VideoCard;
    if (page == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "videocard new fail");
        return nullptr;
    }
    page->SetFilePath(video);
    page->SetPreviewFile(preview);
    return page;
}

void MainViewSample::SetPage(int16_t page, bool loadAdjacent)
{
    if (mainSwipeGroup != nullptr) {
        mainSwipeGroup->SetHorCurrentPage(page, loadAdjacent);
    }

    if (presenter_ != nullptr) {
        CardId *cardSettings = presenter_->GetCardSettings();
        /* page 与CardId 没有对应关系，需要一次转换 */
        CardId id = MAX_CARD;
        for (uint8_t i = 0; i < presenter_->GetCardSettingCount(); ++i) {
            if (cardSettings[i] < MAX_CARD && recordCard[cardSettings[i]] != nullptr &&
                recordCard[cardSettings[i]]->GetViewIndex() == page) {
                id = cardSettings[i];
            }
        }
        presenter_->SetTaskPeroid(id);
    }
}

void MainViewSample::ReloadWatchFace()
{
    if (mainSwipeGroup == nullptr) {
        return;
    }
    mainSwipeGroup->Remove(mainClockView, UISwipeView::HORIZONTAL);

    delete mainClockView;
    mainClockView = nullptr;
    recordCard[WATCH_FACE] = InitWatchFacePage();
    if (recordCard[WATCH_FACE] == nullptr) {
        GRAPHIC_LOGE("failed to reload main clock");
        return;
    }
    mainSwipeGroup->HorInsert(nullptr, mainClockView);

    SwitchToCard(WATCH_FACE);
    Draw();
}

bool MainViewSample::IsMainClockPage(void)
{
    if (mainSwipeGroup->GetCurrentPageId(0) == mainClockView->GetViewIndex() &&
        mainSwipeGroup->GetCurrentPageId(1) == 1) { // 0: horizontal; 1: vertical
        return true;
    } else {
        return false;
    }
}

CardId MainViewSample::GetHorCurrentCard() const
{
    CardId *cardSettings = presenter_->GetCardSettings();
    int pageId = mainSwipeGroup->GetCurrentPageId(UISwipeView::HORIZONTAL); // 0: horizontal; 1: vertical
    /* pageId 与CardId 没有对应关系，需要一次转换 */
    for (uint8_t i = 0; i < presenter_->GetCardSettingCount(); ++i) {
        if (cardSettings[i] < MAX_CARD && recordCard[cardSettings[i]] != nullptr &&
            recordCard[cardSettings[i]]->GetViewIndex() == pageId) {
            return cardSettings[i];
        }
    }
    return MAX_CARD;
}

void MainViewSample::GetVerCurrentCard() const
{
    int pageId = mainSwipeGroup->GetCurrentPageId(UISwipeView::VERTICAL);
    if (pageId == PAGE_ID) {
        MessageModel::GetInstance().SetTime();
        std::vector<MessageItem> &itemInfo = MessageModel::GetInstance().GetMsgListItems();
        MessageMainPage::GetInstance()->RefreshMsgList(itemInfo);
    }
}

void MainViewSample::SwitchToCard(uint8_t cardId, bool loadAdjacent)
{
    if ((cardId < MAX_CARD) && (recordCard[cardId] != nullptr)) {
        SetPage(recordCard[cardId]->GetViewIndex(), loadAdjacent);
    } else {
        SwitchToClockPage(loadAdjacent);
    }
}
} // OHOS
