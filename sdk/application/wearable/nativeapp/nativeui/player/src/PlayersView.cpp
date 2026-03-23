/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Player view
 * Author:
 * Create: 2021-07-26
 */
#include <cstdlib>
#include <unistd.h>
#include "common/image_cache_manager.h"
#include "phoneservice/PhoneService.h"
#include "main/LoadImg.h"
#include "ui_resource_image.h"
#include "UiConfig.h"
#include "player/PlayersModel.h"
#include "player/PlayersMusicItemView.h"
#include "bts_avrcp_target.h"
#include "bts_avrcp_controller.h"
#include "main/MainViewSample.h"
#include "player/PlayersView.h"

namespace OHOS {
static PlayersView* g_pPlayersView = nullptr;
static constexpr uint16_t PROGRESS_PERCENTAGE = 100;
static constexpr uint16_t PLAYER_PERIOD = 500;

static constexpr int16_t NEXT_BUTTON_POSITION_X = 331;
static constexpr int16_t PRE_BUTTON_POSITION_X = 31;
static constexpr int16_t BUTTON_POSITION_Y_ONE = 168;
static constexpr int16_t PLAY_MODE_BUTTON_POSITION_X = 87;
static constexpr int16_t SOUND_BUTTON_POSITION_X = 176;
static constexpr int16_t OPTION_BUTTON_POSITION_X = 284;
static constexpr int16_t BUTTON_POSITION_Y_TWO = 316;
static constexpr int16_t PULL_BUTTON_POSITION_X = 178;
static constexpr int16_t PULL_BUTTON_POSITION_Y = 394;
static constexpr int16_t PLAY_OR_PAUSE_BUTTON_POSITION_X = 140;
static constexpr int16_t PLAY_OR_PAUSE_BUTTON_POSITION_Y = 139;
static constexpr int16_t PLAY_OR_PAUSE_BUTTON_SIZE = 172;
static constexpr int16_t MUSIC_NAME_LABEL_POSITION_X = 72;
static constexpr int16_t MUSIC_NAME_LABEL_POSITION_Y = 35;
static constexpr int16_t MUSIC_NAME_LABEL_WIDTH = 300;
static constexpr int16_t MUSIC_NAME_LABEL_HEIGHT = 53;
static constexpr int16_t MUSIC_AUTHOR_LABEL_POSITIO0N_X = 77;
static constexpr int16_t MUSIC_AUTHOR_LABEL_POSITION_Y = 92;
static constexpr int16_t MUSIC_AUTHOR_LABEL_WIDTH = 300;
static constexpr int16_t MUSIC_AUTHOR_LABEL_HEIGHT = 38;
static constexpr char* NEXT_SONG_BUTTON = "NEXT_SONG_BUTTON";
static constexpr char* PRE_SONG_BUTTON = "PRE_SONG_BUTTON";
static constexpr char* PLAY_OR_PAUSE_BUTTON = "PLAY_OR_PAUSE_BUTTON";
static constexpr char* PLAYMODE_CHANGE_BUTTON = "PLAYMODE_CHANGE_BUTTON";
static constexpr char* VOICE_ADJUST_BUTTON = "VOICE_ADJUST_BUTTON";
static constexpr char* OPTIONS_BUTTON = "OPTIONS_BUTTON";
static constexpr char* PULL_BUTTON = "PULL_BUTTON";

PlayersView::PlayersView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView::PlayersView");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetDraggable(true);
    SetTouchable(true);
    SetOnClickListener(this);
    SetOnDragListener(this);
    g_pPlayersView = this;
}

