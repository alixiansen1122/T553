/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay help qrcode view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayHelp.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const char *HELP_TITLE = (char*)"使用支付宝扫码求助";
static const int16 HELP_TITLE_FONT = 30;
static const int16 HELP_TITLE_X = 0;
static const int16 HELP_TITLE_Y = 52;
static const int16 HELP_TITLE_WIDTH = 454;
static const int16 HELP_TITLE_HEIGHT = 42;

static const int16 ALIPAY_IMAGE_MAX = 2;
static const int16 ALIPAY_IMAGE_Y = 35;
static const int16 ALIPAY_IMAGE_WIDTH = 60;
static const int16 ALIPAY_IMAGE_HEIGHT = 58;

AlipayHelp::AlipayHelp(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayHelp::AlipayHelp");
    presenter_ = presenter;
}

AlipayHelp::~AlipayHelp()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayHelp::~AlipayHelp");
    RemoveAll();

    if (labelHelp != nullptr) {
        delete labelHelp;
        labelHelp = nullptr;
    }
    if (qrcodeGroup != nullptr) {
        delete qrcodeGroup;
        qrcodeGroup = nullptr;
    }
}

bool AlipayHelp::AlipayInitImage(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayHelp::AlipayInitImage");

    qrcodeGroup = new UIQrcode();
    if (qrcodeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayHelp::AlipayInitImage qrcodeGroup nullptr");
        return false;
    }
    qrcodeGroup->SetPosition(78, 96, 278, 278);
    qrcodeGroup->SetStyle(STYLE_PADDING_LEFT, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_RIGHT, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_TOP, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_BOTTOM, 10);
    qrcodeGroup->SetAutoEnable(false);
    qrcodeGroup->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    int8_t strAid[ALIPAY_GET_AID_STR_MAX_LENGTH] = { 0 };
    presenter_->AlipayGetAidCode(strAid);
    qrcodeGroup->SetQrcodeInfo((const char *)strAid);

    Add(qrcodeGroup);

    return true;
}


bool AlipayHelp::AlipayInitLabel(void)
{
    labelHelp = new UILabel();
    if (labelHelp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayHelp::AlipayInitLabel new labelHelp fail");
        return false;
    }
    labelHelp->SetText(HELP_TITLE);
    labelHelp->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HELP_TITLE_FONT);
    labelHelp->SetPosition(HELP_TITLE_X, HELP_TITLE_Y, HELP_TITLE_WIDTH, HELP_TITLE_HEIGHT);
    labelHelp->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    labelHelp->SetStyle(STYLE_TEXT_FONT, HELP_TITLE_FONT);
    labelHelp->SetAlign(TEXT_ALIGNMENT_CENTER);
    
    Add(labelHelp);

    return true;
}


bool AlipayHelp::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayHelp::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, 0xff1677ff);
    SetStyle(STYLE_BACKGROUND_OPA, 0xff);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayHelp::InitView AlipayInitImage() false");
        return false;
    }
    
    if (!AlipayInitImage()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayHelp::InitView AlipayInitImage() false");
        return false;
    }
    
    return true;
}
}