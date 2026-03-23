/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuMainPage
 * Create: 2025-07-12
 */
#include "SlicePageFactory.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuMainPage.h"

namespace OHOS {
static constexpr uint16_t LABEL_POS_X = 97;
static constexpr uint16_t LABEL_POS_Y = 41;
static constexpr uint16_t LABEL_WIDTH = 260;
static constexpr uint16_t LABEL_HEIGHT = 53;
static constexpr uint16_t LABEL_FONT_SIZE = 40;
static constexpr uint16_t LIST_POS_X = 14;
static constexpr uint16_t LIST_POS_Y = 116;
static constexpr uint16_t LIST_WIDTH = 426;
static constexpr uint16_t LIST_HEIGHT = 330;
static constexpr uint16_t LIST_ITEM_W = 426;
static constexpr uint16_t LIST_ITEM_H = 110;
static constexpr uint16_t CONTAINER_TOP_BLANK_SIZE = 40;
static constexpr uint16_t CONTAINER_BOTTOM_BLANK_SIZE = 100;

REGIST_SLICE_PAGE(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_MAIN_PAGE, PhoneMenuMainPage, true);

static PhoneMenuMainPage *g_PhoneMenuMainPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    PhoneMenuMainItemView *item = new PhoneMenuMainItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_PhoneMenuMainPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    PhoneMenuMainItemView *item = dynamic_cast<PhoneMenuMainItemView *>(view);
    if (item == nullptr) {
        return;
    }
    PhoneMenuItem *itemInfo = static_cast<PhoneMenuItem *>(data);
    item->SetItemInfo(*itemInfo);
}

void PhoneMenuMainPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuMainPage container_ new fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetOnDragListener(this);
    container_->SetScrollBlankSize(CONTAINER_TOP_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
    container_->SetScrollBlankSize(CONTAINER_BOTTOM_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuMainPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABEL_POS_X, LABEL_POS_Y, LABEL_WIDTH, LABEL_HEIGHT);
    titleLabel_->SetText("电话");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
    container_->Add(titleLabel_);

    phoneMenuList_ = new UISimpleList();
    if (phoneMenuList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuMainPage phoneMenuList_ new fail");
        return;
    }
    phoneMenuList_->SetPosition(LIST_POS_X, LIST_POS_Y, LIST_WIDTH, LIST_HEIGHT);
    phoneMenuList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    phoneMenuList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    phoneMenuList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    for (uint8_t i = 0; i < PHOEN_MENU_ITEM_NUM; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&phoneMenu_[i]);
        phoneMenuList_->AddContent(content);
    }
    phoneMenuList_->RefreshList();
    container_->Add(phoneMenuList_);

    AddViewToPageContainer(container_);
}

PhoneMenuMainPage::PhoneMenuMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuMainPage::PhoneMenuMainPage");
    g_PhoneMenuMainPageInstance = this;
}

PhoneMenuMainPage::~PhoneMenuMainPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (phoneMenuList_ != nullptr) {
        delete phoneMenuList_;
        phoneMenuList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuMainPage::~PhoneMenuMainPage");
}

bool PhoneMenuMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool PhoneMenuMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), PHONE_MENU_ITEM_CONTACTS_BOOKS) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CONTACTS_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), PHONE_MENU_ITEM_RECENT_CALL) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLER_LOG_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), PHONE_MENU_ITEM_DIAL) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_DIAL_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}