/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainPresenterSample
 * Author:
 * Create:
 */

#ifndef MAIN_PRESENTER_SAMPLE_H
#define MAIN_PRESENTER_SAMPLE_H

#include "Presenter.h"
#include "components/root_view.h"
#include "drag_event.h"
#include "main/MainViewSample.h"
#include "main/MainModel.h"
#include "components/ui_swipe_view.h"
#include "components/ui_view.h"
#include "compass/MainCompassView.h"
#include "compass/MainCompassPresenter.h"
#include "player/PlayersView.h"
#include "common/task.h"

namespace OHOS {
static constexpr const char *MAIN_CLICK_WEATHER = "mainclickweather";
static constexpr const char *MAIN_CLICK_ACTIVITY = "mainclickactivity";
static constexpr const char *MAIN_CLICK_MESSAGE = "mainclickmessage";

static constexpr const char *DROPDOWN_CLICK_DONOTDISTURB = "dropdownclickdonotdisturb";
static constexpr const char *DROPDOWN_CLICK_BRIGHTENSCREEN = "dropdownclickbrightscreen";
static constexpr const char *DROPDOWN_CLICK_FINDMYPHONE = "dropdownclickfindmyphone";
static constexpr const char *DROPDOWN_CLICK_ALARM = "dropdownclickalarm";
static constexpr const char *DROPDOWN_CLICK_SETTING = "dropdownclicksetting";
constexpr uint16_t VERTICAL_STATUS = 1;
constexpr uint16_t HORIZONTAL_STATUS = 0;
enum class MainPresenterState {
    START,
    RESUME,
    PAUSE,
    STOP,
};

class MainViewSample;
class MainPresenterSample : public Presenter<MainViewSample>, public UIView::OnClickListener, public Task,
    public UISwipeView::OnSwipeListener, public UIView::OnLongPressListener, public UISwipeView {
public:
    MainPresenterSample();
    ~MainPresenterSample() override;
    static MainPresenterSample *GetInstance(void);

    void OnStart() override;
    void OnPause() override;
    void OnResume() override;
    void OnStop() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnLongPress(UIView& view, const LongPressEvent& event) override;
    void OnSwipe(UISwipeView& view) override;
    void SetPage(int8_t id, bool loadAdjacent = true);
    void SetTaskPeroid(CardId cardId);
    void InitCardSettings(void);
    uint8_t GetCardSettingCount(void);
    CardId *GetCardSettings(void);
    void InitDialSettings(void);
    DialSetting& GetDialSetting();
    bool IsFromSetCardView(void) const;
    void SetFromSetCardView(bool isFrom);
    uint8_t GetFromCardId(void);
    MainPresenterState GetMainPresenterState();

private:
    void ProcessDataNotify(void);
    void Callback() override;
    uint8_t state_ = UICheckBox::UICheckBoxState::MAX_STATUS_NUM;
    bool isCompassSensorOpened_{false};
    MainPresenterState mainPresenterState_ = MainPresenterState::STOP;
};
}

#endif // MAIN_PRESENTER_H
