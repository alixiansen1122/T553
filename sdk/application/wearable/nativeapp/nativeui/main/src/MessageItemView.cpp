/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageItemView
 * Create: 2025-06-29
 */

#include "wearable_log.h"
#include "ui_resource_message.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"
#include "main/MessageMainPage.h"
#include "main/MessageItemView.h"

namespace OHOS {
static constexpr int16_t MESSAGE_ITEM_GROUP_X = 30;
static constexpr int16_t MESSAGE_ITEM_GROUP_EX = -100;
static constexpr int16_t MESSAGE_ITEM_GROUP_MOVEX = 100;
static constexpr int16_t MESSAGE_ITEM_GROUP_WIDTH = 454;
static constexpr int16_t MESSAGE_ITEM_GROUP_HEIGHT = 130;
static constexpr int16_t MESSAGE_ITEM_DELETE_X = 283;
static constexpr int16_t MESSAGE_ITEM_DELETE_SIZE = 36;
static constexpr int16_t MESSAGE_ITEM_APPNAME_X = 60;
static constexpr int16_t MESSAGE_ITEM_APPNAME_Y = 15;
static constexpr int16_t MESSAGE_ITEM_APPNAME_WIDTH = 131;
static constexpr int16_t MESSAGE_ITEM_APPNAME_HEIGHT = 45;
static constexpr int16_t ITEM_APPNAME_FONT_SIZE = 32;
static constexpr int16_t MESSAGE_ITEM_APPICON_X = 10;
static constexpr int16_t MESSAGE_ITEM_APPICON_Y = 15;
static constexpr int16_t MESSAGE_ITEM_APPICON_SIZE = 30;
static constexpr int16_t MESSAGE_ITEM_CONTACT_X = 10;
static constexpr int16_t MESSAGE_ITEM_CONTACT_Y = 73;
static constexpr int16_t MESSAGE_ITEM_CONTACT_WIDTH = 95;
static constexpr int16_t MESSAGE_ITEM_CONTACT_HEIGHT = 42;
static constexpr int16_t ITEM_CONTACT_FONT_SIZE = 30;
static constexpr int16_t MESSAGE_ITEM_INFOR_X = 110;
static constexpr int16_t MESSAGE_ITEM_INFOR_Y = 73;
static constexpr int16_t MESSAGE_ITEM_INFOR_WIDTH = 156;
static constexpr int16_t MESSAGE_ITEM_INFOR_HEIGHT = 42;
static constexpr int16_t ITEM_INFOR_FONT_SIZE = 30;
static constexpr int16_t MESSAGE_ITEM_TIME_X = 270;
static constexpr int16_t MESSAGE_ITEM_TIME_Y = 75;
static constexpr int16_t MESSAGE_ITEM_TIME_WIDTH = 100;
static constexpr int16_t MESSAGE_ITEM_TIME_HEIGHT = 36;
static constexpr int16_t ITEM_TIME_FONT_SIZE = 26;
static constexpr char *DELETE_BUTTON = "deleteButton";

MessageItemView::MessageItemView()
{
    appName_.SetPosition(MESSAGE_ITEM_APPNAME_X, MESSAGE_ITEM_APPNAME_Y,
        MESSAGE_ITEM_APPNAME_WIDTH, MESSAGE_ITEM_APPNAME_HEIGHT);
    appName_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ITEM_APPNAME_FONT_SIZE);
    appName_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    appName_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    appName_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    appName_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    appIcon_.SetPosition(MESSAGE_ITEM_APPICON_X, MESSAGE_ITEM_APPICON_Y,
        MESSAGE_ITEM_APPICON_SIZE, MESSAGE_ITEM_APPICON_SIZE);
    appIcon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);

    appContact_.SetPosition(MESSAGE_ITEM_CONTACT_X, MESSAGE_ITEM_CONTACT_Y,
        MESSAGE_ITEM_CONTACT_WIDTH, MESSAGE_ITEM_CONTACT_HEIGHT);
    appContact_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ITEM_CONTACT_FONT_SIZE);
    appContact_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    appContact_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    appContact_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    appContact_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    appContents_.SetPosition(MESSAGE_ITEM_INFOR_X, MESSAGE_ITEM_INFOR_Y,
        MESSAGE_ITEM_INFOR_WIDTH, MESSAGE_ITEM_INFOR_HEIGHT);
    appContents_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ITEM_INFOR_FONT_SIZE);
    appContents_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    appContents_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    appContents_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    appContents_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    msgTime_.SetPosition(MESSAGE_ITEM_TIME_X, MESSAGE_ITEM_TIME_Y,
        MESSAGE_ITEM_TIME_WIDTH, MESSAGE_ITEM_TIME_HEIGHT);
    msgTime_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ITEM_TIME_FONT_SIZE);
    msgTime_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    msgTime_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    msgTime_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    msgTime_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    LOADIMG::LoadImageViewImg(&bgImg_, MESSAGE_IMAGE, IMAGE_MESSAGE_BG);

    group_.SetPosition(MESSAGE_ITEM_GROUP_X, 0, MESSAGE_ITEM_GROUP_WIDTH, MESSAGE_ITEM_GROUP_HEIGHT);
    group_.SetViewId("message");
    group_.SetDraggable(true);
    group_.SetTouchable(true);

    deleteImg_.SetPosition(MESSAGE_ITEM_DELETE_X, 0, MESSAGE_ITEM_DELETE_SIZE, MESSAGE_ITEM_DELETE_SIZE);
    LOADIMG::LoadImageViewImg(&deleteImg_, MESSAGE_IMAGE, IMAGE_MESSAGE_SINGLE_DELETE);
    deleteImg_.SetViewId(DELETE_BUTTON);
    deleteImg_.SetTouchable(true);
    deleteImg_.SetOnClickListener(this);

    group_.Add(&bgImg_);
    group_.Add(&appName_);
    group_.Add(&appIcon_);
    group_.Add(&appContact_);
    group_.Add(&appContents_);
    group_.Add(&msgTime_);
    Add(&group_);
    Add(&deleteImg_);

}