PlayersView::~PlayersView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView::~PlayersView");
    if (viewiInitStatus) {
        state_ = GetPlayerButtonState();
        if (state_ == UICheckBox::UICheckBoxState::SELECTED) {
            PlayersModel::GetInstance()->SetPlayButtonState(true);
        }
        RemoveAll();
        if (bgImg_ != nullptr) {
            delete bgImg_;
            bgImg_ = nullptr;
        }
        if (musicNameLabel_ != nullptr) {
            delete musicNameLabel_;
            musicNameLabel_ = nullptr;
        }
        if (musicAuthorLabel_ != nullptr) {
            delete musicAuthorLabel_;
            musicAuthorLabel_ = nullptr;
        }
        if (optionsButton_ != nullptr) {
            delete optionsButton_;
            optionsButton_ = nullptr;
        }
        if (preButton_ != nullptr) {
            delete preButton_;
            preButton_ = nullptr;
        }
        if (nextButton_ != nullptr) {
            delete nextButton_;
            nextButton_ = nullptr;
        }
        if (soundButton_ != nullptr) {
            delete soundButton_;
            soundButton_ = nullptr;
        }
        if (playModelChangeButton_ != nullptr) {
            delete playModelChangeButton_;
            playModelChangeButton_ = nullptr;
        }
        if (pullButton_ != nullptr) {
            delete pullButton_;
            pullButton_ = nullptr;
        }
        if (playerOrPauseButton_ != nullptr) {
            delete playerOrPauseButton_;
            playerOrPauseButton_ = nullptr;
        }
        if (playbackProgress_ != nullptr) {
            delete playbackProgress_;
            playbackProgress_ = nullptr;
        }
        if (volumeSettingView_ != nullptr) {
            delete volumeSettingView_;
            volumeSettingView_ = nullptr;
        }
        if (playerOptionGroup_ != nullptr) {
            delete playerOptionGroup_;
            playerOptionGroup_ = nullptr;
        }
        if (playersMusicListGroup != nullptr) {
            delete playersMusicListGroup;
            playersMusicListGroup = nullptr;
        }
    }
    g_pPlayersView = nullptr;
}

void PlayersView::TitleSetText(const char *text)
{
    if (musicNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersView::TitleSetText musicNameLabel_ is null");
        return;
    }

    if (text && *text) {
        musicNameLabel_->SetText(text);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersView::TitleSetText text nullptr");
        musicNameLabel_->SetText("Music Name");
    }
}

void PlayersView::PlayerModelButtonSetSrc(uint32_t playMode)
{
    ImageInfo *image = ImageCacheManager::GetInstance().LoadOneInMultiRes(playMode, PLAYER_IAMGES);
    playModelChangeButton_->SetSrc(image);
}

int16_t PlayersView::GetVolumeRank(void)
{
    return volumeSet;
}

void PlayersView::SetVolumeRank(int16_t value)
{
    volumeSet = value;
}

void PlayersView::SetVolumeAddOne(void)
{
    volumeSet++;
}

UICheckBox::UICheckBoxState PlayersView::GetPlayerButtonState(void)
{
    return playerOrPauseButton_->GetState();
}

void PlayersView::SetPlayerState(bool value)
{
    if (value) {
        SetPlayerButtonState(UICheckBox::UICheckBoxState::SELECTED);
        PlayersModel::GetInstance()->SetPlayButtonState(false);
    }
}

void PlayersView::SetPlayerButtonState(UICheckBox::UICheckBoxState state)
{
    if (playerOrPauseButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersView::SetPlayerButtonState playerButton nullptr");
        return;
    }
    return playerOrPauseButton_->SetState(state);
}

void PlayersView::SetPlaybackProgress(int8_t value)
{
    if (playbackProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersView::playbackProgress_ nullptr");
        return;
    }
    playbackProgress_->SetValue(value);
    playbackProgress_->Invalidate();
    return;
}

void PlayersView::SetPlayerCtrVolume(float leftVolume, float rightVolume)
{
    PlayersModel::GetInstance()->GetPlayerCtr()->SetVolume(leftVolume, rightVolume);
}

PlayersView* PlayersView::GetInstance(void)
{
    return g_pPlayersView;
}

bool PlayersView::InitView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView InitView");
    InitImage();
    InitButton();
    InitLabel();
    InitCircleProgress();
    InitSoundAdjust();
    InitOption();
    InitListView();
    Invalidate();
    return true;
}

