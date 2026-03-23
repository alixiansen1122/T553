/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightPlayPage
 * Create: 2025-04-24
 */

#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "flashlight/FlashLightView.h"
#include "flashlight/FlashLightPresenter.h"
#include "flashlight/FlashLightPlayPage.h"

namespace OHOS {
static constexpr uint32_t g_flashLightPageRes[] = {
    IMAGE_FLASHLIGHT_PLAY_BLACK_PAGE,      IMAGE_FLASHLIGHT_PLAY_WHITE_PAGE,     IMAGE_FLASHLIGHT_PLAY_RED_PAGE,
    IMAGE_FLASHLIGHT_PLAY_ORANGE_PAGE,     IMAGE_FLASHLIGHT_PLAY_YELLOW_PAGE,    IMAGE_FLASHLIGHT_PLAY_GREEN_PAGE,
    IMAGE_FLASHLIGHT_PLAY_LIGHT_BLUE_PAGE, IMAGE_FLASHLIGHT_PLAY_DARK_BLUE_PAGE, IMAGE_FLASHLIGHT_PLAY_PURPLE_PAGE,
    IMAGE_FLASHLIGHT_PLAY_MULTI_PAGE,
};
static constexpr uint32_t g_flashLightBlinkFreq[] = {500, 100, 0};
static constexpr char *PLAY_VIEW_ID = "flashLightPlayPage";

REGIST_SLICE_PAGE(VIEW_FLASHLIGHT, FlashLightAllView::FLASH_LIGHT_PLAY_VIEW, FlashLightPlayPage, false);
void FlashLightPlayPage::OnStart(void *data)
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        container_->SetViewId(PLAY_VIEW_ID);
    }
    uint16_t colorIndex = (uint16_t)FlashLightModel::GetInstance().GetViewColorId();
    uint16_t blinkIndex = (uint16_t)FlashLightModel::GetInstance().GetBlinkFrequency();
    if (playImageAnimal_ == nullptr) {
        playImageAnimal_ = new UIImageAnimatorView();
    }
    playImageInfo_[0].imageInfo =
        ImageCacheManager::GetInstance().LoadOneInMultiRes(g_flashLightPageRes[colorIndex], FLASHLIGHT_IMAGE);
    playImageInfo_[0].imageType = IMG_SRC_IMAGE_INFO;
    playImageInfo_[0].pos.x = 0;
    playImageInfo_[0].pos.y = 0;
    playImageInfo_[0].width = HORIZONTAL_RESOLUTION;
    playImageInfo_[0].height = VERTICAL_RESOLUTION;
    playImageInfo_[1].imageInfo =
        ImageCacheManager::GetInstance().LoadOneInMultiRes(g_flashLightPageRes[0], FLASHLIGHT_IMAGE);
    playImageInfo_[1].imageType = IMG_SRC_IMAGE_INFO;
    playImageInfo_[1].pos.x = 0;
    playImageInfo_[1].pos.y = 0;
    playImageInfo_[1].width = HORIZONTAL_RESOLUTION;
    playImageInfo_[1].height = VERTICAL_RESOLUTION;
    playImageAnimal_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    playImageAnimal_->SetImageAnimatorSrc(playImageInfo_, IMAGE_NUM, g_flashLightBlinkFreq[blinkIndex]);
    playImageAnimal_->SetRepeat(true);
    playImageAnimal_->SetAutoEnable(false);
    playImageAnimal_->SetResizeMode(UIImageView::FILL);
    container_->Add(playImageAnimal_);

    playImageAnimal_->Start();
    container_->SetOnDragListener(this);
    container_->SetOnClickListener(this);
    AddViewToPageContainer(container_);
}

FlashLightPlayPage::FlashLightPlayPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightPlayPage::FlashLightPlayPage");
}

FlashLightPlayPage::~FlashLightPlayPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (playImageAnimal_ != nullptr) {
        playImageAnimal_->Stop();
        delete playImageAnimal_;
        playImageAnimal_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightPlayPage::~FlashLightPlayPage");
}

bool FlashLightPlayPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool FlashLightPlayPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), PLAY_VIEW_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}