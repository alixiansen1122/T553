/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: VolumeSettingView view
 * Create: 2021-09-5
 */
#include "main/VolumeSettingView.h"
#include "player/PlayersView.h"
#include "common/image_cache_manager.h"
#include "UiConfig.h"
#include "ui_resource_image.h"

namespace OHOS {
static constexpr uint16_t BUTTON_X = 182;
static constexpr uint16_t BUTTON_Y = 0;
static constexpr uint16_t BUTTON_Y2 = 363;
static constexpr uint16_t BUTTON_WIDTH = 90;
static constexpr uint16_t BUTTON_HEIGHT = 90;
static constexpr uint16_t SLIDER_X = 207;
static constexpr uint16_t SLIDER_Y = 90;
static constexpr uint16_t SLIDER_WIDTH = 40;
static constexpr uint16_t SLIDER_HEIGHT = 274;

constexpr uint16_t VOLUME_OK = 0;
constexpr int16_t VOLUME_ERROR = (-1);

VolumeSettingView::VolumeSettingView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeSettingView::VolumeSettingView");
}

VolumeSettingView::~VolumeSettingView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeSettingView::~VolumeSettingView");
    RemoveAll();
    if (listener != nullptr) {
        delete listener;
        listener = nullptr;
    }
    delete increaseButton;
    increaseButton = nullptr;

    delete decreaseButton;
    decreaseButton = nullptr;

    delete progress;
    progress = nullptr;
}

bool VolumeSettingView::SetUpVolume(void)
{
    int ret;
    SetPosition(0, 0);
    SetWidth(HORIZONTAL_RESOLUTION);
    SetHeight(VERTICAL_RESOLUTION);

    if (progress == nullptr) {
        progress = new UISlider();
        if (progress == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "progress new fail");
            return false;
        }
    }
    progress->SetSliderColor(Color::Gray(), Color::Silver());
    progress->SetPosition(SLIDER_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    progress->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
    progress->SetViewId(VOLUME_PROGRESS);
    progress->SetRange(VOLUME_MAX, VOLUME_MIN);

    if (listener == nullptr) {
        listener = new VolumeSliderEventListener(this);
        if (listener == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "listener new fail");
            return false;
        }
    }
    progress->SetSliderEventListener(listener);
    progress->SetOnClickListener(listener);
    progress->SetOnDragListener(listener);
    progress->SetKnobStyle(StyleDefault::GetSliderKnobStyle());
    SetViewId(VOLUME_VIEW);
    SetOnDragListener(listener);
    SetDraggable(true);
    SetTouchable(true);
    Add(progress);
    ShowVolume();

    return true;
}

UISlider *VolumeSettingView::GetProgress(void)
{
    return progress;
}

UIButton *VolumeSettingView::GetIncreaseButton(void)
{
    return increaseButton;
}

UIButton *VolumeSettingView::GetDecreaseButton(void)
{
    return decreaseButton;
}

int VolumeSettingView::ShowVolume(void)
{
    ImageInfo *image;
    if (increaseButton == nullptr) {
        increaseButton = new UIButton();
        if (increaseButton == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "increaseButton new fail");
            return VOLUME_ERROR;
        }
    }
    increaseButton->SetPosition(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    increaseButton->SetViewId(INCREASE_BUTTON);
    image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_INCREASE_BUTTON, PLAYER_IAMGES);
    increaseButton->SetImageSrc(image, image);
    increaseButton->SetOnClickListener(listener);
    Add(increaseButton);

    if (decreaseButton == nullptr) {
        decreaseButton = new UIButton();
        if (decreaseButton == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "decreaseButton new fail");
            return VOLUME_ERROR;
        }
    }
    decreaseButton->SetPosition(BUTTON_X, BUTTON_Y2, BUTTON_WIDTH, BUTTON_HEIGHT);
    decreaseButton->SetViewId(DECREASE_BUTTON);
    image = ImageCacheManager::GetInstance().LoadOneInMultiRes(PLAYER_DECREASE_BUTTON, PLAYER_IAMGES);
    decreaseButton->SetImageSrc(image, image);
    decreaseButton->SetOnClickListener(listener);
    Add(decreaseButton);

    return VOLUME_OK;
}

VolumeSliderEventListener *VolumeSettingView::GetVolumeEventListener(void)
{
    return listener;
}

}
