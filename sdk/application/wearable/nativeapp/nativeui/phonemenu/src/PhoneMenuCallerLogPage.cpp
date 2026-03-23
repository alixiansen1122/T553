/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallerLogPage
 * Create: 2025-06-06
 */

#include <string.h>
#include "UiConfig.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "SlicePageFactory.h"
#include "NativeAbility.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallerLogItemView.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "phonemenu/PhoneMenuCallerLogPage.h"

namespace OHOS {
static constexpr uint16_t TITLI_LABEL_X = 97;
static constexpr uint16_t TITLI_LABEL_Y = 31;
static constexpr uint16_t TITLI_LABEL_W = 260;
static constexpr uint16_t TITLI_LABEL_H = 53;
static constexpr uint16_t TITLI_LABEL_FONT = 38;
static constexpr uint16_t NOCALL_LABEL_X = 156;
static constexpr uint16_t NOCALL_LABEL_Y = 202;
static constexpr uint16_t NOCALL_LABEL_W = 148;
static constexpr uint16_t NOCALL_LABEL_H = 50;
static constexpr uint16_t NOCALL_LABEL_FONT = 36;
static constexpr uint16_t CLEAR_LABEL_X = 156;
static constexpr uint16_t CLEAR_LABEL_W = 148;
static constexpr uint16_t CLEAR_LABEL_H = 50;
static constexpr uint16_t CLEAR_LABEL_FONT = 36;
static constexpr uint16_t LIST_X = 17;
static constexpr uint16_t LIST_Y = 110;
static constexpr uint16_t LIST_W = 418;
static constexpr uint16_t LIST_H = 344;
static constexpr uint16_t LIST_ITEM_W = 418;
static constexpr uint16_t LIST_ITEM_H = 143;
static constexpr uint16_t TOP_BLANK_SIZE = 31;
static constexpr uint16_t BOTTOM_BLANK_SIZE = 80;
const char *PHONE_MENU_CLEAR_BUTTON = "phoneMenuClearButton";

REGIST_SLICE_PAGE(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLER_LOG_PAGE, PhoneMenuCallerLogPage, false);

static PhoneMenuCallerLogPage *g_PhoneMenuCallerLogPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    PhoneMenuCallerLogItemView *item = new PhoneMenuCallerLogItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_PhoneMenuCallerLogPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    PhoneMenuCallerLogItemView *item = dynamic_cast<PhoneMenuCallerLogItemView *>(view);
    if (item == nullptr) {
        return;
    }
    CallLogInfo *itemInfo = static_cast<CallLogInfo *>(data);
    item->SetItemInfo(*itemInfo);
}

PhoneMenuCallerLogPage::PhoneMenuCallerLogPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage::PhoneMenuCallerLogPage");
    g_PhoneMenuCallerLogPageInstance = this;
}

PhoneMenuCallerLogPage::~PhoneMenuCallerLogPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (noCallLabel_ != nullptr) {
        delete noCallLabel_;
        noCallLabel_ = nullptr;
    }
    if (clearButton_ != nullptr) {
        delete clearButton_;
        clearButton_ = nullptr;
    }
    if (callerLogList_ != nullptr) {
        delete callerLogList_;
        callerLogList_ = nullptr;
    }
    if (info_ != nullptr) {
        delete[] info_;
        info_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage::~PhoneMenuCallerLogPage");
}

void PhoneMenuCallerLogPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(TITLI_LABEL_X, TITLI_LABEL_Y, TITLI_LABEL_W, TITLI_LABEL_H);
    titleLabel_->SetText("最近通话");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, TITLI_LABEL_FONT);
    group_->Add(titleLabel_);

    noCallLabel_ = new UILabel();
    if (noCallLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage noCallLabel_ new fail");
        return;
    }
    noCallLabel_->SetPosition(NOCALL_LABEL_X, NOCALL_LABEL_Y, NOCALL_LABEL_W, NOCALL_LABEL_H);
    noCallLabel_->SetText("暂无通话");
    noCallLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    noCallLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, NOCALL_LABEL_FONT);
    group_->Add(noCallLabel_);

    clearButton_ = new UILabelButton();
    if (clearButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage clearButton_ new fail");
        return;
    }
    clearButton_->SetText("全部清除");
    clearButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, CLEAR_LABEL_FONT);
    clearButton_->SetViewId(PHONE_MENU_CLEAR_BUTTON);
    clearButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    clearButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    clearButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    clearButton_->SetOnClickListener(this);
    group_->Add(clearButton_);

    callerLogList_ = new UISimpleList();
    if (callerLogList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage callerLogList_ new fail");
        return;
    }
    callerLogList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    callerLogList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    callerLogList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    callerLogList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    int num = 0;
    if (GetCallLog(&info_, &num) == -1) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage GetCallLog fail");
    }

    for (int i = num - 1; i >= 0; i--) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&info_[i]);
        callerLogList_->AddContent(content);
    }
    callerLogList_->RefreshList();
    group_->Add(callerLogList_);

    if (num == 0) {
        noCallLabel_->SetVisible(true);
        callerLogList_->SetVisible(false);
        clearButton_->SetVisible(false);
    } else {
        group_->SetYScrollBarVisible(true);
        group_->SetScrollBlankSize(TOP_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
        group_->SetScrollBlankSize(BOTTOM_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);
        noCallLabel_->SetVisible(false);
        callerLogList_->SetVisible(true);
        callerLogList_->Resize(LIST_W, LIST_ITEM_H * num);
        clearButton_->SetVisible(true);
        clearButton_->SetPosition(CLEAR_LABEL_X, LIST_Y + LIST_ITEM_H * num, CLEAR_LABEL_W, CLEAR_LABEL_H);
    }

    AddViewToPageContainer(group_);
}

bool PhoneMenuCallerLogPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool PhoneMenuCallerLogPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), PHONE_MENU_CLEAR_BUTTON) == 0) {
        if (ClearCallLogFile() == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage ClearCallLogFile success");
        }
        callerLogList_->ClearAll();
        callerLogList_->RefreshList();
        NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLER_LOG_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else {
        if (GetBtConnectStatus() != PROFILE_STATE_CONNECTED) {
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_DISCONNECT_PAGE,
                TransitionType::TRANSITION_INVALID, false);
            return true;
        }

        PhoneMenuCallerLogItemView *itemView = dynamic_cast<PhoneMenuCallerLogItemView *>(&view);
        if (itemView == nullptr) {
            return false;
        }
        unsigned char *phoneNumber = itemView->GetItemPhoneNumber();
        SetMicMute(false);
        PhoneMenuCallerLogModel::GetInstance()->SetMicMute(false);
        if (DialCall(phoneNumber, strlen((const char *)phoneNumber)) != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogPage DialCall fail");
            return false;
        }
        PhoneMenuCallerLogModel::GetInstance()->SaveCurPage();
    }
    return true;
}
}