/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightBlinkPage
 * Create: 2025-04-24
 */

#include "SlicePageFactory.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "ui_resource_flashlight.h"
#include "flashlight/FlashLightView.h"
#include "flashlight/FlashLightPresenter.h"
#include "flashlight/FlashLightModel.h"
#include "flashlight/FlashLightBlinkPage.h"

namespace OHOS {
static constexpr uint16_t BLINK_PICKER_POS_X = 106;
static constexpr uint16_t BLINK_PICKER_POS_Y = 113;
static constexpr uint16_t BLINK_PICKER_WIDTH = 254;
static constexpr uint16_t BLINK_PICKER_HEIGHT = 218;
static constexpr uint16_t BLINK_PICKER_BACKGROUND_FONT_SIZE = 38;
static constexpr uint16_t BLINK_PICKER_HIGHLIGHT_FONT_SIZE = 44;
static constexpr uint16_t BLINK_PICKER_ITEM_HEIGHT = 73;
static constexpr uint16_t BLINK_PICKER_NUM = 3;
static constexpr uint16_t BLINK_BUTTON_POS_X = 59;
static constexpr uint16_t BLINK_BUTTON_POS_Y = 358;
static constexpr uint16_t BLINK_BUTTON_WIDTH = 337;
static constexpr uint16_t BLINK_BUTTON_HEIGHT = 96;
static constexpr uint16_t BLINK_TEXT_POS_X = 100;
static constexpr uint16_t BLINK_TEXT_POS_Y = 38;
static constexpr uint16_t BLINK_TEXT_WIDTH = 253;
static constexpr uint16_t BLINK_TEXT_HEIGHT = 53;
static constexpr uint16_t BLINK_TEXT_FONT_SIZE = 38;
static constexpr char *START_BLINK_BUTTON_ID = "startBlink";

REGIST_SLICE_PAGE(VIEW_FLASHLIGHT, FlashLightAllView::FLASH_LIGHT_BLINK_VIEW, FlashLightBlinkPage, false);
void FlashLightBlinkPage::OnStart(void *data)
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        container_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    }

    if (blinkLabel_ == nullptr) {
        blinkLabel_ = new UILabel();
    }
    blinkLabel_->SetPosition(BLINK_TEXT_POS_X, BLINK_TEXT_POS_Y, BLINK_TEXT_WIDTH, BLINK_TEXT_HEIGHT);
    blinkLabel_->SetText("闪烁频率");
    blinkLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    blinkLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    blinkLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, BLINK_TEXT_FONT_SIZE);
    container_->Add(blinkLabel_);
    blinkLabel_->SetVisible(true);

    if (blinkPicker_ == nullptr) {
        blinkPicker_ = new UIPicker();
    }
    blinkPicker_->SetPosition(BLINK_PICKER_POS_X, BLINK_PICKER_POS_Y, BLINK_PICKER_WIDTH, BLINK_PICKER_HEIGHT);
    blinkPicker_->SetViewId("uiPicker1");
    blinkPicker_->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BLINK_PICKER_BACKGROUND_FONT_SIZE);
    blinkPicker_->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, BLINK_PICKER_HIGHLIGHT_FONT_SIZE);
    const char *uiPicker1Data[] = {"慢", "快", "不闪烁"};
    blinkPicker_->SetValues(uiPicker1Data, BLINK_PICKER_NUM);
    uint16_t selectBlink = FlashLightModel::GetInstance().GetBlinkFrequency();
    blinkPicker_->SetSelected(selectBlink);
    blinkPicker_->SetTextColor(Color::Gray(), Color::White());
    blinkPicker_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    blinkPicker_->SetItemHeight(BLINK_PICKER_ITEM_HEIGHT);
    container_->Add(blinkPicker_);
    blinkPicker_->SetVisible(true);

    if (blinkButton_ == nullptr) {
        blinkButton_ = new UILabelButtonExt();
    }
    blinkButton_->SetPosition(BLINK_BUTTON_POS_X, BLINK_BUTTON_POS_Y, BLINK_BUTTON_WIDTH, BLINK_BUTTON_HEIGHT);
    blinkButton_->SetViewId(START_BLINK_BUTTON_ID);
    LOADIMG::LoadBtnImage(blinkButton_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_BLINKBUTTON, IMAGE_FLASHLIGHT_BLINKBUTTON);
    blinkButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    blinkButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    container_->Add(blinkButton_);
    blinkButton_->SetVisible(true);

    blinkButton_->SetOnClickListener(this);
    container_->SetOnDragListener(this);
    AddViewToPageContainer(container_);
}

FlashLightBlinkPage::FlashLightBlinkPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightBlinkPage::FlashLightBlinkPage");
}

FlashLightBlinkPage::~FlashLightBlinkPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (blinkLabel_ != nullptr) {
        delete blinkLabel_;
        blinkLabel_ = nullptr;
    }
    if (blinkPicker_ != nullptr) {
        delete blinkPicker_;
        blinkPicker_ = nullptr;
    }
    if (blinkButton_ != nullptr) {
        delete blinkButton_;
        blinkButton_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightBlinkPage::~FlashLightBlinkPage");
}

bool FlashLightBlinkPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_SET_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool FlashLightBlinkPage::OnClick(UIView &view, const ClickEvent &event)
{
    uint16_t itemIndex = blinkPicker_->GetSelected();
    if (strcmp(view.GetViewId(), START_BLINK_BUTTON_ID) == 0) {
        FlashLightModel::GetInstance().SetBlinkFrequency((FlashLightBlinkFrequency)itemIndex);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}