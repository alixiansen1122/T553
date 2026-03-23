/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallerLogItemView
 * Create: 2025-07-12
 */

#include <string>
#include <sys/time.h>
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "phonemenu/PhoneMenuCallerLogItemView.h"

namespace OHOS {
static constexpr uint16_t BG_GROUP_W = 418;
static constexpr uint16_t BG_GROUP_H = 128;
static constexpr uint16_t BG_BACKGROUND_OPA = 48;
static constexpr uint16_t BG_BORDER_RADIUS = 65;
static constexpr uint16_t ICON_X = 40;
static constexpr uint16_t ICON_Y = 40;
static constexpr uint16_t PHONE_LABEL_X = 107;
static constexpr uint16_t PHONE_LABEL_Y = 20;
static constexpr uint16_t PHONE_LABEL_W = 230;
static constexpr uint16_t PHONE_LABEL_H = 46;
static constexpr uint16_t PHONE_LABEL_FONT = 36;
static constexpr uint16_t STATUS_LABEL_X = 107;
static constexpr uint16_t STATUS_LABEL_Y = 66;
static constexpr uint16_t STATUS_LABEL_W = 268;
static constexpr uint16_t STATUS_LABEL_H = 42;
static constexpr uint16_t STATUS_LABEL_FONT = 30;
static constexpr uint16_t NUM_10 = 10;

PhoneMenuCallerLogItemView::PhoneMenuCallerLogItemView()
{
    bg_.SetPosition(0, 0, BG_GROUP_W, BG_GROUP_H);
    bg_.SetStyle(STYLE_BACKGROUND_OPA, BG_BACKGROUND_OPA);
    bg_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    bg_.SetStyle(STYLE_BORDER_RADIUS, BG_BORDER_RADIUS);

    icon_.SetPosition(ICON_X, ICON_Y);

    phoneLabel_.SetPosition(PHONE_LABEL_X, PHONE_LABEL_Y, PHONE_LABEL_W, PHONE_LABEL_H);
    phoneLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, PHONE_LABEL_FONT);
    phoneLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);

    stutasLabel_.SetPosition(STATUS_LABEL_X, STATUS_LABEL_Y, STATUS_LABEL_W, STATUS_LABEL_H);
    stutasLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, STATUS_LABEL_FONT);
    stutasLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    stutasLabel_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    Add(&bg_);
    Add(&icon_);
    Add(&stutasLabel_);
    Add(&phoneLabel_);
}

PhoneMenuCallerLogItemView::~PhoneMenuCallerLogItemView()
{
    Remove(&bg_);
    Remove(&icon_);
    Remove(&stutasLabel_);
    Remove(&phoneLabel_);
}

void PhoneMenuCallerLogItemView::SetItemInfo(const CallLogInfo &itemInfo)
{
    memset_s(tel_, MAX_PHONE_NUM, 0, MAX_PHONE_NUM);
    memcpy_s(tel_, MAX_PHONE_NUM, itemInfo.tel, itemInfo.telLen);
    phoneLabel_.SetText(itemInfo.name);
    if (strlen(itemInfo.name) == 0 || itemInfo.nameLen == 0) {
        phoneLabel_.SetText((const char *)itemInfo.tel);
    }
    std::string timeStr;
    struct tm pTM;
    localtime_r((time_t *)&itemInfo.creationTime, &pTM);
    if (pTM.tm_mon < NUM_10) {
        timeStr += "0";
    }
    timeStr += std::to_string(pTM.tm_mon) + "/";
    if (pTM.tm_mday < NUM_10) {
        timeStr += "0";
    }
    timeStr += std::to_string(pTM.tm_mday);
    if (itemInfo.status == CALL_MISSED) {
        stutasLabel_.SetText(("未接来电   " + timeStr).c_str());
        phoneLabel_.SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
        LOADIMG::LoadImageViewImg(&icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALLIN);
    } else if (itemInfo.status == CALL_INCOMING) {
        stutasLabel_.SetText(("接听        " + timeStr).c_str());
        LOADIMG::LoadImageViewImg(&icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALLIN);
    } else if (itemInfo.status == CALL_OUTGOING) {
        stutasLabel_.SetText(("拨号        " + timeStr).c_str());
        LOADIMG::LoadImageViewImg(&icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALLOUT);
    }
    SetViewId("CallerLogItem");
}

unsigned char *PhoneMenuCallerLogItemView::GetItemPhoneNumber()
{
    return tel_;
}
}
