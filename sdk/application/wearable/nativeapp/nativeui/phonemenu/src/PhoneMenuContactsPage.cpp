/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuContactsPage
 * Create: 2025-07-12
 */

#include "SlicePageFactory.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "phonemenu/PhoneMenuContactsItemView.h"
#include "phonemenu/PhoneMenuContactsPage.h"

namespace OHOS {
static constexpr uint16_t LABEL_POS_X = 97;
static constexpr uint16_t LABEL_POS_Y = 31;
static constexpr uint16_t LABEL_WIDTH = 260;
static constexpr uint16_t LABEL_HEIGHT = 53;
static constexpr uint16_t LABEL_FONT_SIZE = 38;
static constexpr uint16_t IMAGE_POS_X = 174;
static constexpr uint16_t IMAGE_POS_Y = 111;
static constexpr uint16_t LABEL_POS_X_NO_CONTACTS = 104;
static constexpr uint16_t LABEL_POS_Y_NO_CONTACTS = 257;
static constexpr uint16_t LABEL_WIDTH_NO_CONTACTS = 246;
static constexpr uint16_t LABEL_HEIGHT_NO_CONTACTS = 100;
static constexpr uint16_t LABEL_FONT_NO_CONTACTS = 34;
static constexpr uint16_t LIST_POS_X = 35;
static constexpr uint16_t LIST_POS_Y = 104;
static constexpr uint16_t LIST_WIDTH = 383;
static constexpr uint16_t LIST_HEIGHT = 350;
static constexpr uint16_t LIST_ITEM_W = 383;
static constexpr uint16_t LIST_ITEM_H = 102;
static constexpr uint16_t CONTAINER_TOP_BLANK_SIZE = 31;
static constexpr uint16_t CONTAINER_BOTTOM_BLANK_SIZE = 100;

REGIST_SLICE_PAGE(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CONTACTS_PAGE, PhoneMenuContactsPage, false);

static PhoneMenuContactsPage *g_PhoneMenuContactsPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    PhoneMenuContactsItemView *item = new PhoneMenuContactsItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_PhoneMenuContactsPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    PhoneMenuContactsItemView *item = dynamic_cast<PhoneMenuContactsItemView *>(view);
    if (item == nullptr) {
        return;
    }
    PhoneMenuContactsItem *itemInfo = static_cast<PhoneMenuContactsItem *>(data);
    item->SetItemInfo(*itemInfo);
}

void PhoneMenuContactsPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage container_ new fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetOnDragListener(this);
    container_->SetScrollBlankSize(CONTAINER_TOP_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
    container_->SetScrollBlankSize(CONTAINER_BOTTOM_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    contactsLabel_ = new UILabel();
    if (contactsLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage contactsLabel_ new fail");
        return;
    }
    contactsLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    container_->Add(contactsLabel_);

    auto list = PhoneMenuCallerLogModel::GetInstance()->GetContacts();
    if (list.empty()) {
        image_ = new UIImageView();
        if (image_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage image_ new fail");
            return;
        }
        image_->SetPosition(IMAGE_POS_X, IMAGE_POS_Y);
        LOADIMG::LoadImageViewImg(image_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_PHONE);
        container_->Add(image_);

        contactsLabel_->SetPosition(LABEL_POS_X_NO_CONTACTS, LABEL_POS_Y_NO_CONTACTS,
                                    LABEL_WIDTH_NO_CONTACTS, LABEL_HEIGHT_NO_CONTACTS);
        contactsLabel_->SetText("请前往手机APP\n同步常用联系人");
        contactsLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_NO_CONTACTS);
    } else {
        contactsLabel_->SetPosition(LABEL_POS_X, LABEL_POS_Y, LABEL_WIDTH, LABEL_HEIGHT);
        contactsLabel_->SetText("通讯录");
        contactsLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);

        contactsList_ = new UISimpleList();
        if (contactsList_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage contactsList_ new fail");
            return;
        }
        contactsList_->SetPosition(LIST_POS_X, LIST_POS_Y, LIST_WIDTH, LIST_HEIGHT);
        contactsList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
        contactsList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);
        contactsList_->Resize(LIST_WIDTH, LIST_ITEM_H * list.size());
        for (auto &contacts : list) {
            Contents content;
            content.createFunc = CreateViewCb;
            content.updateFunc = UpdateViewCb;
            content.type = 0;
            content.data = reinterpret_cast<void *>(&contacts);
            contactsList_->AddContent(content);
        }
        contactsList_->RefreshList();
        container_->Add(contactsList_);
    }
    AddViewToPageContainer(container_);
}

PhoneMenuContactsPage::PhoneMenuContactsPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage::PhoneMenuContactsPage");
    g_PhoneMenuContactsPageInstance = this;
}

PhoneMenuContactsPage::~PhoneMenuContactsPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (image_ != nullptr) {
        delete image_;
        image_ = nullptr;
    }
    if (contactsLabel_ != nullptr) {
        delete contactsLabel_;
        contactsLabel_ = nullptr;
    }
    if (contactsList_ != nullptr) {
        delete contactsList_;
        contactsList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage::~PhoneMenuContactsPage");
}

bool PhoneMenuContactsPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool PhoneMenuContactsPage::OnClick(UIView &view, const ClickEvent &event)
{
    PhoneMenuContactsItemView *itemView = dynamic_cast<PhoneMenuContactsItemView *>(&view);
    if (itemView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage::OnClick dynamic_cast view fail");
        return false;
    }
    std::string phoneNumber = itemView->GetContactsPhoneNumber();
    if (GetBtConnectStatus() != PROFILE_STATE_CONNECTED) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_DISCONNECT_PAGE,
            TransitionType::TRANSITION_INVALID, false);
        return true;
    }
    SetMicMute(false);
    PhoneMenuCallerLogModel::GetInstance()->SetMicMute(false);
    if (DialCall((unsigned char *)phoneNumber.c_str(), phoneNumber.size()) != OHOS_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuContactsPage::OnClick DialCall[fail]");
    }
    PhoneMenuCallerLogModel::GetInstance()->SaveCurPage();
    return true;
}
}