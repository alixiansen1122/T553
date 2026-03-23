/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersView
 * Create: 2025-03-23
 */

#ifndef PLAYERS_VIEW_H
#define PLAYERS_VIEW_H

#include <string>
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_abstract_scroll.h"
#include "components/ui_circle_progress.h"
#include "components/ui_checkbox.h"
#include "components/ui_simple_list.h"
#include "font/ui_font_bitmap.h"
#include "font/ui_font.h"
#include "graphic_config.h"
#include "components/ui_card_page.h"
#include "main/VolumeSettingView.h"
#include "player/PlayersPresenter.h"
#include "player/PlayersOptionGroup.h"
#include "player/PlayersMusicListGroup.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {
#define PLAY_PROGRESS_MIN 0
#define PLAY_PROGRESS_MAX 100
class PlayersPresenter;
class VolumeSettingView;
class VolumeSliderEventListener;
class PlayersView : public UICardPage, public PeriodicUpdateInterface,
                    public UIView::OnClickListener,
                    public UIView::OnDragListener {
public:
    PlayersView();
    ~PlayersView() override;
    static PlayersView *GetInstance(void);
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool InitView(void);
    void PreLoad(void) override;
    void UnLoad(void) override;
    void OnInactive(void) override;
    void InitButton();
    void InitLabel();
    void InitImage();
    void InitCircleProgress();
    void InitSoundAdjust();
    void InitOption();
    void InitListView(void);
    void SetUpPlayerMode();
    void PlayOrPauseButtonOnClick();
    void TitleSetText(const char *text);
    void PlayerModelButtonSetSrc(uint32_t playMode);
    void SetPlaybackProgress(int8_t value);
    UICheckBox::UICheckBoxState GetPlayerButtonState(void);
    void SetPlayerState(bool value);
    void SetPlayerButtonState(UICheckBox::UICheckBoxState state);
    void PlayerHeadsetReset();

    void SetPlayerCtrVolume(float leftVolume, float rightVolume);
    int16_t GetVolumeRank(void);
    void SetVolumeRank(int16_t value);
    void SetVolumeAddOne(void);
    void Update(void) override;
    uint32_t GetPeriod(void) override;
private:
    UIImageView *bgImg_{nullptr};
    UILabel *musicNameLabel_{nullptr};
    UILabel *musicAuthorLabel_{nullptr};
    UIImageView *optionsButton_{nullptr};
    UIImageView *preButton_{nullptr};
    UIImageView *nextButton_{nullptr};
    UIImageView *soundButton_{nullptr};
    UIImageView *playModelChangeButton_{nullptr};
    UIImageView *pullButton_{nullptr};
    UICheckBox *playerOrPauseButton_{nullptr};
    UICircleProgress *playbackProgress_{nullptr};
    VolumeSettingView *volumeSettingView_{nullptr};
    VolumeSliderEventListener *volumeSliderEventListener_{nullptr};
    PlayersMusicListGroup *playersMusicListGroup{nullptr};
    PlayersOptionGroup *playerOptionGroup_{nullptr};
    int16_t volumeSet{0};
    uint8_t state_ = UICheckBox::UICheckBoxState::MAX_STATUS_NUM;
    bool viewiInitStatus{false};
    int64_t playerCurrentTime_{0};
    int64_t playerMusicDuration_{0};
    int64_t playerProgressValue_{0};
};
} // namespace OHOS
#endif // MAIN_VIEW_H