void PlayersView::InitButton()
{
    if (nextButton_ == nullptr) {
        nextButton_ = new UIImageView();
    }
    if (preButton_ == nullptr) {
        preButton_ = new UIImageView();
    }
    if (playerOrPauseButton_ == nullptr) {
        playerOrPauseButton_ = new UICheckBox();
    }
    if (playModelChangeButton_ == nullptr) {
        playModelChangeButton_ = new UIImageView();
    }
    if (soundButton_ == nullptr) {
        soundButton_ = new UIImageView();
    }
    if (optionsButton_ == nullptr) {
        optionsButton_ = new UIImageView();
    }
    if (pullButton_ == nullptr) {
        pullButton_ = new UIImageView();
    }
    if ((nextButton_ == nullptr) || (preButton_ == nullptr) || (optionsButton_ == nullptr)
        || (soundButton_ == nullptr) || (playModelChangeButton_ == nullptr) || (pullButton_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UIButton new fail");
        return;
    }
    if ((playerOrPauseButton_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerOrPauseButton_ new fail");
        return;
    }

    nextButton_->SetPosition(NEXT_BUTTON_POSITION_X, BUTTON_POSITION_Y_ONE);
    nextButton_->SetViewId(NEXT_SONG_BUTTON);
    LOADIMG::LoadImageViewImg(nextButton_, PLAYER_IAMGES, PLAYER_NEXT_BUTTON);
    nextButton_->SetTouchable(true);
    nextButton_->SetOnClickListener(this);

    preButton_->SetPosition(PRE_BUTTON_POSITION_X, BUTTON_POSITION_Y_ONE);
    preButton_->SetViewId(PRE_SONG_BUTTON);
    LOADIMG::LoadImageViewImg(preButton_, PLAYER_IAMGES, PLAYER_PRE_BUTTOW);
    preButton_->SetTouchable(true);
    preButton_->SetOnClickListener(this);

    ImageInfo *image;
    ImageInfo *hlImage;
    playerOrPauseButton_->SetPosition(PLAY_OR_PAUSE_BUTTON_POSITION_X, PLAY_OR_PAUSE_BUTTON_POSITION_Y,
                                      PLAY_OR_PAUSE_BUTTON_SIZE, PLAY_OR_PAUSE_BUTTON_SIZE);
    playerOrPauseButton_->SetViewId(PLAY_OR_PAUSE_BUTTON);
    playerOrPauseButton_->SetOnClickListener(this);
    image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_PLAYER, PLAYER_IAMGES);
    hlImage = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_PLAYERHIGHLIGHT, PLAYER_IAMGES);
    playerOrPauseButton_->SetImages(hlImage, image);

    playModelChangeButton_->SetPosition(PLAY_MODE_BUTTON_POSITION_X, BUTTON_POSITION_Y_TWO);
    playModelChangeButton_->SetViewId(PLAYMODE_CHANGE_BUTTON);
    SetUpPlayerMode();
    playModelChangeButton_->SetTouchable(true);
    playModelChangeButton_->SetOnClickListener(this);

    soundButton_->SetPosition(SOUND_BUTTON_POSITION_X, BUTTON_POSITION_Y_TWO);
    soundButton_->SetViewId(VOICE_ADJUST_BUTTON);
    LOADIMG::LoadImageViewImg(soundButton_, PLAYER_IAMGES, PLAYER_SOUND_IMAGE);
    soundButton_->SetTouchable(true);
    soundButton_->SetOnClickListener(this);

    optionsButton_->SetPosition(OPTION_BUTTON_POSITION_X, BUTTON_POSITION_Y_TWO);
    optionsButton_->SetViewId(OPTIONS_BUTTON);
    LOADIMG::LoadImageViewImg(optionsButton_, PLAYER_IAMGES, PLAYER_OPTIONS_IMAGE);
    optionsButton_->SetTouchable(true);
    optionsButton_->SetOnClickListener(this);

    pullButton_->SetPosition(PULL_BUTTON_POSITION_X, PULL_BUTTON_POSITION_Y);
    pullButton_->SetViewId(PULL_BUTTON);
    pullButton_->SetSrc(PNG_PULLUP_BUTTON);
    pullButton_->SetTouchable(true);
    pullButton_->SetOnClickListener(this);

    Add(nextButton_);
    Add(preButton_);
    Add(optionsButton_);
    Add(soundButton_);
    Add(playModelChangeButton_);
    Add(playerOrPauseButton_);
    Add(pullButton_);
}

