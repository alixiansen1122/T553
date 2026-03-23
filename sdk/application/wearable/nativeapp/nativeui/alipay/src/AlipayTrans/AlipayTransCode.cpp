/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay transcode view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayTrans/AlipayTransCode.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const char *TRANSCODE_BUTTON = (char *)"切换交通卡";
static const int16 TRANSCODE_TITLE_FONT = 30;
static const int16 TRANSCODE_BUTTON_FONT = 35;
static const int16 TRANSCODE_TITLE_X = 120;
static const int16 TRANSCODE_TITLE_Y = 24;
static const int16 TRANSCODE_TITLE_WIDTH = 212;
static const int16 TRANSCODE_TITLE_HEIGHT = 42;

static const int16 TRANSCODE_BUTTON_X = 139;
static const int16 TRANSCODE_BUTTON_Y = 386;
static const int16 TRANSCODE_BUTTON_WIDTH = 176;
static const int16 TRANSCODE_BUTTON_HEIGHT = 50;

AlipayTransCode::AlipayTransCode(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::AlipayTransCode");
    presenter_ = presenter;
}

AlipayTransCode::~AlipayTransCode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::~AlipayTransCode");
    RemoveAll();
    if (labelPayCode != nullptr) {
        delete labelPayCode;
        labelPayCode = nullptr;
    }
    if (buttonPayCode != nullptr) {
        delete buttonPayCode;
        buttonPayCode = nullptr;
    }
    if (qrcodeGroup != nullptr) {
        delete qrcodeGroup;
        qrcodeGroup = nullptr;
    }
}

bool AlipayTransCode::AlipayInitQrcode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::AlipayInitQrcode");

    qrcodeGroup = new UIQrcode();
    if (qrcodeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::AlipayInitQrcode qrcodeGroup nullptr");
        return false;
    }
    qrcodeGroup->SetPosition(70, 70, 298, 298);
    qrcodeGroup->SetECCLevel(QrCode::Ecc::LOW);
    qrcodeGroup->SetStyle(STYLE_PADDING_LEFT, 8);
    qrcodeGroup->SetStyle(STYLE_PADDING_RIGHT, 8);
    qrcodeGroup->SetStyle(STYLE_PADDING_TOP, 8);
    qrcodeGroup->SetStyle(STYLE_PADDING_BOTTOM, 8);
    qrcodeGroup->SetAutoEnable(false);
    qrcodeGroup->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    qrcodeGroup->SetQrcodeInfo(presenter_->AlipayGetTransCode(), presenter_->AlipayGetTransCodeLen());

    Add(qrcodeGroup);

    return true;
}

bool AlipayTransCode::AlipayInitLabel(void)
{
    labelPayCode = new UILabel();
    if (labelPayCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::AlipayInitLabel new label fail");
        return false;
    }
    labelPayCode->SetText((const char *)presenter_->AlipayGetCardTitle());
    labelPayCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TRANSCODE_TITLE_FONT);
    labelPayCode->SetPosition(TRANSCODE_TITLE_X, TRANSCODE_TITLE_Y, TRANSCODE_TITLE_WIDTH, TRANSCODE_TITLE_HEIGHT);
    labelPayCode->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    labelPayCode->SetAlign(TEXT_ALIGNMENT_CENTER);
    labelPayCode->SetRollSpeed(30);
    labelPayCode->SetRollStartPos(TRANSCODE_TITLE_X);
    Add(labelPayCode);

    return true;
}

bool AlipayTransCode::AlipayInitButton()
{
    buttonPayCode = new UILabelButton();
    if (buttonPayCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::AlipayInitButton new button fail");
        return false;
    }
    buttonPayCode->SetPosition(TRANSCODE_BUTTON_X, TRANSCODE_BUTTON_Y, TRANSCODE_BUTTON_WIDTH, TRANSCODE_BUTTON_HEIGHT);
    buttonPayCode->SetText(TRANSCODE_BUTTON);
    buttonPayCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TRANSCODE_BUTTON_FONT);
    buttonPayCode->SetViewId(ALIPAY_TRANS_CODE_BACK_LIST);

    buttonPayCode->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);
    buttonPayCode->SetOnClickListener(presenter_);
    Add(buttonPayCode);

    return true;
}

bool AlipayTransCode::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, 0xff1677ff);
    SetStyle(STYLE_BACKGROUND_OPA, 0xff);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::InitView AlipayInitLabel() false");
        return false;
    }

    if (!AlipayInitQrcode()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::InitView AlipayInitQrcode() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransCode::InitView AlipayInitButton() false");
        return false;
    }
    return true;
}
}