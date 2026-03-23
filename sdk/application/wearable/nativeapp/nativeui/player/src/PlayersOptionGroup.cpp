/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersOptionGroup
 * Created: 2025-06
 */

#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_image.h"
#include "graphic_service.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuView.h"
#include "main/VolumeSettingView.h"
#include "settings/common/SettingCommon.h"
#include "player/PlayersPresenter.h"
#include "player/PlayersModel.h"
#include "bts_avrcp_controller.h"
#include "bts_avrcp_target.h"
#include "player/PlayersOptionGroup.h"

namespace OHOS {
static constexpr int16_t BUTTON_X = 320;
static constexpr int16_t BUTTON_X2 = 313;
static constexpr int16_t WATCH_BUTTON_Y = 104;
static constexpr int16_t PHONE_BUTTON_Y = 194;
static constexpr int16_t BLUETOOTH_BUTTON_Y = 278;
static constexpr int16_t BUTTON_SIZE = 55;
static constexpr int16_t LABEL_WIDTH = 155;
static constexpr int16_t LABEL_HEIGHT = 40;
static constexpr int16_t LABEL_X = 155;
static constexpr int16_t WATCH_LABEL_Y = 108;
static constexpr int16_t PHONE_LABEL_Y = 199;
static constexpr int16_t BLUETOOTH_LABEL_Y = 290;
static constexpr int16_t IMAGE_X = 50;
static constexpr int16_t WATCH_IMAGE_Y = 98;
static constexpr int16_t PHONE_IMAGE_Y = 187;
static constexpr int16_t BLUETOOTH_IMAGE_Y = 278;
static constexpr int16_t IMAGE_SIZE = 75;
static constexpr char* BLUETOOTH_BUTTON = "BLUETOOTH_BUTTON";
static PlayersOptionGroup* g_pPlayersOptionGroup = nullptr;

PlayersOptionGroup::PlayersOptionGroup()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersOptionGroup::PlayersOptionGroup");
    g_pPlayersOptionGroup = this;
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetDraggable(true);
    SetTouchable(true);
    SetOnClickListener(this);
    SetOnDragListener(this);
    InitImage();
    InitButton();
    InitLabel();
}

PlayersOptionGroup::~PlayersOptionGroup()
{
    RemoveAll();
    if (watchLabel_ != nullptr) {
        delete watchLabel_;
        watchLabel_ = nullptr;
    }
    if (phoneLabel_ != nullptr) {
        delete phoneLabel_;
        phoneLabel_ = nullptr;
    }
    if (bluetoothLabel_ != nullptr) {
        delete bluetoothLabel_;
        bluetoothLabel_ = nullptr;
    }
    if (watchImage_ != nullptr) {
        delete watchImage_;
        watchImage_ = nullptr;
    }
    if (phoneImage_ != nullptr) {
        delete phoneImage_;
        phoneImage_ = nullptr;
    }
    if (bluetoothImage_ != nullptr) {
        delete bluetoothImage_;
        bluetoothImage_ = nullptr;
    }
    if (watchButton_ != nullptr) {
        delete watchButton_;
        watchButton_ = nullptr;
    }
    if (phoneButton_ != nullptr) {
        delete phoneButton_;
        phoneButton_ = nullptr;
    }
    if (bluetoothButton_ != nullptr) {
        delete bluetoothButton_;
        bluetoothButton_ = nullptr;
    }
    if (deleteButton != nullptr) {
        delete deleteButton;
        deleteButton = nullptr;
    }
    g_pPlayersOptionGroup = nullptr;
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(PLAYER_IAMGES);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersOptionGroup::~PlayersOptionGroup");
}

