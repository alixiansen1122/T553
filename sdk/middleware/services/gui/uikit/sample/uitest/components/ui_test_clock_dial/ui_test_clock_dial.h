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

#ifndef UI_TEST_ANALOG_CLOCK_H
#define UI_TEST_ANALOG_CLOCK_H

#include "ui_test.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_swipe_view.h"
#include "components/ui_scroll_view.h"
#include "swipe_effect/zoom_effect.h"
#include "swipe_effect/cube_effect.h"
#include "swipe_effect/card_flip_effect.h"
#include "swipe_effect/turn_page_effect.h"
#include "font/ui_font_bitmap.h"
#include "font/ui_font.h"
#include "graphic_config.h"
#include "ScreenModel.h"
#include "ActivityWeekView.h"
#include "CompassControlView.h"
#include "MainClockView.h"
#include "HeartRateView.h"
#include "AppGroupView.h"
#include "PullUpDownView.h"
#include "components/ui_cross_view.h"

namespace OHOS {
class SwipeListener : public UISwipeView::OnSwipeListener {
public:
    SwipeListener(void) {}
    ~SwipeListener() override {}
    void OnSwipe(UISwipeView& view) override;
};

enum class EffectType {
    ZOOM = 0,
    TURN_PAGE,
    CUBE,
    CARD_FLIP,
    ENUM_COUNT,
};
class UITestClockDial : public UITest, public UIView::OnClickListener {
public:
    UITestClockDial();
    ~UITestClockDial() override {}
    static UITestClockDial *GetInstance(void);
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void PreLoad(uint16_t index);
    AppGroupView *InitWatchFacePage(void);
    AppGroupView *InitHeartRatePage(void);
    AppGroupView *InitActivityPage(void);
    bool OnClick(UIView& view, const ClickEvent& event) override;   // 切换特性
    AppGroupView *InitCompassPage(void);
    void AnimatorStart(int pageIndex);
    void AnimatorStop(void);
    uint16_t GetHorCurrentPage() const
    {
        return mainSwipeGroup->GetCurrentPageId(0); // 0: horizontal; 1: vertical
    }

    UICrossView* GetMainViewGroup(void) const
    {
        return mainSwipeGroup;
    }

private:
    int16_t maxPage{0};
    PullUpDownView *dropDownView{nullptr};
    UICardPage *messageView{nullptr};
    UILabel *messageLabel{nullptr};
    MainClockView *mainClockView{nullptr};
    ActivityWeekView *activityWeekView{nullptr};
    CompassControlView *compassControlView{nullptr};
    HeartRateView *heartRateView{nullptr};
    AppGroupView *recordCard[static_cast<uint32_t>(CardId::MAX_CARD)]{nullptr};
    bool pageStatus[static_cast<uint32_t>(CardId::MAX_CARD)]{false};
    UICrossView *mainSwipeGroup{nullptr};
    SwipeListener *listener{nullptr};
    CardSwipe* callback{nullptr};
    UILabelButton* controlButton {nullptr}; // 切换按钮
    bool HorizontalViewInit(void);
    void VerticalViewInit(void);
    void VertViewLoad(void);
    void HorViewLoad(void);
    void SwitchFlipTypeMode(uint8_t mode);
    AppGroupView *GreatePage(int32_t streamId);
};
}
#endif // UI_TEST_ANALOG_CLOCK_H
