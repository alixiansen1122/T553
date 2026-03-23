/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: VolumeSettingView view
 * Create: 2021-09-5
 */
#include "main/VolumeSettingView.h"
#include "main/VolumeSettingPresenter.h"
#include "player/PlayersModel.h"
#include "common/image_cache_manager.h"
#include "UiConfig.h"
#include "ui_resource_image.h"
#include "drag_event.h"
#include "phonemenu/PhoneMenuView.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "bts_avrcp_controller.h"
#include "bts_avrcp_target.h"
#include "notification_manager.h"
#include "settings/model/SettingBluetoothHeadsetEvent.h"
#include "bts_avrcp_controller.h"

namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif

constexpr int16_t SOUND_CHANGE = 10;

VolumeSliderEventListener::VolumeSliderEventListener(VolumeSettingView *volumeSetting) : volumeSettingView(volumeSetting)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeSliderEventListener new");
}

VolumeSliderEventListener::~VolumeSliderEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeSliderEventListener::~VolumeSliderEventListener");
}

bool VolumeSliderEventListener::OnClick(UIView& view, const ClickEvent& event)
{
    UNUSED(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnClick OnClick");
    if (strcmp(view.GetViewId(), INCREASE_BUTTON) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GetIncreaseButton OnClick");
        ChangeVolume(VOLUMEUP);
    } else if (strcmp(view.GetViewId(), DECREASE_BUTTON) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GetDecreaseButton OnClick");
        ChangeVolume(VOLUMEDOWN);
    } else if (strcmp(view.GetViewId(), VOLUME_PROGRESS) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GetDragButton OnClick");
        ChangeVolume(VOLUMEDRAG);
    }
    return true;
}

void VolumeSliderEventListener::ChangeVolume(VolumeChangeStatus status)
{
    ImageInfo *image;
    UISlider *progress = volumeSettingView->GetProgress();
    if (progress == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OnClick:progress new fail");
        return;
    }
    UIButton *decrease = volumeSettingView->GetDecreaseButton();
    if (decrease == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OnClick:decrease new fail");
        return;
    }

    preSound = volumeSettingView->sounding;
    if (status == VOLUMEUP) {
        volumeSettingView->sounding = progress->GetValue() + SOUND_CHANGE;
        if (volumeSettingView->sounding > VOLUME_MAX) {
            volumeSettingView->sounding = VOLUME_MAX;
        }
        image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_DECREASE_BUTTON, PLAYER_IAMGES);
        decrease->SetImageSrc(image, image);
        // 刷新UI
        decrease->Invalidate();
        progress->SetValue(volumeSettingView->sounding);
    } else if (status == VOLUMEDOWN) {
        volumeSettingView->sounding = progress->GetValue() - SOUND_CHANGE;
        if (volumeSettingView->sounding < VOLUME_MIN) {
            volumeSettingView->sounding = VOLUME_MIN;
        }
        if (volumeSettingView->sounding == VOLUME_MIN &&
            (playerCoefficients == VOLUMEPHONE || playerCoefficients == VOLUMEPHONENOTIFY)) {
            // 通话场景音量不能设置为0
            volumeSettingView->sounding = SOUND_CHANGE;
        }
        if ((volumeSettingView->sounding == 0) && (preSound != 0)) {
            image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_MUTE_BUTTON, PLAYER_IAMGES);
            decrease->SetImageSrc(image, image);
            // 刷新UI
            decrease->Invalidate();
        }
        progress->SetValue(volumeSettingView->sounding);
    } else if (status == VOLUMEDRAG) {
        volumeSettingView->sounding = progress->GetValue();
        if (volumeSettingView->sounding == VOLUME_MIN &&
            (playerCoefficients == VOLUMEPHONE || playerCoefficients == VOLUMEPHONENOTIFY)) {
            // 通话场景音量不能设置为0
            volumeSettingView->sounding = SOUND_CHANGE;
            progress->SetValue(volumeSettingView->sounding);
        }
        if (volumeSettingView->sounding == 0) {
            image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_MUTE_BUTTON, PLAYER_IAMGES);
            decrease->SetImageSrc(image, image);
            // 刷新UI
            decrease->Invalidate();
        } else {
            image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_DECREASE_BUTTON, PLAYER_IAMGES);
            decrease->SetImageSrc(image, image);
            // 刷新UI
            decrease->Invalidate();
        }
    }

    if (playerCoefficients == VOLUMEPLAYERS) {
        BtAndLocalVolumeSet(volumeSettingView->sounding);
    }
    if (playerCoefficients == VOLUMEPHONE || playerCoefficients == VOLUMEPHONENOTIFY) {
        unsigned char volumeValue = (unsigned char) ((double) (volumeSettingView->sounding * CALL_VOLUME_MAX) / (VOLUME_MAX - VOLUME_MIN)); // 音量转换
        SetSpeakerVolume(volumeValue);
    }
}

extern SettingBtAvrcpTgInf g_avrcp_srv_inf;
void VolumeSliderEventListener::BtAndLocalVolumeSet(int16_t sounding)
{
    if (g_avrcp_srv_inf.avrcp_srv_conn_stat == PROFILE_STATE_CONNECTED) {
        uint8_t volume = sounding * AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 / 100; // 100：本地音量最大值
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "avrcp volume = %d", volume);
        (void)avrcp_ct_set_absolute_volume(&(g_avrcp_srv_inf.bd_addr), volume); // 手表音量设置同步远端BT音量
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audio volume = %d", sounding);
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_MUSIC, sounding); // 本地音量设置
}

void VolumeSliderEventListener::OnRelease(int32_t value)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnRelease %d", value);
    ChangeVolume(VOLUMEDRAG);
}

void VolumeSliderEventListener::SetPlayerCoefficients(int16_t value)
{
    playerCoefficients = value;
}

void VolumeSliderEventListener::OnChange(int32_t value)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnChange %d", value);
    ChangeVolume(VOLUMEDRAG);
}

bool VolumeSliderEventListener::OnDrag(UIView& view, const DragEvent& event)
{
    if (strcmp(view.GetViewId(), VOLUME_VIEW) == 0) {
        if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeSliderEventListener::OnDrag");
            if (playerCoefficients == VOLUMEPLAYERS) {
                PlayersView *playersView = PlayersView::GetInstance();
                if (playersView == nullptr) {
                    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OnDrag:playersView new fail");
                    return false;
                }
                volumeSettingView->SetVisible(false);
                playersView->SetVisible(true);
            }
            if (playerCoefficients == VOLUMEPHONE) {
                volumeSettingView->SetVisible(false);
                PhoneMenuCallerLogModel::GetInstance()->SetVolumeScreenStatus(false);
            }
            if (playerCoefficients == VOLUMEPHONENOTIFY) {
                volumeSettingView->SetVisible(false);
                NotificationManager::GetInstance()->SetVolumeScreenStatus(false);
            }
            RootView::GetInstance()->Invalidate();
        }
    }
    return true;
}
#ifdef __cplusplus
}
#endif
}