void PlayersView::InitLabel()
{
    musicNameLabel_ = new UILabel();
    if (musicNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "musicNameLabel_ new fail");
        return;
    }
    musicNameLabel_->SetPosition(MUSIC_NAME_LABEL_POSITION_X, MUSIC_NAME_LABEL_POSITION_Y,
                                 MUSIC_NAME_LABEL_WIDTH, MUSIC_NAME_LABEL_HEIGHT);
    musicNameLabel_->SetText("Music Name");
    musicNameLabel_->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    musicNameLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
                              UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    musicNameLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_FORTY_SIZE);
    Add(musicNameLabel_);

    musicAuthorLabel_ = new UILabel();
    if (musicAuthorLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "musicAuthorLabel_ new fail");
        return;
    }
    musicAuthorLabel_->SetPosition(MUSIC_AUTHOR_LABEL_POSITIO0N_X, MUSIC_AUTHOR_LABEL_POSITION_Y,
                                   MUSIC_AUTHOR_LABEL_WIDTH, MUSIC_AUTHOR_LABEL_HEIGHT);
    musicAuthorLabel_->SetText("Musical Authors");
    musicAuthorLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_THIRTY_SIZE); // 30：font data
    musicAuthorLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
                                UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    Add(musicAuthorLabel_);
}

void PlayersView::InitImage()
{
    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImg_ new fail");
        return;
    }
    bgImg_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(bgImg_, PLAYER_IAMGES, PLAYER_BACK_GROUND);
    Add(bgImg_);
}

void PlayersView::InitCircleProgress()
{
    playbackProgress_ = new UICircleProgress();
    if (playbackProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playbackProgress_ new fail");
        return;
    }
    playbackProgress_->SetPosition(140, 139, 172, 172); // 140:x 139:y 172:width 172:height
    playbackProgress_->SetCenterPosition(86, 86); // 86:x 86:y
    playbackProgress_->SetRange(PLAY_PROGRESS_MAX, PLAY_PROGRESS_MIN);
    playbackProgress_->SetRadius(86); // 86:radius
    playbackProgress_->SetStartAngle(0);
    playbackProgress_->SetEndAngle(360); // 360: end angle
    playbackProgress_->SetValue(0);
    playbackProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, Color::Gray().full);
    playbackProgress_->SetForegroundStyle(STYLE_LINE_COLOR, Color::Red().full);
    playbackProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, 3); // 3:width
    playbackProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, 3); // 3:width
    Add(playbackProgress_);
}

void PlayersView::InitSoundAdjust()
{
    volumeSettingView_ = new VolumeSettingView();
    if (volumeSettingView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "volumeSetting nullptr err");
        return;
    }
    if (!volumeSettingView_->SetUpVolume()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "volumeSetting SetUpVolume fail");
        return;
    }
    int32_t volume = AudioManager::GetInstance().GetVolume(AUDIO_STREAM_MUSIC);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "current volume = %d", volume);
    volumeSettingView_->GetProgress()->SetValue(volume);
    if (volume == 0) {
        ImageInfo *image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_MUTE_BUTTON, PLAYER_IAMGES);
        volumeSettingView_->GetDecreaseButton()->SetImageSrc(image, image);
    }
    volumeSliderEventListener_ = volumeSettingView_->GetVolumeEventListener();
    if (volumeSliderEventListener_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SoundButtonOnClick volumeSliderEventListener_ nullptr err");
        return;
    }
    volumeSettingView_->SetVisible(false);
    Add(volumeSettingView_);
}

