/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay pay view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayPayCode.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static AlipayPayCode *g_alipayPayCode = nullptr;

static const char *PAYCODE_TITLE = (char *)"付款码";
static const char *PAYCODE_BUTTON = (char *)"切换条形码";
static const int16 PAYCODE_TITLE_FONT = 38;
static const int16 PAYCODE_BUTTON_FONT = 35;
static const int16 PAYCODE_TITLE_X = 170;
static const int16 PAYCODE_TITLE_Y = 23;
static const int16 PAYCODE_TITLE_WIDTH = 114;
static const int16 PAYCODE_TITLE_HEIGHT = 53;

static const int16 PAYCODE_BUTTON_X = 139;
static const int16 PAYCODE_BUTTON_Y = 380;
static const int16 PAYCODE_BUTTON_WIDTH = 176;
static const int16 PAYCODE_BUTTON_HEIGHT = 50;

static const int16 QRCODE_BUTTON_X = 139;
static const int16 QRCODE_BUTTON_Y = 382;
static const int16 QRCODE_BUTTON_WIDTH = 176;
static const int16 QRCODE_BUTTON_HEIGHT = 50;

static const int16 PAYCODE_IMAGE_X = 76;
static const int16 PAYCODE_IMAGE_Y = 76;
static const int16 PAYCODE_IMAGE_WAITING_X = 302;
static const int16 PAYCODE_IMAGE_WAITING_Y = 302;

static const int16 PAYCODE_WIDTH = 80;
static const int16 PAYCODE_DROP_WIDTH = 45;
static const int16 PAYCODE_DROP_X = 200;
static const int16 PAYCODE_DROP_Y = 415;

static const int16 RES_IMAGE_X_GREY = 232;
static const int16 RES_IMAGE_X_WHITE = 207;
static const int16 RES_IMAGE_Y = 13;
static const int16 RES_IMAGE_WIDTH = 15;
static const int16 RES_IMAGE_HEIGHT = 15;

AlipayPayCode::AlipayPayCode(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayPayCode");
    g_alipayPayCode = this;
    presenter_ = presenter;
}

AlipayPayCode::~AlipayPayCode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::~AlipayPayCode");
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
    if (alipayImageGrey != nullptr) {
        delete alipayImageGrey;
        alipayImageGrey = nullptr;
    }
    if (alipayImageWhite != nullptr) {
        delete alipayImageWhite;
        alipayImageWhite = nullptr;
    }
    g_alipayPayCode = nullptr;
}

AlipayPayCode *AlipayPayCode::GetInstance(void)
{
    return g_alipayPayCode;
}

bool AlipayPayCode::AlipayInitImage(void)
{
    alipayImageGrey = new UIImageView();
    if (alipayImageGrey == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayInitImage new tempImage fail");
        return false;
    }
    alipayImageGrey->SetPosition(RES_IMAGE_X_GREY, RES_IMAGE_Y, RES_IMAGE_WIDTH, RES_IMAGE_HEIGHT);
    alipayImageGrey->SetSrc(PNG_ALIPAY_PAY_ICON_GREY);

    // Set additional property
    alipayImageGrey->SetAutoEnable(false);
    alipayImageGrey->SetResizeMode(UIImageView::FILL);

    Add(alipayImageGrey);

    alipayImageWhite = new UIImageView();
    if (alipayImageWhite == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayInitImage new tempImage fail");
        return false;
    }
    alipayImageWhite->SetPosition(RES_IMAGE_X_WHITE, RES_IMAGE_Y, RES_IMAGE_WIDTH, RES_IMAGE_HEIGHT);
    alipayImageWhite->SetSrc(PNG_ALIPAY_PAY_ICON_WHITE);

    // Set additional property
    alipayImageWhite->SetAutoEnable(false);
    alipayImageWhite->SetResizeMode(UIImageView::FILL);

    Add(alipayImageWhite);
    return true;
}

bool AlipayPayCode::AlipayInitQrcode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayInitQrcode");
    qrcodeGroup = new UIQrcode();
    if (qrcodeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayInitQrcode qrcodeGroup nullptr");
        return false;
    }
    qrcodeGroup->SetPosition(76, 76, 282, 282);
    qrcodeGroup->SetStyle(STYLE_PADDING_LEFT, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_RIGHT, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_TOP, 10);
    qrcodeGroup->SetStyle(STYLE_PADDING_BOTTOM, 10);
    qrcodeGroup->SetAutoEnable(false);
    qrcodeGroup->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    uint8_t str[ALIPAY_GET_PAY_STR_MAX_LENGTH] = {0};
    presenter_->AlipayGetPayCode(str);
    qrcodeGroup->SetECCLevel(QrCode::Ecc::QUARTILE);
    qrcodeGroup->SetQrcodeInfo((const char *)str);
    qrcodeGroup->SetTouchable(true);
    qrcodeGroup->SetOnClickListener(presenter_);
    qrcodeGroup->SetViewId(ALIPAY_PAY_QRCODE);

    Add(qrcodeGroup);

    return true;
}

void AlipayPayCode::AlipayRefreshQrcode(const char *refreshStr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayRefreshQrcode");

    if (qrcodeGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayRefreshQrcode qrcodeGroup nullptr");
        return;
    }
    qrcodeGroup->SetECCLevel(QrCode::Ecc::QUARTILE);
    qrcodeGroup->SetQrcodeInfo(refreshStr);

    return;
}

bool AlipayPayCode::AlipayInitLabel(void)
{
    labelPayCode = new UILabel();
    if (labelPayCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayInitLabel new label fail");
        return false;
    }
    labelPayCode->SetText(PAYCODE_TITLE);
    labelPayCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, PAYCODE_TITLE_FONT);
    labelPayCode->SetPosition(PAYCODE_TITLE_X, PAYCODE_TITLE_Y, PAYCODE_TITLE_WIDTH, PAYCODE_TITLE_HEIGHT);
    Add(labelPayCode);

    return true;
}


bool AlipayPayCode::AlipayInitButton()
{
    buttonPayCode = new UILabelButton();
    if (buttonPayCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::AlipayInitButton new labelbutton fail");
        return false;
    }
    buttonPayCode->SetPosition(PAYCODE_BUTTON_X, PAYCODE_BUTTON_Y, PAYCODE_BUTTON_WIDTH, PAYCODE_BUTTON_HEIGHT);
    buttonPayCode->SetText(PAYCODE_BUTTON);
    buttonPayCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, PAYCODE_BUTTON_FONT);
    buttonPayCode->SetViewId(ALIPAY_PAY_SWITCH_BAR);
    buttonPayCode->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);
    buttonPayCode->SetOnClickListener(presenter_);
    Add(buttonPayCode);

    return true;
}

bool AlipayPayCode::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, 0xff1677ff);
    SetStyle(STYLE_BACKGROUND_OPA, 0xff);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitImage()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::InitView AlipayInitImage() false");
        return false;
    }

    if (!AlipayInitQrcode()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::InitView AlipayInitQrcode() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPayCode::InitView AlipayInitButton() false");
        return false;
    }
    return true;
}
}