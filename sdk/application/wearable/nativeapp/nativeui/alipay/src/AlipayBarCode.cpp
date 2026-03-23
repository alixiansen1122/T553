/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay barcode view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayBarCode.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
#define ALIPAY_BARCODE_SPACE 32
#define ALIPAY_BARCODE_SPACE_INTER 4
#define ALIPAY_BARCODE_SPACE_NUM 3
#define ALIPAY_BARCODE_LABEL_NUM 23

static AlipayBarCode *g_alipayBarCode = nullptr;

static const char *BARCODE_TITLE = (char *)"付款码";
static const char *BARCODE_BUTTON = (char *)"切换二维码";

static const int16 BARCODE_TITLE_FONT = 38;
static const int16 BARCODE_BUTTON_FONT = 35;

static const int16 BARCODE_TITLE_X = 170;
static const int16 BARCODE_TITLE_Y = 23;
static const int16 BARCODE_TITLE_WIDTH = 114;
static const int16 BARCODE_TITLE_HEIGHT = 53;

static const int16 BARCODE_BUTTON_X = 139;
static const int16 BARCODE_BUTTON_Y = 380;
static const int16 BARCODE_BUTTON_WIDTH = 176;
static const int16 BARCODE_BUTTON_HEIGHT = 50;

static const int16 RES_IMAGE_X_GREY = 207;
static const int16 RES_IMAGE_X_WHITE = 232;
static const int16 RES_IMAGE_Y = 13;
static const int16 RES_IMAGE_WIDTH = 15;
static const int16 RES_IMAGE_HEIGHT = 15;

AlipayBarCode::AlipayBarCode(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayBarCode");
    g_alipayBarCode = this;
    presenter_ = presenter;
}

AlipayBarCode::~AlipayBarCode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::~AlipayBarCode");
    RemoveAll();

    if (labelBarCode != nullptr) {
        delete labelBarCode;
        labelBarCode = nullptr;
    }
    if (buttonBarCode != nullptr) {
        delete buttonBarCode;
        buttonBarCode = nullptr;
    }
    if (alipayImageGrey != nullptr) {
        delete alipayImageGrey;
        alipayImageGrey = nullptr;
    }
    if (alipayImageWhite != nullptr) {
        delete alipayImageWhite;
        alipayImageWhite = nullptr;
    }
    if (alipayBarcode != nullptr) {
        delete alipayBarcode;
        alipayBarcode = nullptr;
    }
    if (labelPay != nullptr) {
        delete labelPay;
        labelPay = nullptr;
    }
    g_alipayBarCode = nullptr;
}

AlipayBarCode *AlipayBarCode::GetInstance(void)
{
    return g_alipayBarCode;
}

bool AlipayBarCode::AlipayInitImage(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayInitImage");

    alipayImageGrey = new UIImageView();
    if (alipayImageGrey == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayInitImage new Grey fail");
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayInitImage new White fail");
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

bool AlipayBarCode::AlipayInitLabel(void)
{
    labelBarCode = new UILabel();
    if (labelBarCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayInitLabel new label fail");
        return false;
    }
    labelBarCode->SetText(BARCODE_TITLE);
    labelBarCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BARCODE_TITLE_FONT);
    labelBarCode->SetPosition(BARCODE_TITLE_X, BARCODE_TITLE_Y, BARCODE_TITLE_WIDTH, BARCODE_TITLE_HEIGHT);
    Add(labelBarCode);

    return true;
}

void AlipayBarCode::AlipayBarcodeReformat(uint8_t *str, uint8_t *strBarcode)
{
    for (uint8_t i = 0, j = 0; i < ALIPAY_GET_PAY_STR_MAX_LENGTH && j < ALIPAY_BARCODE_LABEL_NUM; i++, j++) {
        if ((i % ALIPAY_BARCODE_SPACE_INTER == 0) && i != 0 &&
            i <= ALIPAY_BARCODE_SPACE_INTER * ALIPAY_BARCODE_SPACE_NUM) {
            strBarcode[j] = ALIPAY_BARCODE_SPACE;
            j++;
        }
        strBarcode[j] = str[i];
    }
    return;
}

bool AlipayBarCode::AlipayInitBarcode()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayInitBarcode");
    alipayBarcode = new UIBarcode();
    if (alipayBarcode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayInitBarcode new UIBarcode fail");
        return false;
    }
    alipayBarcode->SetPosition(43, 159, 348, 124);
    alipayBarcode->SetStyle(STYLE_PADDING_TOP, 6);
    alipayBarcode->SetStyle(STYLE_PADDING_BOTTOM, 6);
    alipayBarcode->SetStyle(STYLE_PADDING_LEFT, 10);
    alipayBarcode->SetStyle(STYLE_PADDING_RIGHT, 10);
    alipayBarcode->EnableStretch(true);
    uint8_t str[ALIPAY_GET_PAY_STR_MAX_LENGTH] = {0};
    presenter_->AlipayGetPayCode(str);
    alipayBarcode->SetBarcodeInfo((const char *)str);
    alipayBarcode->SetTouchable(true);
    alipayBarcode->SetOnClickListener(presenter_);
    alipayBarcode->SetViewId(ALIPAY_PAY_BARCODE);

    Add(alipayBarcode);

    labelPay = new UILabel();
    if (labelPay == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitBarCode new labelPay fail");
        return false;
    }
    uint8_t strBarcode[ALIPAY_BARCODE_LABEL_NUM] = {0};
    AlipayBarcodeReformat(str, strBarcode);
    labelPay->SetText((char *)strBarcode);
    labelPay->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    labelPay->SetAlign(TEXT_ALIGNMENT_CENTER);
    labelPay->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30);
    labelPay->SetPosition(36, 297, 381, 42);
    Add(labelPay);
    return true;
}

void AlipayBarCode::AlipayRefreshBarcode(const char *refreshStr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayRefreshBarcode");

    if (alipayBarcode == nullptr || labelPay == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::AlipayRefreshBarcode alipayBarcode or labelPay nullptr");
        return;
    }
    alipayBarcode->SetBarcodeInfo(refreshStr);
    uint8_t strBarcode[ALIPAY_BARCODE_LABEL_NUM] = {0};
    AlipayBarcodeReformat((uint8_t *)refreshStr, strBarcode);
    labelPay->SetText((char *)strBarcode);
    labelPay->Invalidate();

    return;
}

bool AlipayBarCode::AlipayInitButton()
{
    buttonBarCode = new UILabelButton();
    if (buttonBarCode == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitBarCode new label fail");
        return false;
    }
    buttonBarCode->SetPosition(BARCODE_BUTTON_X, BARCODE_BUTTON_Y, BARCODE_BUTTON_WIDTH, BARCODE_BUTTON_HEIGHT);
    buttonBarCode->SetText(BARCODE_BUTTON);
    buttonBarCode->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BARCODE_BUTTON_FONT);
    buttonBarCode->SetViewId(ALIPAY_PAY_SWITCH_QR);
    buttonBarCode->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);
    buttonBarCode->SetOnClickListener(presenter_);
    Add(buttonBarCode);

    return true;
}

bool AlipayBarCode::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, 0xff1677ff);
    SetStyle(STYLE_BACKGROUND_OPA, 0xff);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitView AlipayInitButton() false");
        return false;
    }

    if (!AlipayInitBarcode()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitView AlipayInitQrcode() false");
        return false;
    }

    if (!AlipayInitImage()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBarCode::InitView AlipayInitImage() false");
        return false;
    }
    return true;
}
}