void PlayersOptionGroup::InitButton()
{
    if (watchButton_ == nullptr) {
        watchButton_ = new UIRadioButton("MusicCategory");
        if (watchButton_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "watchButton UIRadio new fail");
            return;
        }
    }

    watchButton_->SetPosition(BUTTON_X, WATCH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE);
    watchButton_->SetOnClickListener(this);
    watchButton_->SetTouchable(false);

    if (phoneButton_ == nullptr) {
        phoneButton_ = new UIRadioButton("MusicCategory");
        if (phoneButton_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "phoneButton UIRadio new fail");
            return;
        }
    }
    phoneButton_->SetPosition(BUTTON_X, PHONE_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE);
    phoneButton_->SetOnClickListener(this);
    phoneButton_->SetTouchable(false);

    if (bluetoothButton_ == nullptr) {
        bluetoothButton_ = new UIButton();
        if (bluetoothButton_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "settingButton UIButton new fail");
            return;
        }
    }
    bluetoothButton_->SetPosition(BUTTON_X2, BLUETOOTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE);
    bluetoothButton_->SetOnClickListener(this);
    bluetoothButton_->SetImageSrc(PNG_SETTTING_BUTTON_IMAGE, PNG_SETTTING_BUTTON_IMAGE);
    bluetoothButton_->SetViewId(BLUETOOTH_BUTTON);
    Add(watchButton_);
    Add(phoneButton_);
    Add(bluetoothButton_);
}

void PlayersOptionGroup::InitLabel()
{
    if (watchLabel_ == nullptr) {
        watchLabel_ = new UILabel();
        if (watchLabel_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "watchLabel new fail");
            return ;
        }
    }

    watchLabel_->SetPosition(LABEL_X, WATCH_LABEL_Y, LABEL_WIDTH, LABEL_HEIGHT);
    watchLabel_->SetText("手表音乐");
    watchLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_THIRTY_SIZE);
    if (phoneLabel_ == nullptr) {
        phoneLabel_ = new UILabel();
        if (phoneLabel_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "phoneLabel new fail");
            return ;
        }
    }
    phoneLabel_->SetPosition(LABEL_X, PHONE_LABEL_Y, LABEL_WIDTH, LABEL_HEIGHT);
    phoneLabel_->SetText("手机音乐");
    phoneLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_THIRTY_SIZE);
    if (bluetoothLabel_ == nullptr) {
        bluetoothLabel_ = new UILabel();
        if (bluetoothLabel_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetoothLabel new fail");
            return ;
        }
    }
    bluetoothLabel_->SetPosition(LABEL_X, BLUETOOTH_LABEL_Y, LABEL_WIDTH, LABEL_HEIGHT);
    bluetoothLabel_->SetText("蓝牙音乐");
    bluetoothLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_THIRTY_SIZE);
    Add(watchLabel_);
    Add(phoneLabel_);
    Add(bluetoothLabel_);
}

void PlayersOptionGroup::InitImage()
{
    watchImage_ = new UIImageView();
    if (watchImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "watchImage new fail");
        return ;
    }
    watchImage_->SetPosition(IMAGE_X, WATCH_IMAGE_Y, IMAGE_SIZE, IMAGE_SIZE);
    watchImage_->SetSrc(PNG_WATCH_IMAGE);

    phoneImage_ = new UIImageView();
    if (phoneImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "phoneImage new fail");
        return ;
    }
    phoneImage_->SetPosition(IMAGE_X, PHONE_IMAGE_Y, IMAGE_SIZE, IMAGE_SIZE);
    phoneImage_->SetSrc(PNG_PHONE_IMAGE);

    bluetoothImage_ = new UIImageView();
    if (bluetoothImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetoothImage new fail");
        return ;
    }
    bluetoothImage_->SetPosition(IMAGE_X, BLUETOOTH_IMAGE_Y, IMAGE_SIZE, IMAGE_SIZE);
    bluetoothImage_->SetSrc(PNG_BLUETOOTH_IMAGE);
    Add(watchImage_);
    Add(phoneImage_);
    Add(bluetoothImage_);
}

bool PlayersOptionGroup::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), BLUETOOTH_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchSlice(VIEW_SETTING, SettingPageId::BLUETOOTH_PAGE);
    }
    return true;
}

bool PlayersOptionGroup::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
            SetVisible(false);
    }
    return true;
}
}