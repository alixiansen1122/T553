/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuDisconnectPage
 * Create: 2025-07-12
 */
#include "SlicePageFactory.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuDisconnectPage.h"

namespace OHOS {
static constexpr uint16_t LABEL_FONT = 32;
static constexpr uint16_t LABEL_XPOS = 30;
static constexpr uint16_t LABEL_YPOS = 257;
static constexpr uint16_t LABEL_WIDTH = 396;
static constexpr uint16_t LABEL_HEIGHT = 50;
static constexpr uint16_t IMAGE_XPOS = 176;
static constexpr uint16_t IMAGE_YPOS = 115;

REGIST_SLICE_PAGE(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_DISCONNECT_PAGE, PhoneMenuDisconnectPage, false);

void PhoneMenuDisconnectPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDisconnectPage container_ new fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetOnDragListener(this);

    text_ = new UILabel();
    if (text_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDisconnectPage text_ new fail");
        return;
    }
    text_->SetPosition(LABEL_XPOS, LABEL_YPOS, LABEL_WIDTH, LABEL_HEIGHT);
    text_->SetText("设备未连接，请连接设备!");
    text_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    text_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    container_->Add(text_);

    image_ = new UIImageView();
    if (image_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDisconnectPage image_ new fail");
        return;
    }
    image_->SetPosition(IMAGE_XPOS, IMAGE_YPOS);
    LOADIMG::LoadImageViewImg(image_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_DISCONNECT);
    container_->Add(image_);

    AddViewToPageContainer(container_);
}

PhoneMenuDisconnectPage::PhoneMenuDisconnectPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuDisconnectPage::PhoneMenuDisconnectPage");
}

PhoneMenuDisconnectPage::~PhoneMenuDisconnectPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (text_ != nullptr) {
        delete text_;
        text_ = nullptr;
    }
    if (image_ != nullptr) {
        delete image_;
        image_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuDisconnectPage::~PhoneMenuDisconnectPage");
}

bool PhoneMenuDisconnectPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool PhoneMenuDisconnectPage::OnClick(UIView &view, const ClickEvent &event)
{
    return false;
}
}