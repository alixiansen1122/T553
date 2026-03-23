/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainViewSample
 * Author:
 * Create:
 */

#ifndef MAIN_VIEW_SAMPLE_H
#define MAIN_VIEW_SAMPLE_H

#include <string>
#include "View.h"
#include "components/root_view.h"
#include "swipe_effect/zoom_effect.h"
#include "ChangeSliceListener.h"
#include "font/ui_font.h"
#include "graphic_config.h"
#include "screensetting/ScreenModels.h"
#include "activity/ActivityWeekView.h"
#include "compass/CompassControlView.h"
#include "main/dial/DialViewGroup.h"
#include "clock/MainClockView.h"
#include "player/PlayersView.h"
#include "main/MainPresenterSample.h"
#include "components/ui_card_page.h"
#include "main/PullUpDownView.h"
#include "components/ui_cross_view.h"
#include "settings/model/SettingCardModel.h"
#include "settings/common/SettingCardEffectSample.h"
#include "videocard/VideoCard.h"
#include "main/MessageMainPage.h"

namespace OHOS {
class MainPresenterSample;
class MainViewSample : public View<MainPresenterSample> {
public:
    MainViewSample();
    ~MainViewSample() override;
    static MainViewSample *GetInstance(void);
    void OnStart() override;
    void OnStop() override;
    UICardPage *InitWatchFacePage(void);
    UICardPage *InitActivityPage(void);
    UICardPage *InitCompassPage(void);
    UICardPage *InitMusicPlayerPage(void);
    UICardPage *InitVideoCardPage1(void);
    UICardPage *InitVideoCardPage2(void);
    UICardPage *InitVideoCardPage3(void);
    void AnimatorStart(int pageIndex);
    void AnimatorStop(void);
    CardId GetHorCurrentCard() const;
    void GetVerCurrentCard() const;
    UIView* GetMainViewGroup(void) const
    {
        return mainSwipeGroup;
    }

    UICardPage* GetViewById(int16_t id) const
    {
        if (id >= MAX_CARD) {
            return nullptr;
        }
        return recordCard[id];
    }

    void SwitchToClockPage(bool loadAdjacent = true)
    {
        if (mainClockView != nullptr) {
            SetPage(mainClockView->GetViewIndex(), loadAdjacent);
        }
    }
    void SwitchToCard(uint8_t cardId, bool loadAdjacent = true);
    bool IsMainClockPage(void);
    void SetPage(int16_t page, bool loadAdjacent = true);
    void ReloadWatchFace();

private:
    PullUpDownView *dropDownView{nullptr};
    MessageMainPage *messageView{nullptr};
    UICardPage *mainClockView{nullptr};
    ActivityWeekView *activityWeekView{nullptr};
    PlayersView *playersView{nullptr};
    CompassControlView *compassControlView{nullptr};
    UICardPage *recordCard[MAX_CARD]{nullptr};
    bool pageStatus[MAX_CARD]{false};
    UICrossView *mainSwipeGroup{nullptr};
    CardSwipe* callback{nullptr};
    VideoCard *videoCard1{nullptr};
    VideoCard *videoCard2{nullptr};
    VideoCard *videoCard3{nullptr};

    bool HorizontalViewInit(void);
    void VerticalViewInit(void);
    void VertViewLoad(void);
    void HorViewLoad(void);
    UICardPage *InitWatchOffDial(DialSetting &setting);
    UICardPage *InitWatchDial(DialSetting &setting);
    UICardPage *InitVideoCardPage(const std::string &video, const std::string &preview);
    bool SetCardEffect(CardEffectID style);
};
} // namespace OHOS
#endif // MAIN_VIEW_SAMPLE_H
