/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageMainPage
 * Create: 2025-06-29
 */

#include "wearable_log.h"
#include "common/screen.h"
#include "ui_resource_image.h"
#include "ui_resource_message.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"
#include "UiConfig.h"
#include "main/MainPresenterSample.h"
#include "main/MessageMainPage.h"

namespace OHOS {

static constexpr int16_t MESSAGE_TITLE_X = 97;
static constexpr int16_t MESSAGE_TITLE_Y = 31;
static constexpr int16_t MESSAGE_TITLE_WIDTH = 260;
static constexpr int16_t MESSAGE_TITLE_HEIGHT = 53;
static constexpr int16_t MESSAGE_TITLE_FONT_SIZE = 38;
static constexpr int16_t NOTICE_IMAGE_X = 191;
static constexpr int16_t NOTICE_IMAGE_Y = 361;
static constexpr int16_t NOTICE_IMAGE_SIZE = 85;
static constexpr int16_t MESAGE_LIST_X = 5;
static constexpr int16_t MESAGE_LIST_Y = 100;
static constexpr int16_t MESSAGE_LIST_WIDTH = 454;
static constexpr int16_t MESSAGE_LIST_HEIGHT = 354;
static constexpr int16_t MESSAGE_SCORLL_BAR_X = 222;
static constexpr int16_t MESSAGE_SCORLL_BAR_Y = 129;
static constexpr int16_t MESSAGE_SCORLL_BLANK_SIZE = 70;
static constexpr int16_t MESSAGE_DELETE_REMINDER_X = 21;
static constexpr int16_t MESSAGE_DELETE_REMINDER_Y = 158;
static constexpr int16_t MESSAGE_DELETE_REMINDER_WIDTH = 424;
static constexpr int16_t MESSAGE_DELETE_REMINDER_HEIGHT = 50;
static constexpr int16_t DELETE_REMINDER_FONT_SIZE = 36;
static constexpr int16_t MESSAGE_DELETE_CONFIRM_X = 260;
static constexpr int16_t MESSAGE_DELETE_CONFIRM_Y = 300;
static constexpr int16_t MESSAGE_DELETE_CONFIRM_SIZE = 85;
static constexpr int16_t MESSAGE_DELETE_CANCEL_X = 110;
static constexpr int16_t MESSAGE_DELETE_CANCEL_Y = 300;
static constexpr int16_t MESSAGE_DELETE_CANCEL_SIZE = 85;
static constexpr int16_t NOMESSAGE_TITLE_X = 45;
static constexpr int16_t NOMESSAGE_TITLE_Y = 341;
static constexpr int16_t NOMESSAGE_TITLE_WIDTH = 364;
static constexpr int16_t NOMESSAGE_TITLE_HEIGHT = 50;
static constexpr int16_t NOMESSAGE_TITLE_FONT_SIZE = 36;
static constexpr int16_t NOMESSAGE_ICON_X = 154;
static constexpr int16_t NOMESSAGE_ICON_Y = 154;
static constexpr int16_t NOMESSAGE_ICON_SIZE = 146;
static constexpr int16_t MOVE_DISTANCE = 30;
static constexpr int16_t MESSAGE_ITEM_HEIGHT = 130;
static constexpr char *DELETE_BUTTON = "deleteButton";
static constexpr char *CONFIRM_BUTTON = "confirmButton";
static constexpr char *CANCEL_BUTTON = "cancelButton";

static MessageMainPage *g_pMessageMainView = nullptr;

MessageMainPage::MessageMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MessageMainPage::MessageMainPage");
    g_pMessageMainView = this;
}

MessageMainPage::~MessageMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MessageMainPage::~MessageMainPage");
    RemoveAll();
    if (detailsGroup_ != nullptr) {
        delete detailsGroup_;
        detailsGroup_ = nullptr;
    }
    if (deleteGroup_ != nullptr) {
        deleteGroup_->RemoveAll();
        delete deleteGroup_;
        deleteGroup_ = nullptr;
    }
    if (noMsgLable_ != nullptr) {
        delete noMsgLable_;
        noMsgLable_ = nullptr;
    }
    if (msgLable_ != nullptr) {
        delete msgLable_;
        msgLable_ = nullptr;
    }
    if (noMsgImage_ != nullptr) {
        delete noMsgImage_;
        noMsgImage_ = nullptr;
    }
    if (deleteTips_ != nullptr) {
        delete deleteTips_;
        deleteTips_ = nullptr;
    }
    if (confirm_ != nullptr) {
        delete confirm_;
        confirm_ = nullptr;
    }
    if (cancel_ != nullptr) {
        delete cancel_;
        cancel_ = nullptr;
    }
    if (deleteImage_ != nullptr) {
        delete deleteImage_;
        deleteImage_ = nullptr;
    }
    if (msgList_ != nullptr) {
        delete msgList_;
        msgList_ = nullptr;
    }

    g_pMessageMainView = nullptr;
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(MESSAGE_IMAGE);
    MessageModel::GetInstance().SetViewStatus(false);
}

