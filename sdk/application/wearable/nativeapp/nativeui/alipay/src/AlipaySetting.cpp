/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay setting view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipaySetting.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const char *SETTING_TITLE = (char *)"设置";
static const char *SETTING_BUTTON = (char *)"解除绑定";

static const int16 SETTING_TITLE_FONT = 38;
static const int16 SETTING_TITLE_X = 0;
static const int16 SETTING_TITLE_Y = 35;
static const int16 SETTING_TITLE_WIDTH = 454;
static const int16 SETTING_TITLE_HEIGHT = 53;

static const int16 SETTING_NAME_FONT = 38;
static const int16 SETTING_NAME_X = 0;
static const int16 SETTING_NAME_Y = 142;
static const int16 SETTING_NAME_WIDTH = 454;
static const int16 SETTING_NAME_HEIGHT = 53;

static const int16 SETTING_ID_FONT = 30;
static const int16 SETTING_ID_X = 0;
static const int16 SETTING_ID_Y = 198;
static const int16 SETTING_ID_WIDTH = 454;
static const int16 SETTING_ID_HEIGHT = 109;

static const int16 SETTING_BUTTON_X = 121;
static const int16 SETTING_BUTTON_Y = 320;
static const int16 SETTING_BUTTON_WIDTH = 212;
static const int16 SETTING_BUTTON_HEIGHT = 73;
static const int16 SETTING_BUTTON_FONT = 38;

AlipaySetting::AlipaySetting(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipaySetting::AlipaySetting");
    presenter_ = presenter;
}

AlipaySetting::~AlipaySetting()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipaySetting::~AlipaySetting");
    RemoveAll();

    if (labelTitle != nullptr) {
        delete labelTitle;
        labelTitle = nullptr;
    }
    if (labelName != nullptr) {
        delete labelName;
        labelName = nullptr;
    }
    if (labelId != nullptr) {
        delete labelId;
        labelId = nullptr;
    }
    if (buttonSetting != nullptr) {
        delete buttonSetting;
        buttonSetting = nullptr;
    }
}

bool AlipaySetting::AlipayInitButton()
{
    buttonSetting = new UILabelButton();
    if (buttonSetting == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipaySetting::AlipayInitButton new label fail");
        return false;
    }
    buttonSetting->SetPosition(SETTING_BUTTON_X, SETTING_BUTTON_Y, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT);
    buttonSetting->SetText(SETTING_BUTTON);
    buttonSetting->SetAlign(TEXT_ALIGNMENT_CENTER);
    buttonSetting->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_BUTTON_FONT);
    buttonSetting->SetViewId(ALIPAY_UNBIND_BUTTON);
    buttonSetting->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);

    buttonSetting->SetOnClickListener(presenter_);
    Add(buttonSetting);

    return true;
}

bool AlipaySetting::AlipayInitLabel(void)
{
    labelTitle = new UILabel();
    if (labelTitle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipaySetting::AlipayInitLabel new labelTitle fail");
        return false;
    }
    labelTitle->SetText(SETTING_TITLE);
    labelTitle->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_TITLE_FONT);
    labelTitle->SetPosition(SETTING_TITLE_X, SETTING_TITLE_Y, SETTING_TITLE_WIDTH, SETTING_TITLE_HEIGHT);
    labelTitle->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    labelTitle->SetStyle(STYLE_TEXT_FONT, SETTING_TITLE_FONT);
    labelTitle->SetAlign(TEXT_ALIGNMENT_CENTER);

    Add(labelTitle);

    labelName = new UILabel();
    if (labelName == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipaySetting::AlipayInitLabel new labelName fail");
        return false;
    }
    uint8_t str[ALIPAY_GET_NICKNAME_MAX_LENGTH] = {0};
    presenter_->AlipayGetNickName(str);
    labelName->SetText((const char *)str);
    labelName->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_NAME_FONT);
    labelName->SetPosition(SETTING_NAME_X, SETTING_NAME_Y, SETTING_NAME_WIDTH, SETTING_NAME_HEIGHT);
    labelName->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    labelName->SetStyle(STYLE_TEXT_FONT, SETTING_NAME_FONT);
    labelName->SetAlign(TEXT_ALIGNMENT_CENTER);
    labelName->SetRollSpeed(30);
    labelName->SetRollStartPos(SETTING_NAME_X);

    Add(labelName);

    labelId = new UILabel();
    if (labelTitle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipaySetting::AlipayInitLabel new labelId fail");
        return false;
    }
    uint8_t strID[ALIPAY_GET_ID_STR_MAX_LENGTH] = {0};
    presenter_->AlipayGetIdName(strID);
    labelId->SetText((const char *)strID);
    labelId->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ID_FONT);
    labelId->SetPosition(SETTING_ID_X, SETTING_ID_Y, SETTING_ID_WIDTH, SETTING_ID_HEIGHT);
    labelId->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    labelId->SetTextColor(Color::GetColorFromRGBA(0x99, 0x99, 0x99, 0xff));
    labelId->SetStyle(STYLE_TEXT_FONT, SETTING_ID_FONT);
    labelId->SetAlign(TEXT_ALIGNMENT_CENTER);
    labelId->SetRollSpeed(30);
    labelId->SetRollStartPos(SETTING_ID_X);

    Add(labelId);

    return true;
}


bool AlipaySetting::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipaySetting::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipaySetting::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipaySetting::InitView AlipayInitButton() false");
        return false;
    }

    return true;
}
}