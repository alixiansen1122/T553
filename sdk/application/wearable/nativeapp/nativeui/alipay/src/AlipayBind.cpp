/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bind view.
 * Author:
 * Create:
 */
#include "UiConfig.h"
#include <string>
#include "alipay/AlipayBind.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const char *BIND_TITLE = (char *)"使用支付宝扫码绑定";
static const char *BIND_BUTTON = (char *)"重新获取";
static const int16 BIND_TITLE_FONT = 30;
static const int16 BIND_BUTTON_FONT = 38;
static const int16 BIND_TITLE_X = 0;
static const int16 BIND_TITLE_Y = 52;
static const int16 BIND_TITLE_WIDTH = 454;
static const int16 BIND_TITLE_HEIGHT = 42;
static const int16 BIND_IMAGE_X = 130;
static const int16 BIND_IMAGE_Y = 126;
static const int16 BIND_IMAGE_WAITING_X = 114;
static const int16 BIND_IMAGE_WAITING_Y = 111;
static const int16 BACKGROUND_IMAGE_X = 135;
static const int16 BACKGROUND_IMAGE_Y = 125;
static const int16 BIND_WIDTH = 80;
static const int16 BIND_DROP_WIDTH = 45;
static const int16 BIND_DROP_X = 200;
static const int16 BIND_DROP_Y = 415;
static const int16 BIND_BUTTON_X = 120;
static const int16 BIND_BUTTON_Y = 320;
static const int16 BIND_BUTTON_WIDTH = 212;
static const int16 BIND_BUTTON_HEIGHT = 73;
static const int16 ALIPAY_IMAGE_MAX = 2;
static const int16 ALIPAY_IMAGE_Y = 35;
static const int16 ALIPAY_COLD_IMAGE_X = 120;
static const int16 ALIPAY_HOT_IMAGE_X = 275;
static const int16 ALIPAY_IMAGE_WIDTH = 60;
static const int16 ALIPAY_IMAGE_HEIGHT = 58;

AlipayBind::AlipayBind(AlipayPresenter *present)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBind::AlipayBind");
    presenter_ = present;
}

AlipayBind::~AlipayBind()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBind::~AlipayBind");
    RemoveAll();
    if (labelBind != nullptr) {
        delete labelBind;
        labelBind = nullptr;
    }
    if (qrcodeGroup != nullptr) {
        delete qrcodeGroup;
        qrcodeGroup = nullptr;
    }
}

bool AlipayBind::AlipayInitImage(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBind::AlipayInitImage");
    qrcodeGroup = new UIQrcode();
    qrcodeGroup->SetPosition(88, 100, 277, 277);
    qrcodeGroup->SetAutoEnable(false);
    qrcodeGroup->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    uint8_t str[ALIPAY_GET_BIND_STR_MAX_LENGTH] = { 0 };
    presenter_->AlipayGetBindCode(str);
    qrcodeGroup->SetQrcodeInfo((const char *)str);
    Add(qrcodeGroup);
    return true;
}

bool AlipayBind::AlipayInitLabel(void)
{
    labelBind = new UILabel();
    if (labelBind == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBind::InitBind new label fail");
        return false;
    }
    labelBind->SetText(BIND_TITLE);
    labelBind->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BIND_TITLE_FONT);
    labelBind->SetPosition(BIND_TITLE_X, BIND_TITLE_Y, BIND_TITLE_WIDTH, BIND_TITLE_HEIGHT);
    labelBind->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    labelBind->SetTextColor(Color::GetColorFromRGBA(0x00, 0x00, 0x00, 0xff));
    labelBind->SetStyle(STYLE_TEXT_FONT, BIND_TITLE_FONT);
    labelBind->SetAlign(TEXT_ALIGNMENT_CENTER);
    Add(labelBind);
    return true;
}


bool AlipayBind::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBind::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBind::InitView AlipayInitLabel() false");
        return false;
    }

    if (!AlipayInitImage()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBind::InitView AlipayInitImage() false");
        return false;
    }

    return true;
}
}