/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageDetailsGroup
 * Created: 2025-06
 */

#include <string>
#include "common/image_cache_manager.h"
#include "NativeAbility.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "main/MessageDetailsGroup.h"

namespace OHOS {
static constexpr int16_t MESSAGE_ICON_X = 130;
static constexpr int16_t MESSAGE_ICON_Y = 50;
static constexpr int16_t MESSAGE_ICON_SIZE = 40;
static constexpr int16_t MESSAGE_TITLES_X = 170;
static constexpr int16_t MESSAGE_TITLES_Y = 40;
static constexpr int16_t MESSAGE_TITLES_WIDTH = 160;
static constexpr int16_t MESSAGE_TITLES_HEIGHT = 53;
static constexpr int16_t MESSAGE_TITLES_FONT_SIZE = 38;
static constexpr int16_t MESSAGE_CONTACTS_X = 15;
static constexpr int16_t MESSAGE_CONTACTS_Y = 138;
static constexpr int16_t MESSAGE_CONTACTS_WIDTH = 370;
static constexpr int16_t MESSAGE_CONTACTS_HEIGHT = 50;
static constexpr int16_t MESSAGE_CONTACTA_FONT_SIZE = 36;
static constexpr int16_t MESSAGE_CONTENTS_X = 15;
static constexpr int16_t MESSAGE_CONTENTS_Y = 201;
static constexpr int16_t MESSAGE_CONTENTS_WIDTH = 370;
static constexpr int16_t MESSAGE_CONTENTS_HEIGHT = 200;
static constexpr int16_t MESSAGE_CONTENTA_FONT_SIZE = 36;
static constexpr int16_t MESSAGE_TIME_X = 77;
static constexpr int16_t MESSAGE_TIME_Y = 385;
static constexpr int16_t MESSAGE_TIME_WIDTH = 300;
static constexpr int16_t MESSAGE_TIME_HEIGHT = 40;
static constexpr int16_t MESSAGE_TIME_FONT_SIZE = 36;
constexpr int16_t PAGE_WIDTH = 454;

DetailsGroup::DetailsGroup()
{
    InitDetailsPage();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DetailsGroup::DetailsGroup");
}

DetailsGroup::~DetailsGroup()
{
    RemoveAll();
    if (detailsMsgIcon_ != nullptr) {
        delete detailsMsgIcon_;
        detailsMsgIcon_ = nullptr;
    }
    if (detailsMsgTitle_ != nullptr) {
        delete detailsMsgTitle_;
        detailsMsgTitle_ = nullptr;
    }
    if (detailsMsgContacts_ != nullptr) {
        delete detailsMsgContacts_;
        detailsMsgContacts_ = nullptr;
    }
    if (detailsMsgContents_ != nullptr) {
        delete detailsMsgContents_;
        detailsMsgContents_ = nullptr;
    }
    if (detailsMsgTime_ != nullptr) {
        delete detailsMsgTime_;
        detailsMsgTime_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DetailsGroup::~DetailsGroup");
}

void DetailsGroup::InitDetailsPage()
{
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, HORIZONTAL_RESOLUTION);
    SetViewId("detail");
    SetThrowDrag(true);
    SetScrollBlankSize(31);

    detailsMsgIcon_ = new UIImageView;
    if (detailsMsgIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new detailsMsgIcon_ fail");
        return;
    }
    detailsMsgIcon_->SetPosition(MESSAGE_ICON_X, MESSAGE_ICON_Y);

    detailsMsgTitle_ = new UILabel;
    if (detailsMsgTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new detailsMsgTitle_ fail");
        return;
    }
    detailsMsgTitle_->SetPosition(MESSAGE_TITLES_X, MESSAGE_TITLES_Y, MESSAGE_TITLES_WIDTH, MESSAGE_TITLES_HEIGHT);
    detailsMsgTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, MESSAGE_TITLES_FONT_SIZE);
    detailsMsgTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    detailsMsgTitle_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    detailsMsgTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    detailsMsgTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    detailsMsgTitle_->SetVisible(true);