MessageMainPage *MessageMainPage::GetInstance(void)
{
    return g_pMessageMainView;
}

bool MessageMainPage::InitView()
{
    msgLable_ = new UILabel();
    if (msgLable_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "msgLable_ new fail");
        return false;
    }
    msgLable_->SetPosition(MESSAGE_TITLE_X, MESSAGE_TITLE_Y, MESSAGE_TITLE_WIDTH, MESSAGE_TITLE_HEIGHT);
    msgLable_->SetText("通知");
    msgLable_->SetAlign(TEXT_ALIGNMENT_CENTER);
    msgLable_->SetFont(BOLD_VECTOR_FONT_FILENAME, MESSAGE_TITLE_FONT_SIZE);
    msgLable_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    msgLable_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    // 删除图标
    deleteImage_ = new UIImageView();
    if (deleteImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new deleteImage_ fail");
        return false;
    }
    deleteImage_->SetPosition(NOTICE_IMAGE_X, NOTICE_IMAGE_Y, NOTICE_IMAGE_SIZE, NOTICE_IMAGE_SIZE);
    LOADIMG::LoadImageViewImg(deleteImage_, MESSAGE_IMAGE, IMAGE_MESSAGE_DELETE);
    deleteImage_->SetViewId(DELETE_BUTTON);
    deleteImage_->SetTouchable(true);
    deleteImage_->SetOnClickListener(this);
    InitMsgList();
    InitScrollData();
    Add(msgLable_);
    Add(deleteImage_);
    return true;
}

void MessageMainPage::PreLoad(void)
{
    if (! MessageModel::GetInstance().GetViewStatus()) {
        DeleteView();
        NoNotify();
        InitView();
        const std::vector<MessageItem> &itemInfo = MessageModel::GetInstance().GetMsgListItems();
        if (itemInfo.size() == 0) {
            msgLable_->SetVisible(true);
            deleteImage_->SetVisible(false);
            msgList_->SetVisible(false);
            deleteGroup_->SetVisible(false);
            noMsgLable_->SetVisible(true);
            noMsgImage_->SetVisible(true);
        } else {
            msgLable_->SetVisible(true);
            deleteImage_->SetVisible(true);
            msgList_->SetVisible(true);
            deleteGroup_->SetVisible(false);
            noMsgLable_->SetVisible(false);
            noMsgImage_->SetVisible(false);
        }
        MessageModel::GetInstance().SetViewStatus(true);
    }
}

void MessageMainPage::InitMsgList(void)
{
    msgList_ = new UISimpleList();
    if (msgList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new msgList_ is nullptr");
        return;
    }
    msgList_->SetPosition(MESAGE_LIST_X, MESAGE_LIST_Y, MESSAGE_LIST_WIDTH, MESSAGE_LIST_HEIGHT);
    msgList_->SetScrollBlankSize(MESSAGE_SCORLL_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);
    msgList_->SetDraggable(true);
    msgList_->SetThrowDrag(true);
    msgList_->EnableAutoAlign(true);
    msgList_->SetYScrollBarVisible(true);
    Point scorllBarPoint = {MESSAGE_SCORLL_BAR_X, MESSAGE_SCORLL_BAR_Y};
    msgList_->SetScrollBarCenter(scorllBarPoint);
    msgList_->SetOnDragListener(this);
    Add(msgList_);
}

void MessageMainPage::ConfirmDelete(void)
{
    if (deleteImage_->IsVisible()) {
        deleteImage_->SetVisible(false);
    }
    if (msgLable_->IsVisible()) {
        msgLable_->SetVisible(false);
    }
    if (msgList_->IsVisible()) {
        msgList_->SetVisible(false);
    }
    if (noMsgLable_->IsVisible()) {
        noMsgLable_->SetVisible(false);
    }
    if (noMsgImage_->IsVisible()) {
        noMsgImage_->SetVisible(false);
    }
    deleteGroup_->SetVisible(true);
}

