/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans enable view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayTrans/AlipayTransEnable.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const char *TRANSCODE_TITLE = (char *)"乘车码未开通";
static const int16 TRANSCODE_TITLE_FONT = 30;
static const int16 TRANSCODE_TITLE_X = 136;
static const int16 TRANSCODE_TITLE_Y = 45;
static const int16 TRANSCODE_TITLE_WIDTH = 182;
static const int16 TRANSCODE_TITLE_HEIGHT = 42;

static const char *TRANSCODE_TIPS = (char *)"使用支付宝扫码开通";
static const char *TRANSCODE_URL = (char *)"alipays://platformapi/startapp?appId=20002047&scene=bus";
static const int16 TRANSCODE_TIPS_FONT = 30;
static const int16 TRANSCODE_TIPS_X = 91;
static const int16 TRANSCODE_TIPS_Y = 362;
static const int16 TRANSCODE_TIPS_WIDTH = 270;
static const int16 TRANSCODE_TIPS_HEIGHT = 42;

AlipayTransEnable::AlipayTransEnable(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::AlipayTransEnable");
    presenter_ = presenter;
}

AlipayTransEnable::~AlipayTransEnable()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::~AlipayTransEnable");
    RemoveAll();
    if (labelEnableCode != nullptr) {
        delete labelEnableCode;
        labelEnableCode = nullptr;
    }
    if (qrcodeGroup != nullptr) {
        delete qrcodeGroup;
        qrcodeGroup = nullptr;
    }
    if (labelTips != nullptr) {
        delete labelTips;
        labelTips = nullptr;
    }
}

bool AlipayTransEnable::AlipayInitQrcode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::AlipayInitQrcode");
    qrcodeGroup = new UIQrcode();
    if (qrcodeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::AlipayInitQrcode qrcodeGroup nullptr");
        return false;
    }
    qrcodeGroup->SetPosition(102, 100, 231, 231);
    qrcodeGroup->SetStyle(STYLE_PADDING_LEFT, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_RIGHT, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_TOP, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_BOTTOM, 10);
    qrcodeGroup->SetAutoEnable(false);
    qrcodeGroup->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    qrcodeGroup->SetQrcodeInfo(TRANSCODE_URL);
    qrcodeGroup->SetTouchable(true);
    qrcodeGroup->SetOnClickListener(presenter_);
    qrcodeGroup->SetViewId(ALIPAY_PAY_QRCODE);

    Add(qrcodeGroup);

    return true;
}

bool AlipayTransEnable::AlipayInitLabel(void)
{
    labelEnableCode = new UILabel();
    if (labelEnableCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::AlipayInitLabel new labelEnableCode fail");
        return false;
    }
    labelEnableCode->SetText(TRANSCODE_TITLE);
    labelEnableCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TRANSCODE_TITLE_FONT);
    labelEnableCode->SetPosition(TRANSCODE_TITLE_X, TRANSCODE_TITLE_Y, TRANSCODE_TITLE_WIDTH, TRANSCODE_TITLE_HEIGHT);
    Add(labelEnableCode);

    labelTips = new UILabel();
    if (labelTips == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::AlipayInitLabel new labelTips fail");
        return false;
    }
    labelTips->SetText(TRANSCODE_TIPS);
    labelTips->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TRANSCODE_TIPS_FONT);
    labelTips->SetPosition(TRANSCODE_TIPS_X, TRANSCODE_TIPS_Y, TRANSCODE_TIPS_WIDTH, TRANSCODE_TIPS_HEIGHT);
    Add(labelTips);

    return true;
}

bool AlipayTransEnable::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, 0xff1677ff);
    SetStyle(STYLE_BACKGROUND_OPA, 0xff);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::InitView AlipayInitLabel() false");
        return false;
    }

    if (!AlipayInitQrcode()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransEnable::InitView AlipayInitQrcode() false");
        return false;
    }
    return true;
}
}