    detailsMsgContacts_ = new UILabel;
    if (detailsMsgContacts_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new detailsMsgContacts_ fail");
        return;
    }
    detailsMsgContacts_->SetPosition(MESSAGE_CONTACTS_X, MESSAGE_CONTACTS_Y,
        MESSAGE_CONTACTS_WIDTH, MESSAGE_CONTACTS_HEIGHT);
    detailsMsgContacts_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MESSAGE_CONTACTA_FONT_SIZE);
    detailsMsgContacts_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    detailsMsgContacts_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    detailsMsgContacts_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    detailsMsgContacts_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    detailsMsgContacts_->SetVisible(true);

    detailsMsgContents_ = new UILabel;
    if (detailsMsgContents_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new detailsMsgContents_ fail");
        return;
    }
    detailsMsgContents_->SetPosition(MESSAGE_CONTENTS_X, MESSAGE_CONTENTS_Y,
        MESSAGE_CONTENTS_WIDTH, MESSAGE_CONTENTS_HEIGHT);
    detailsMsgContents_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MESSAGE_CONTENTA_FONT_SIZE);
    detailsMsgContents_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_TOP);
    detailsMsgContents_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    detailsMsgContents_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    detailsMsgContents_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    detailsMsgContents_->SetVisible(true);

    detailsMsgTime_ = new UILabel;
    if (detailsMsgTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new detailsMsgTime_ fail");
        return;
    }
    detailsMsgTime_->SetPosition(MESSAGE_TIME_X, MESSAGE_TIME_Y, MESSAGE_TIME_WIDTH, MESSAGE_TIME_HEIGHT);
    detailsMsgTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MESSAGE_TIME_FONT_SIZE);
    detailsMsgTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    detailsMsgTime_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    detailsMsgTime_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    detailsMsgTime_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    detailsMsgTime_->SetVisible(true);

    Add(detailsMsgIcon_);
    Add(detailsMsgTitle_);
    Add(detailsMsgContacts_);
    Add(detailsMsgContents_);
    Add(detailsMsgTime_);
}

void DetailsGroup::RefreshNotify()
{
    detailsMsgTitle_->SetText(detailsMsg_.appName.c_str());
    if (strcmp(detailsMsg_.appName.c_str(), "微信") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_WX);
    } else if (strcmp(detailsMsg_.appName.c_str(), "QQ") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_QQ);
    } else if (strcmp(detailsMsg_.appName.c_str(), "抖音") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DY);
    } else if (strcmp(detailsMsg_.appName.c_str(), "日历") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_CALENDAR);
    } else if (strcmp(detailsMsg_.appName.c_str(), "信息") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DUANXIN);
    } else if (strcmp(detailsMsg_.appName.c_str(), "短信") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_DUANXIN);
    } else if (strcmp(detailsMsg_.appName.c_str(), "今日头条") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_TOUTIAO);
    } else if (strcmp(detailsMsg_.appName.c_str(), "微博") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_WB);
    } else if (strcmp(detailsMsg_.appName.c_str(), "小红书") == 0) {
        LOADIMG::LoadImageViewImg(detailsMsgIcon_, MESSAGE_IMAGE, IMAGE_MESSAGE_XHS);
    }
    int controlOffset = 10;
    int cut = 2;
    int posX = (PAGE_WIDTH - detailsMsgTitle_->GetTextWidth() - detailsMsgIcon_->GetWidth() - controlOffset) / cut;
    detailsMsgIcon_->SetX(posX);
    detailsMsgTitle_->SetWidth(detailsMsgTitle_->GetTextWidth());
    detailsMsgTitle_->SetX(posX + detailsMsgIcon_->GetWidth() + controlOffset);

    std::string str = detailsMsg_.appContact + "：";
    detailsMsgContacts_->SetText(str.c_str());
    if (detailsMsgContacts_->GetTextWidth() > MESSAGE_CONTACTS_WIDTH) {
        detailsMsgContacts_->SetWidth(MESSAGE_CONTACTS_WIDTH);
    } else {
        detailsMsgContacts_->SetWidth(detailsMsgContacts_->GetTextWidth());
    }
    posX = (PAGE_WIDTH - detailsMsgContacts_->GetWidth()) / cut;
    detailsMsgContacts_->SetX(posX);

    detailsMsgContents_->SetText(detailsMsg_.appContents.c_str());
    int height = detailsMsgContents_->GetTextHeight();
    detailsMsgContents_->SetHeight(height);
    posX = (PAGE_WIDTH - detailsMsgContents_->GetWidth()) / cut;
    detailsMsgContents_->SetX(posX);

    detailsMsgTime_->SetText(detailsMsg_.time.c_str());
    int posY = detailsMsgContents_->GetY() + height + (cut * controlOffset);
    detailsMsgTime_->SetY(posY);
    posX = (PAGE_WIDTH - detailsMsgTime_->GetWidth()) / cut;
}

}