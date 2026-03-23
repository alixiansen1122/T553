/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightRecoverPage
 * Create: 2025-04-24
 */

#include "SlicePageFactory.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "ui_resource_flashlight.h"
#include "flashlight/FlashLightView.h"
#include "flashlight/FlashLightModel.h"
#include "flashlight/FlashLightPresenter.h"
#include "flashlight/FlashLightRecoverPage.h"

namespace OHOS {
static constexpr uint16_t RECOVER_LABEL_POS_X = 15;
static constexpr uint16_t RECOVER_LABEL_POS_Y = 185;
static constexpr uint16_t RECOVER_LABEL_WIDTH = 424;
static constexpr uint16_t RECOVER_LABEL_HEIGHT = 50;
static constexpr uint16_t RECOVER_LABEL_FONT_SIZE = 36;
static constexpr uint16_t CANCLE_BUTTON_POS_X = 115;
static constexpr uint16_t CANCLE_BUTTON_POS_Y = 300;
static constexpr uint16_t CONFIRM_BUTTON_POS_X = 255;
static constexpr uint16_t CONFIRM_BUTTON_POS_Y = 300;
static constexpr uint16_t CANCLE_BUTTON_WIDTH = 85;
static constexpr uint16_t CANCLE_BUTTON_HEIGHT = 85;
static constexpr char *CANCLE_BUTTON_ID = "cancle";
static constexpr char *CONFIRM_BUTTON_ID = "confirm";

REGIST_SLICE_PAGE(VIEW_FLASHLIGHT, FlashLightAllView::FLASH_LIGHT_RECOVER_VIEW, FlashLightRecoverPage, false);
void FlashLightRecoverPage::OnStart(void *data)
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        container_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    }

    if (recoverLabel_ == nullptr) {
        recoverLabel_ = new UILabel();
    }
    recoverLabel_->SetPosition(RECOVER_LABEL_POS_X, RECOVER_LABEL_POS_Y, RECOVER_LABEL_WIDTH, RECOVER_LABEL_HEIGHT);
    recoverLabel_->SetText("是否恢复默认设置");
    recoverLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    recoverLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    recoverLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, RECOVER_LABEL_FONT_SIZE);
    // Add to rootView
    container_->Add(recoverLabel_);
    recoverLabel_->SetVisible(true);

    if (cancelButton_ == nullptr) {
        cancelButton_ = new UILabelButtonExt();
    }
    cancelButton_->SetPosition(CANCLE_BUTTON_POS_X, CANCLE_BUTTON_POS_Y, CANCLE_BUTTON_WIDTH, CANCLE_BUTTON_HEIGHT);
    cancelButton_->SetViewId(CANCLE_BUTTON_ID);
    LOADIMG::LoadBtnImage(cancelButton_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_RECOVERCANCLE,
                          IMAGE_FLASHLIGHT_RECOVERCANCLE);
    cancelButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    cancelButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    container_->Add(cancelButton_);
    cancelButton_->SetVisible(true);

    if (confirmButton_ == nullptr) {
        confirmButton_ = new UILabelButtonExt();
    }
    confirmButton_->SetPosition(CONFIRM_BUTTON_POS_X, CONFIRM_BUTTON_POS_Y, CANCLE_BUTTON_WIDTH, CANCLE_BUTTON_HEIGHT);
    confirmButton_->SetViewId(CONFIRM_BUTTON_ID);
    LOADIMG::LoadBtnImage(confirmButton_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_RECOVERCONFIRM,
                          IMAGE_FLASHLIGHT_RECOVERCONFIRM);
    confirmButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    container_->Add(confirmButton_);
    confirmButton_->SetVisible(true);

    // 设置界面组件回调函数
    container_->SetOnDragListener(this);
    cancelButton_->SetOnClickListener(this);
    confirmButton_->SetOnClickListener(this);
    AddViewToPageContainer(container_);
}

FlashLightRecoverPage::FlashLightRecoverPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightRecoverPage::FlashLightRecoverPage");
}

FlashLightRecoverPage::~FlashLightRecoverPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightRecoverPage::~FlashLightRecoverPage");
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (recoverLabel_ != nullptr) {
        delete recoverLabel_;
        recoverLabel_ = nullptr;
    }
    if (cancelButton_ != nullptr) {
        delete cancelButton_;
        cancelButton_ = nullptr;
    }
    if (confirmButton_ != nullptr) {
        delete confirmButton_;
        confirmButton_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
}

bool FlashLightRecoverPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_SET_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool FlashLightRecoverPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), CONFIRM_BUTTON_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_VIEW,
            TransitionType::TRANSITION_INVALID, false);
        FlashLightModel::GetInstance().SetDefault();
    } else if (strcmp(view.GetViewId(), CANCLE_BUTTON_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_SET_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}