void MessageMainPage::NewNotify(void)
{
    if (noMsgLable_->IsVisible()) {
        noMsgLable_->SetVisible(false);
    }
    if (noMsgImage_->IsVisible()) {
        noMsgImage_->SetVisible(false);
    }
    if (! deleteImage_->IsVisible()) {
        deleteImage_->SetVisible(true);
    }
    if (! msgLable_->IsVisible()) {
        msgLable_->SetVisible(true);
    }
    if (! msgList_->IsVisible()) {
        msgList_->SetVisible(true);
    }
}

void MessageMainPage::DeleteView(void)
{
    // deleteGroup_这个页面会多次创建，防止new多次 加入判空处理
    if (deleteGroup_ == nullptr) {
        deleteGroup_ = new UIScrollView;
        if (deleteGroup_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new deleteGroup_ is nullptr");
            return;
        }
    }
    deleteGroup_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, HORIZONTAL_RESOLUTION);
    // 标题
    deleteTips_ = new UILabel();
    if (deleteTips_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new deleteTips_ is nullptr");
        return;
    }
    deleteTips_->SetPosition(MESSAGE_DELETE_REMINDER_X, MESSAGE_DELETE_REMINDER_Y,
        MESSAGE_DELETE_REMINDER_WIDTH, MESSAGE_DELETE_REMINDER_HEIGHT);
    deleteTips_->SetText("是否清空消息通知？");
    deleteTips_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DELETE_REMINDER_FONT_SIZE);
    deleteTips_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    deleteTips_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);

    // 确认按钮
    confirm_ = new UIImageView();
    if (confirm_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new confirm_ is nullptr");
        return;
    }
    confirm_->SetPosition(MESSAGE_DELETE_CONFIRM_X, MESSAGE_DELETE_CONFIRM_Y,
        MESSAGE_DELETE_CONFIRM_SIZE, MESSAGE_DELETE_CONFIRM_SIZE);
    LOADIMG::LoadImageViewImg(confirm_, MESSAGE_IMAGE, IMAGE_MESSAGE_CONFIRM);
    confirm_->SetViewId(CONFIRM_BUTTON);
    confirm_->SetTouchable(true);
    confirm_->SetOnClickListener(this);

    // 取消按钮
    cancel_ = new UIImageView();
    if (cancel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new cancel_ is nullptr");
        return;
    }
    cancel_->SetPosition(MESSAGE_DELETE_CANCEL_X, MESSAGE_DELETE_CANCEL_Y,
        MESSAGE_DELETE_CANCEL_SIZE, MESSAGE_DELETE_CANCEL_SIZE);
    LOADIMG::LoadImageViewImg(cancel_, MESSAGE_IMAGE, IMAGE_MESSAGE_CANCEL);
    cancel_->SetViewId(CANCEL_BUTTON);
    cancel_->SetTouchable(true);
    cancel_->SetOnClickListener(this);

    deleteGroup_->Add(deleteTips_);
    deleteGroup_->Add(confirm_);
    deleteGroup_->Add(cancel_);
    Add(deleteGroup_);
}

void MessageMainPage::VerticalViewInits(void)
{
    deleteGroup_->SetVisible(false);
    if (! noMsgLable_->IsVisible()) {
        noMsgLable_->SetVisible(true);
    }
    if (! noMsgImage_->IsVisible()) {
        noMsgImage_->SetVisible(true);
    }
    if (! msgLable_->IsVisible()) {
        msgLable_->SetVisible(true);
    }
}

void MessageMainPage::NoNotify(void)
{
    noMsgLable_ = new UILabel();
    if (noMsgLable_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new noMsgLable_ fail");
        return;
    }
    noMsgLable_->SetPosition(NOMESSAGE_TITLE_X, NOMESSAGE_TITLE_Y,
        NOMESSAGE_TITLE_WIDTH, NOMESSAGE_TITLE_HEIGHT);
    noMsgLable_->SetText("没有通知");
    noMsgLable_->SetAlign(TEXT_ALIGNMENT_CENTER);
    noMsgLable_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, NOMESSAGE_TITLE_FONT_SIZE);
    noMsgLable_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    noMsgImage_ = new UIImageView();
    if (noMsgImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new noMsgImage_ is nullptr");
        return;
    }
    noMsgImage_->SetPosition(NOMESSAGE_ICON_X, NOMESSAGE_ICON_Y, NOMESSAGE_ICON_SIZE, NOMESSAGE_ICON_SIZE);
    LOADIMG::LoadImageViewImg(noMsgImage_, MESSAGE_IMAGE, IMAGE_MESSAGE_RING);
    Add(noMsgLable_);
    Add(noMsgImage_);
}

