/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans tips view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayTrans/AlipayTransTips.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const int16 TIPS_LABEL_X_FIRST = 38;
static const int16 TIPS_LABEL_Y_FIRST = 179;
static const int16 TIPS_LABEL_WIDTH_FIRST = 378;
static const int16 TIPS_LABEL_HEIGHT_FIRST = 84;
static const int16 TIPS_LABEL_FONT_FIRST = 30;

static const char *TIPS_LABEL_TEXT_1 = (char *)"未知错误_具体错误码，请联系设备厂商解决";
static const char *TIPS_LABEL_TEXT_2 = (char *)"开通的乘车码数量太多，清在手机支付宝-出行-管理中，管理后再用";
static const char *TIPS_LABEL_TEXT_3 = (char *)"存在安全风险，12小时内不可用，请切换到手机侧使用";
static const char *TIPS_LABEL_TEXT_4 = (char *)"未知错误_具体错误码，请联系设备厂商解决";
static const char *TIPS_LABEL_TEXT_5 = (char *)"暂不支持，请切换到手机侧使用";

static const int16 RES_IMAGE_X = 170;
static const int16 RES_IMAGE_Y = 55;
static const int16 RES_IMAGE_WIDTH = 112;
static const int16 RES_IMAGE_HEIGHT = 112;

static const char *RECOGNIZE_BUTTON = (char *)"确定";
static const int16 RECOGNIZE_BUTTON_FONT = 38;
static const int16 RECOGNIZE_BUTTON_X = 120;
static const int16 RECOGNIZE_BUTTON_Y = 320;
static const int16 RECOGNIZE_BUTTON_WIDTH = 212;
static const int16 RECOGNIZE_BUTTON_HEIGHT = 73;

typedef struct AlipayTransTipsInfo {
    const char *str;
    const char *button;
} AlipayTransTipsInfo;
static const AlipayTransTipsInfo g_AlipayTransTipsMapper[] = {
    {TIPS_LABEL_TEXT_1, ALIPAY_TRANS_CERTAIN_BACK_LIST},
    {TIPS_LABEL_TEXT_2, ALIPAY_TRANS_CERTAIN_BACK_LIST},
    {TIPS_LABEL_TEXT_3, ALIPAY_TRANS_CERTAIN_TO_TRANS},
    {TIPS_LABEL_TEXT_4, ALIPAY_TRANS_CERTAIN_TO_TRANS},
    {TIPS_LABEL_TEXT_5, ALIPAY_TRANS_CERTAIN_TO_TRANS},
};

AlipayTransTips::AlipayTransTips(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::AlipayTransTips");
    presenter_ = presenter;
}

AlipayTransTips::~AlipayTransTips()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::~AlipayTransTips");
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
}

void AlipayTransTips::AlipayTransRefreshTips(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::AlipayTransRefreshTips");

    if (alipayLabel == nullptr || alipayImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::AlipayTransRefreshTips nullptr");
        return;
    }

    return;
}

bool AlipayTransTips::AlipayInitLabel(const char *resStr)
{
    alipayLabel = new UILabel();
    if (alipayLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::InitExit new alipayLabel fail");
        return false;
    }
    alipayLabel->SetPosition(TIPS_LABEL_X_FIRST, TIPS_LABEL_Y_FIRST, TIPS_LABEL_WIDTH_FIRST, TIPS_LABEL_HEIGHT_FIRST);
    alipayLabel->SetText((const char *)resStr);
    alipayLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TIPS_LABEL_FONT_FIRST);
    alipayLabel->SetStyle(STYLE_TEXT_FONT, TIPS_LABEL_FONT_FIRST);
    alipayLabel->SetAlign(TEXT_ALIGNMENT_LEFT);
    alipayLabel->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);

    Add(alipayLabel);

    return true;
}

bool AlipayTransTips::AlipayInitImage(const char *imageSrc)
{
    alipayImage = new UIImageView();
    if (alipayImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::AlipayInitImage new alipayImage fail");
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

bool AlipayTransTips::AlipayInitButton(uint32_t index)
{
    alipayButton = new UILabelButton();
    if (alipayButton == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::AlipayInitButton new alipayButton fail");
        return false;
    }
    alipayButton->SetPosition(RECOGNIZE_BUTTON_X, RECOGNIZE_BUTTON_Y, RECOGNIZE_BUTTON_WIDTH,
        RECOGNIZE_BUTTON_HEIGHT);
    alipayButton->SetText(RECOGNIZE_BUTTON);
    alipayButton->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, RECOGNIZE_BUTTON_FONT);
    alipayButton->SetViewId(g_AlipayTransTipsMapper[index].button);
    alipayButton->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);
    alipayButton->SetOnClickListener(presenter_);
    Add(alipayButton);

    return true;
}

bool AlipayTransTips::InitTipsView(uint32_t index)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::InitTipsView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    if (!AlipayInitLabel(g_AlipayTransTipsMapper[index].str)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitImage(PNG_ALIPAY_BIND_FAILURE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::InitView AlipayInitImage() false");
        return false;
    }

    if (!AlipayInitButton(index)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransTips::InitView AlipayInitButton() false");
        return false;
    }
    return true;
}
}