/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans action res view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayTrans/AlipayTransRes.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const int16 EXIT_LABEL_X_FIRST = 38;
static const int16 EXIT_LABEL_Y_FIRST = 179;
static const int16 EXIT_LABEL_WIDTH_FIRST = 378;
static const int16 EXIT_LABEL_HEIGHT_FIRST = 84;

static const int16 EXIT_LABEL_FONT_FIRST = 30;

static const char *EXIT_LABEL_TEXT_1 = (char *)"网络异常，请打开手机运动健康app，并确保设备已连接后再重试";

static const int16 RES_IMAGE_X = 170;
static const int16 RES_IMAGE_Y = 55;
static const int16 RES_IMAGE_WIDTH = 112;
static const int16 RES_IMAGE_HEIGHT = 112;

static const char *RECOGNIZE_BUTTON = (char *)"确定";
static const int16 RECOGNIZE_BUTTON_FONT = 38;
static const int16 RECOGNIZE_BUTTON_X = 50;
static const int16 RECOGNIZE_BUTTON_Y = 312;
static const int16 RECOGNIZE_BUTTON_WIDTH = 169;
static const int16 RECOGNIZE_BUTTON_HEIGHT = 73;

static const char *RECOGNIZE_BUTTON_SEC = (char *)"详情";
static const int16 RECOGNIZE_BUTTON_SEC_FONT = 38;
static const int16 RECOGNIZE_BUTTON_SEC_X = 233;
static const int16 RECOGNIZE_BUTTON_SEC_Y = 312;
static const int16 RECOGNIZE_BUTTON_SEC_WIDTH = 169;
static const int16 RECOGNIZE_BUTTON_SEC_HEIGHT = 73;

typedef struct AlipayTransResInfo {
    const char *str;
    const char *button_left;
    const char *button_right;
} AlipayTransResInfo;
static const AlipayTransResInfo g_AlipayTransResMapper[] = {
    {EXIT_LABEL_TEXT_1, ALIPAY_TRANS_CERTAIN_BACK_LIST,  ALIPAY_TRANS_LIST_RES_DETAIL},
    {EXIT_LABEL_TEXT_1, ALIPAY_TRANS_CERTAIN_TO_TRANS,  ALIPAY_TRANS_CODE_RES_DETAIL},

};

AlipayTransRes::AlipayTransRes(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayTransRes");
    presenter_ = presenter;
}

AlipayTransRes::~AlipayTransRes()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::~AlipayTransRes");
    RemoveAll();

    if (alipayLabel != nullptr) {
        delete alipayLabel;
        alipayLabel = nullptr;
    }

    if (alipayImage != nullptr) {
        delete alipayImage;
        alipayImage = nullptr;
    }

    if (alipayButton != nullptr) {
        delete alipayButton;
        alipayButton = nullptr;
    }
    if (alipayButtonSec != nullptr) {
        delete alipayButtonSec;
        alipayButtonSec = nullptr;
    }
}

void AlipayTransRes::AlipayTransRefreshRes(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayRefreshBindRes");

    if (alipayLabel == nullptr || alipayImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayRefreshBindRes nullptr");
        return;
    }

    return;
}

bool AlipayTransRes::AlipayInitLabel(const char *resStr)
{
    alipayLabel = new UILabel();
    if (alipayLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayInitLabel new alipayLabel fail");
        return false;
    }
    alipayLabel->SetPosition(EXIT_LABEL_X_FIRST, EXIT_LABEL_Y_FIRST, EXIT_LABEL_WIDTH_FIRST, EXIT_LABEL_HEIGHT_FIRST);
    alipayLabel->SetText(resStr);
    alipayLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_FIRST);
    alipayLabel->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_FIRST);
    alipayLabel->SetAlign(TEXT_ALIGNMENT_LEFT);
    alipayLabel->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);

    Add(alipayLabel);

    return true;
}

bool AlipayTransRes::AlipayInitImage(const char *imageSrc)
{
    alipayImage = new UIImageView();
    if (alipayImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayInitImage new tempImage fail");
        return false;
    }
    alipayImage->SetPosition(RES_IMAGE_X, RES_IMAGE_Y, RES_IMAGE_WIDTH, RES_IMAGE_HEIGHT);
    alipayImage->SetSrc(imageSrc);

    // Set additional property
    alipayImage->SetAutoEnable(false);
    alipayImage->SetResizeMode(UIImageView::FILL);

    Add(alipayImage);
    return true;
}

bool AlipayTransRes::AlipayInitButton(uint32_t index)
{
    alipayButton = new UILabelButton();
    if (alipayButton == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayInitButton new alipayButton fail");
        return false;
    }
    alipayButton->SetPosition(RECOGNIZE_BUTTON_X, RECOGNIZE_BUTTON_Y, RECOGNIZE_BUTTON_WIDTH,
        RECOGNIZE_BUTTON_HEIGHT);
    alipayButton->SetText(RECOGNIZE_BUTTON);
    alipayButton->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, RECOGNIZE_BUTTON_FONT);
    alipayButton->SetViewId(g_AlipayTransResMapper[index].button_left);
    alipayButton->SetStyleForState(STYLE_BORDER_COLOR, 0xff4a83bf, UIButton::PRESSED);
    alipayButton->SetOnClickListener(presenter_);
    alipayButton->SetTextColor(Color::GetColorFromRGBA(0x00, 0x00, 0x00, 0xff));
    alipayButton->SetStyle(STYLE_BACKGROUND_COLOR, 0xffffffff);
    Add(alipayButton);

    alipayButtonSec = new UILabelButton();
    if (alipayButtonSec == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::AlipayInitButton new alipayButtonSec fail");
        return false;
    }
    alipayButtonSec->SetPosition(RECOGNIZE_BUTTON_SEC_X, RECOGNIZE_BUTTON_SEC_Y, RECOGNIZE_BUTTON_WIDTH,
        RECOGNIZE_BUTTON_HEIGHT);
    alipayButtonSec->SetText(RECOGNIZE_BUTTON_SEC);
    alipayButtonSec->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayButtonSec->SetFont(DEFAULT_VECTOR_FONT_FILENAME, RECOGNIZE_BUTTON_FONT);
    alipayButtonSec->SetViewId(g_AlipayTransResMapper[index].button_right);
    alipayButtonSec->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);
    alipayButtonSec->SetOnClickListener(presenter_);
    Add(alipayButtonSec);

    return true;
}

bool AlipayTransRes::InitResView(uint32_t index)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::InitExit");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    if (!AlipayInitLabel(g_AlipayTransResMapper[index].str)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitImage(PNG_ALIPAY_BIND_FAILURE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::InitView AlipayInitImage() false");
        return false;
    }

    if (!AlipayInitButton(index)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransRes::InitView AlipayInitButton() false");
        return false;
    }
    return true;
}

}