/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: phone message notification
 * Create: 2025-04-25
 */

#include "notification_manager.h"
#include "ohos_timer.h"
#include "wearable_log.h"
#include <string>
#include "main/MessageModel.h"
#include "ui_resource_message.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"

namespace OHOS {
// 三方应用名
static const uint32_t APP_NAME_POS_X = 170;
static const uint32_t APP_NAME_POS_Y = 40;
static const uint32_t APP_NAME_WIDTH = 160;
static const uint32_t APP_NAME_HEIGHT = 53;
// 三方消息主题
static const uint32_t MSG_TITLE_POS_X = 15;
static const uint32_t MSG_TITLE_POS_Y = 138;
static const uint32_t MSG_TITLE_WIDTH = 370;
static const uint32_t MSG_TITLE_HEIGHT = 50;
// 三方消息内容
static const uint32_t MSG_CONTENT_POS_X = 15;
static const uint32_t MSG_CONTENT_POS_Y = 201;
static const uint32_t MSG_CONTENT_WIDTH = 370;
static const uint32_t MSG_CONTENT_HEIGHT = 200;
// 通知图标
static const uint32_t MSG_ICON_POS_X = 130;
static const uint32_t MSG_ICON_POS_Y = 50;
static const uint32_t MSG_ICON_WIDTH = 40;
static const uint32_t MSG_ICON_HEIGHT = 40;
static const uint32_t MSG_PAGE_WIDTH = 454;

// 设置字体大小
static const uint32_t APP_NAME_FONT_SIZE = 38;
static const uint32_t MSG_TITLE_FONT_SIZE = 36;
static const uint32_t MSG_CONTENT_FONT_SIZE = 36;

UIScrollView *PhoneMsgView::phoneMsgView_ = nullptr;
UILabel *PhoneMsgView::appNameLabel_ = nullptr;
UIImageView *PhoneMsgView::appIcon_ = nullptr;
UILabel *PhoneMsgView::titleLabel_ = nullptr;
UILabel *PhoneMsgView::contentLabel_ = nullptr;
GraphicTimer *PhoneMsgView::phoneMsgNotifyStop_ = nullptr;
PhoneMsg PhoneMsgView::phoneMsg_;

void PhoneMsgStopNotifyCallback(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMsgStopNotifyCallback");
    (void)data;
    NotificationManager::GetInstance()->StopNotify();
}

void PhoneMsgView::StartPhoneMsgTimer(int32_t interval)
{
    uint32_t tick = GetOSTick(interval);
    phoneMsgNotifyStop_ = new GraphicTimer(tick, PhoneMsgStopNotifyCallback, nullptr, true);
    phoneMsgNotifyStop_->Start();
}

void PhoneMsgView::DisablePhoneMsgTimer()
{
    if (phoneMsgNotifyStop_ != nullptr) {
        phoneMsgNotifyStop_->Stop();
        delete phoneMsgNotifyStop_;
        phoneMsgNotifyStop_ = nullptr;
    }
}

UIViewGroup *PhoneMsgView::CreatePhoneMsgView()
{
    if (phoneMsgView_ == nullptr) {
        phoneMsgView_ = new UIScrollView();
        phoneMsgView_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        phoneMsgView_->SetViewId("phoneMsgView");
        phoneMsgView_->SetThrowDrag(true);
        phoneMsgView_->SetScrollBlankSize(40);
    }

    if (appNameLabel_ == nullptr) {
        appNameLabel_ = new UILabel();
    }
    appNameLabel_->SetPosition(APP_NAME_POS_X, APP_NAME_POS_Y, APP_NAME_WIDTH, APP_NAME_HEIGHT);
    appNameLabel_->SetText(phoneMsg_.appName.c_str());
    appNameLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, APP_NAME_FONT_SIZE);
    appNameLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    phoneMsgView_->Add(appNameLabel_);

    if (appIcon_ == nullptr) {
        appIcon_ = new UIImageView();
    }
    appIcon_->SetPosition(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT);
    phoneMsgView_->Add(appIcon_);
    appIcon_->SetVisible(true);

    if (titleLabel_ == nullptr) {
        titleLabel_ = new UILabel();
    }
    titleLabel_->SetPosition(MSG_TITLE_POS_X, MSG_TITLE_POS_Y, MSG_TITLE_WIDTH, MSG_TITLE_HEIGHT);
    titleLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MSG_TITLE_FONT_SIZE);
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    phoneMsgView_->Add(titleLabel_);

    if (contentLabel_ == nullptr) {
        contentLabel_ = new UILabel();
    }
    contentLabel_->SetPosition(MSG_CONTENT_POS_X, MSG_CONTENT_POS_Y, MSG_CONTENT_WIDTH, MSG_CONTENT_HEIGHT);
    contentLabel_->SetText(phoneMsg_.msgContent.c_str());
    contentLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MSG_CONTENT_FONT_SIZE);
    contentLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    contentLabel_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    phoneMsgView_->Add(contentLabel_);

    SetControl();
    phoneMsgView_->Invalidate();
    phoneMsgView_->SetDraggable(true);
    phoneMsgView_->SetTouchable(true);
    UIView::OnDragListener *dragListener =
        static_cast<UIView::OnDragListener *>(PhoneMsgViewEventListener::GetInstance());
    UIView::OnClickListener *clickListener =
        static_cast<UIView::OnClickListener *>(PhoneMsgViewEventListener::GetInstance());
    phoneMsgView_->SetOnDragListener(dragListener);
    phoneMsgView_->SetOnClickListener(clickListener);
    StartPhoneMsgTimer(PhoneMsgView::notifyTime);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreePhoneMsgView);
    return phoneMsgView_;
}