void PlayersView::InitOption()
{
    playerOptionGroup_ = new PlayersOptionGroup(); // 打开BluetoothView对应的group
    if (playerOptionGroup_ == nullptr) {
        return;
    }
    playerOptionGroup_->SetVisible(false);
    Add(playerOptionGroup_);
}

void PlayersView::InitListView()
{
    // 遍历文件名到list链表
    PlayersListGroup::GetInstance()->SetUpListCase();
    PlayersModel::GetInstance()->PlayerInit();
    playersMusicListGroup = new PlayersMusicListGroup(); // 打开BluetoothView对应的group
    if (playersMusicListGroup == nullptr) {
        return;
    }
    playersMusicListGroup->SetVisible(false);
    Add(playersMusicListGroup);
}

void PlayersView::SetUpPlayerMode()
{
    if (PlayersModel::GetInstance()->GetPlayerModel() == LOOP) {
        LOADIMG::LoadImageViewImg(playModelChangeButton_, PLAYER_IAMGES, PLAYER_PLAYERSMODEL_LOOP);
    } else if (PlayersModel::GetInstance()->GetPlayerModel() == SEQUENCE) {
        LOADIMG::LoadImageViewImg(playModelChangeButton_, PLAYER_IAMGES, PLAYER_PLAYERSMODEL_SEQUENCE);
    } else if (PlayersModel::GetInstance()->GetPlayerModel() == REPEATS) {
        LOADIMG::LoadImageViewImg(playModelChangeButton_, PLAYER_IAMGES, PLAYER_PLAYERSMODEL_REPEATS);
    } else if (PlayersModel::GetInstance()->GetPlayerModel() == RANDOM) {
        LOADIMG::LoadImageViewImg(playModelChangeButton_, PLAYER_IAMGES, PLAYER_PLAYERSMODEL_RANDOM);
    }
}

void PlayersView::PlayerHeadsetReset()
{
    if (playerOrPauseButton_->GetState() == UICheckBox::UICheckBoxState::UNSELECTED) {
        SetPlayerButtonState(UICheckBox::UICheckBoxState::SELECTED);
    } else {
        SetPlayerButtonState(UICheckBox::UICheckBoxState::UNSELECTED);
    }
}

void PlayersView::PreLoad(void)
{
    if (!viewiInitStatus) {
        if (InitView()) {
            viewiInitStatus = true;
        }
    }
}

void PlayersView::UnLoad(void)
{
    // Players's usage scenes is complicated, so we do not release it's resources inner slice.
}

void PlayersView::OnInactive(void)
{
    volumeSettingView_->SetVisible(false);
    playersMusicListGroup->SetVisible(false);
    SetVisible(true);
}

bool PlayersView::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), NEXT_SONG_BUTTON) == 0) {
        playbackProgress_->SetValue(0); // 0:进度0%
        playbackProgress_->Invalidate();
        PlayersModel::GetInstance()->LoopingPlayout();
    } else if (strcmp(view.GetViewId(), PRE_SONG_BUTTON) == 0) {
        playbackProgress_->SetValue(0); // 0:进度0%
        playbackProgress_->Invalidate();
        PlayersModel::GetInstance()->PreviousPlayout();
    } else if (strcmp(view.GetViewId(), PLAY_OR_PAUSE_BUTTON) == 0) {
        PlayOrPauseButtonOnClick();
    } else if (strcmp(view.GetViewId(), PLAYMODE_CHANGE_BUTTON) == 0) {
        PlayersModel::GetInstance()->SetPlayerModelAddOne();
        if (PlayersModel::GetInstance()->GetPlayerModel() > RANDOM) {
            PlayersModel::GetInstance()->SetPlayerModel(LOOP);
        }
        SetUpPlayerMode();
    } else if (strcmp(view.GetViewId(), VOICE_ADJUST_BUTTON) == 0) {
        volumeSliderEventListener_->SetPlayerCoefficients(VOLUMEPLAYERS); // 进度条系数
        volumeSettingView_->SetVisible(true);
    } else if (strcmp(view.GetViewId(), OPTIONS_BUTTON) == 0) {
        playerOptionGroup_->SetVisible(true);
    } else if (strcmp(view.GetViewId(), PULL_BUTTON) == 0) {
        // 无对应的逻辑代码，预留接口
    }
    Invalidate();
    return true;
}