void MessageMainPage::CancelDelete(void)
{
    deleteGroup_->SetVisible(false);
    if (! msgLable_->IsVisible()) {
        msgLable_->SetVisible(true);
    }
    if (! deleteImage_->IsVisible()) {
        deleteImage_->SetVisible(true);
    }
    if (! msgList_->IsVisible()) {
        msgList_->SetVisible(true);
    }
}

static UIView* CreateViewCb(uint8_t type)
{
    MessageItemView* item = new MessageItemView();
    if (item == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new item is nullptr");
        return nullptr;
    }
    item->SetPosition(0, 0, HORIZONTAL_RESOLUTION, MESSAGE_ITEM_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    item->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    item->SetTouchable(true);
    item->SetOnClickListener(g_pMessageMainView);
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    MessageItemView* item = dynamic_cast<MessageItemView*>(view);
    if (data == nullptr || item == nullptr) {
        return;
    }
    MessageItem* content = static_cast<MessageItem *>(data);
    item->SetItemInfo(*content);
}

void MessageMainPage::RefreshMsgList(std::vector<MessageItem> &item)
{
    if (item.size() == 0) {
        msgLable_->SetVisible(true);
        deleteImage_->SetVisible(false);
        msgList_->SetVisible(false);
        deleteGroup_->SetVisible(false);
        noMsgLable_->SetVisible(true);
        noMsgImage_->SetVisible(true);
        return;
    }        
     // 设置msgList_数据
    msgList_->ClearAll();
    for (size_t i = 0; i < item.size(); ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 1;
        content.data = reinterpret_cast<void *>(&item[i]);
        msgList_->AddContent(content);
    }
    msgList_->RefreshList();
}

bool MessageMainPage::InitScrollData()
{
    std::vector<MessageItem> &itemInfo = MessageModel::GetInstance().GetMsgListItems();
    RefreshMsgList(itemInfo);
    return true;
}

bool MessageMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), DELETE_BUTTON) == 0) {
        ConfirmDelete();
    } else if (strcmp(view.GetViewId(), CONFIRM_BUTTON) == 0) {
        VerticalViewInits();
        MessageModel::GetInstance().ClearListItem();
        MessageModel::GetInstance().SetIndex(0);
    } else if (strcmp(view.GetViewId(), CANCEL_BUTTON) == 0) {
        CancelDelete();
    } else {
        // 点击的是消息item条目
        MessageItemView *MessageItemViewID = static_cast<MessageItemView *>(&view);
        noMsgLable_->SetVisible(false);
        msgLable_->SetVisible(false);
        noMsgImage_->SetVisible(false);
        deleteImage_->SetVisible(false);
        msgList_->SetVisible(false);
        LoadDetailsView();
        detailsGroup_->detailsMsg_ = MessageItemViewID->GetItem();
        detailsGroup_->RefreshNotify();
    }
    return true;
}

bool MessageMainPage::OnDragStartEvent(const DragEvent &event)
{
    UNUSED(event);
    return true;
}

bool MessageMainPage::OnDragEvent(const DragEvent &event)
{
    int16_t deltaX = event.GetDeltaX();
    int16_t deltaY = event.GetDeltaY();
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT &&
        (MATH_ABS(deltaX) >= MATH_ABS(deltaY) && MATH_ABS(deltaX) > MOVE_DISTANCE)) {
        // 返回主页面
        DeleteDetailsView();
        noMsgLable_->SetVisible(true);
        msgLable_->SetVisible(true);
        noMsgImage_->SetVisible(true);
        deleteImage_->SetVisible(true);
        msgList_->SetVisible(true);
        return true;
    }
    return true;
}

void MessageMainPage::LoadDetailsView(void)
{
    detailsGroup_ = new DetailsGroup();
    if (detailsGroup_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new detailsGroup_ is nullptr");
        return;
    }
    Add(detailsGroup_);
}

void MessageMainPage::DeleteDetailsView(void)
{
    if (detailsGroup_ != nullptr) {
        Remove(detailsGroup_);
        delete detailsGroup_;
        detailsGroup_ = nullptr;
    }
}

bool MessageMainPage::OnDragEndEvent(const DragEvent &event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM ||
        event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        std::vector<MessageItem> &itemInfo = MessageModel::GetInstance().GetMsgListItems();
        for (size_t i = 0; i < itemInfo.size(); ++i) {
            itemInfo[i].status = false;
        }
        RefreshMsgList(itemInfo);
    }
    return true;
}
}