void PhoneMsgView::SetControl()
{
    if (strcmp(phoneMsg_.appName.c_str(), "微信") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_WX);
    } else if (strcmp(phoneMsg_.appName.c_str(), "QQ") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_QQ);
    } else if (strcmp(phoneMsg_.appName.c_str(), "抖音") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DY);
    } else if (strcmp(phoneMsg_.appName.c_str(), "日历") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_CALENDAR);
    } else if (strcmp(phoneMsg_.appName.c_str(), "信息") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DUANXIN);
    } else if (strcmp(phoneMsg_.appName.c_str(), "短信") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DUANXIN);
    } else if (strcmp(phoneMsg_.appName.c_str(), "今日头条") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_TOUTIAO);
    } else if (strcmp(phoneMsg_.appName.c_str(), "微博") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_WB);
    } else if (strcmp(phoneMsg_.appName.c_str(), "小红书") == 0) {
            LOADIMG::LoadImageViewImg(appIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_XHS);
    }
    int controlOffset = 10;
    int cut = 2;
    int posX = (MSG_PAGE_WIDTH - appNameLabel_->GetTextWidth() - appIcon_->GetWidth() - controlOffset) / cut;
    appIcon_->SetX(posX);
    appNameLabel_->SetWidth(appNameLabel_->GetTextWidth());
    appNameLabel_->SetX(posX + appIcon_->GetWidth() + controlOffset);

    titleLabel_->SetText(phoneMsg_.msgTitle.c_str());
    if (titleLabel_->GetTextWidth() > MSG_TITLE_WIDTH) {
        titleLabel_->SetWidth(MSG_TITLE_WIDTH);
    } else {
        titleLabel_->SetWidth(titleLabel_->GetTextWidth());
    }
    titleLabel_->SetX((MSG_PAGE_WIDTH - titleLabel_->GetWidth()) / cut);

    int height = contentLabel_->GetTextHeight();
    contentLabel_->SetHeight(height);
    posX = (MSG_PAGE_WIDTH - contentLabel_->GetWidth()) / cut;
    contentLabel_->SetX(posX);
}

void PhoneMsgView::FreePhoneMsgView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FreePhoneMsgView");
    if (phoneMsgView_ != nullptr) {
        DisablePhoneMsgTimer();
        phoneMsgView_->RemoveAll();
        RootView::GetInstance()->Remove(phoneMsgView_);
        delete phoneMsgView_;
        phoneMsgView_ = nullptr;
    }
    if (appNameLabel_ != nullptr) {
        delete appNameLabel_;
        appNameLabel_ = nullptr;
    }
    if (appIcon_ != nullptr) {
        delete appIcon_;
        appIcon_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (contentLabel_ != nullptr) {
        delete contentLabel_;
        contentLabel_ = nullptr;
    }
}

void PhoneMsgView::UpDatePhoneMsg(std::string appName, std::string titile, std::string content)
{
    phoneMsg_.appName = appName;
    phoneMsg_.msgTitle = titile;
    phoneMsg_.msgContent = content;
    MessageModel::GetInstance().SaveMsgToModel(appName, titile, content);
    MessageModel::GetInstance().SetMsgListItems();
}

static OHOS::UIViewGroup* ShowMsgNotify(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ShowMsgNotify");
    return OHOS::PhoneMsgView::CreatePhoneMsgView();
}

void PhoneMsgView::ShowDateMsg(std::string appName, std::string title, std::string content)
{
    const uint16_t TOPIC_DEMO_ALL_MSG = 10001;
    UpDatePhoneMsg(appName, title, content);
    int prio = 2;
    NotificationManager::GetInstance()->SetNotifyPriority(prio);
    NotificationManager::GetInstance()->ShowNotify(ShowMsgNotify, static_cast<uint16_t>(TOPIC_DEMO_ALL_MSG), true);
}

PhoneMsgViewEventListener::PhoneMsgViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMsgViewEventListener :%p", this);
}

PhoneMsgViewEventListener::~PhoneMsgViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~PhoneMsgViewEventListener");
}

PhoneMsgViewEventListener *PhoneMsgViewEventListener::GetInstance(void)
{
    static PhoneMsgViewEventListener phoneMsgViewEventListener;
    return &phoneMsgViewEventListener;
}

bool PhoneMsgViewEventListener::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}

bool PhoneMsgViewEventListener::OnDragEnd(UIView &view, const DragEvent &event)
{
    std::string viewId = view.GetViewId();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMsgViewEventListener OnDragEnd %s", viewId.c_str());
    if (viewId == "phoneMsgView") {
        // NotificationManager::GetInstance()->StopNotify();
    }
    return true;
}

bool PhoneMsgViewEventListener::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    std::string viewId = view.GetViewId();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMsgViewEventListener ClickListener %s", viewId.c_str());
    if (viewId == "phoneMsgView") {
        NotificationManager::GetInstance()->StopNotify();
    }
    return true;
}
}  // namespace OHOS