MessageItemView::~MessageItemView()
{
    group_.RemoveAll();
    Remove(&appIcon_);
    Remove(&appName_);
    Remove(&appContact_);
    Remove(&appContents_);
    Remove(&bgImg_);
    Remove(&msgTime_);
    Remove(&group_);
    Remove(&deleteImg_);
}

void MessageItemView::SetItemInfo(const MessageItem itemInfo)
{
    msgItemInfo_ = itemInfo;
    appName_.SetText(itemInfo.appName.c_str());
    appContact_.SetText(itemInfo.appContact.c_str());
    appContents_.SetText(itemInfo.appContents.c_str());
    msgTime_.SetText(itemInfo.time.c_str());
    // 获取label1的内容更改图标
    if (strcmp(msgItemInfo_.appName.c_str(), "微信") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_WX);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "QQ") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_QQ);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "抖音") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DY);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "日历") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_CALENDAR);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "信息") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DUANXIN);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "短信") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DUANXIN);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "今日头条") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_TOUTIAO);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "微博") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_WB);
    } else if (strcmp(msgItemInfo_.appName.c_str(), "小红书") == 0) {
        LOADIMG::LoadImageViewImg(&appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_XHS);
    }
    if (msgItemInfo_.status == false) {
        group_.SetPosition(MESSAGE_ITEM_GROUP_X, 0, MESSAGE_ITEM_GROUP_WIDTH, MESSAGE_ITEM_GROUP_HEIGHT);
    } else {
        group_.SetPosition(MESSAGE_ITEM_GROUP_EX, 0, MESSAGE_ITEM_GROUP_WIDTH, MESSAGE_ITEM_GROUP_HEIGHT);
    }
    deleteImg_.SetVisible(msgItemInfo_.status);
}

MessageItem MessageItemView::GetItem()
{
    return msgItemInfo_;
}

void MessageItemView::SetIndex(uint16_t index)
{
    itemIndex_ = index;
}

uint16_t MessageItemView ::GetIndex()
{
    return itemIndex_;
}

bool MessageItemView::OnDragStartEvent(const DragEvent& event)
{
    return false;
}

bool MessageItemView::OnDragEvent(const DragEvent& event)
{
    // 检查拖拽方向是否为左到右，并且X坐标在允许范围内（0到100之间）
    if ((event.GetDragDirection() == DragEvent::DIRECTION_RIGHT_TO_LEFT &&
        (((0 - group_.GetX()) <= MESSAGE_ITEM_GROUP_MOVEX)))) {
        int pox = group_.GetX() + event.GetDeltaX();
        group_.SetX(pox);
        group_.Invalidate();
        Invalidate();
        return true;
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        group_.SetX(MESSAGE_ITEM_GROUP_X);
        group_.Invalidate();
        deleteImg_.SetVisible(false);
        return true;
    }
    return false;
}

bool MessageItemView::OnDragEndEvent(const DragEvent& event)
{
    // 超出一定距离 显示X按钮
    if ((0 - group_.GetX()) >= MESSAGE_ITEM_GROUP_MOVEX) {
        group_.SetX(MESSAGE_ITEM_GROUP_EX);
        group_.Invalidate();
        deleteImg_.SetVisible(true);
        msgItemInfo_.status = true;
        SetIndex(msgItemInfo_.index);
        // 恢复其它item位置
        UpdateItemStatus();
        Invalidate();
        return true;
    } else {
        group_.SetPosition(MESSAGE_ITEM_GROUP_X, 0,
            MESSAGE_ITEM_GROUP_WIDTH, MESSAGE_ITEM_GROUP_HEIGHT);
        group_.Invalidate();
    }
    return false;
}

bool MessageItemView::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), DELETE_BUTTON) == 0) {
        deleteImg_.SetVisible(false);
        // 单项删除
        MessageModel::GetInstance().DeleteItem(msgItemInfo_);
        return true;
    }
    return false;
}

// 恢复item的位置
void MessageItemView::UpdateItemStatus()
{
    std::vector<MessageItem> &vecMsg = MessageModel::GetInstance().GetMsgListItems();
    for (int i = 0; i < vecMsg.size(); ++i) {
        if (vecMsg[i].index == itemIndex_) {
            vecMsg[i].status = true;
        } else {
            vecMsg[i].status = false;
        }
    }
    MessageMainPage::GetInstance()->RefreshMsgList(vecMsg);
}
}