void PlayersView::PlayOrPauseButtonOnClick()
{
    uint8_t state = UICheckBox::UICheckBoxState::MAX_STATUS_NUM;
    int32_t playerState;
    int32_t connCtState = avrcp_ct_get_device_connect_state(SettingBluetoothModel::GetInstance()->GetLastConnedAddr());
    int32_t connTgState = avrcp_tg_get_device_connect_state(SettingBluetoothModel::GetInstance()->GetLastConnedAddr());

    state = playerOrPauseButton_->GetState();
    if (PlayersModel::GetInstance()->GetPlayerCtr() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersModel::GetInstance()->GetPlayerCtr() nullptr");
        return;
    }
    PlayersModel::GetInstance()->GetPlayerCtr()->GetPlayerState(playerState);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "buttonState = %d playerState = %d", state, playerState);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "connCtState = %d connTgState = %d", connCtState, connTgState);

    if (GetCurrCallNum() != 0 && GetScoConnectState() == HFP_SCO_STATE_CONNECTED) {
        if (state == UICheckBox::UICheckBoxState::SELECTED) {
            playerOrPauseButton_->SetState(UICheckBox::UICheckBoxState::UNSELECTED);
        } else {
            playerOrPauseButton_->SetState(UICheckBox::UICheckBoxState::SELECTED);
        }
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[PlayerButtonOnClick] calling phone");
        return;
    }
    if (state == UICheckBox::UICheckBoxState::SELECTED) {
        /* start play */
        PlayersModel::GetInstance()->ConnectBtAndLocalPlay(connCtState, connTgState, playerState);
    } else if (state == UICheckBox::UICheckBoxState::UNSELECTED) {
        PlayersModel::GetInstance()->ConnectBtAndLocalPause(connCtState, connTgState);
    }
}

bool PlayersView::OnDragEvent(const DragEvent& event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView OnDrage");
    int yMoveDistance = 300;
    if ((event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) &&
        (event.GetStartPoint().y > yMoveDistance)) {
            PlayersMusicListGroup::GetInstance()->RefreshMusicList();
            playersMusicListGroup->SetVisible(true);
            return true;
    }
    Invalidate();
    return false;
}

void PlayersView::Update(void)
{
    if (playbackProgress_ == nullptr) {
        return;
    }
    int32_t playerState = 0;
    PlayersModel::GetInstance()->GetPlayerCtr()->GetPlayerState(playerState);

    if (playerState == PLAYER_STARTED) {
        SetPlayerButtonState(UICheckBox::UICheckBoxState::SELECTED);
    } else {
        SetPlayerButtonState(UICheckBox::UICheckBoxState::UNSELECTED);
    }

    if (playerState == PLAYER_STARTED) {
        PlayersModel::GetInstance()->GetPlayerCtr()->GetCurrentTime(playerCurrentTime_);
        PlayersModel::GetInstance()->GetDuration(playerMusicDuration_);
        if (playerMusicDuration_ != 0) {
            playerProgressValue_ = (playerCurrentTime_ * PROGRESS_PERCENTAGE) / playerMusicDuration_;
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "playerProgressValue_ %lld", playerProgressValue_);
        } else {
            playerProgressValue_ = 100; // 100: max progress value
        }
        PlayersModel::GetInstance()->SetCircleProgress(playerProgressValue_);
        playbackProgress_->SetValue(playerProgressValue_);
        playbackProgress_->Invalidate();
    }
}

uint32_t PlayersView::GetPeriod(void)
{
    return PLAYER_PERIOD;
}

}
