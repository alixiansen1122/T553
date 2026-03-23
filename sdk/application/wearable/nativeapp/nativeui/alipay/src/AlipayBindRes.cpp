/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bind result view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayBindRes.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
static const int16 EXIT_LABEL_X_FIRST = 151;
static const int16 EXIT_LABEL_Y_FIRST = 257;
static const int16 EXIT_LABEL_WIDTH_FIRST = 151;
static const int16 EXIT_LABEL_HEIGHT_FIRST = 48;
static const char *EXIT_LABEL_TEXT_FIRST = (char *)"绑定失败";
static const int16 EXIT_LABEL_FONT_FIRST = 38;

static const char *EXIT_LABEL_TEXT_SEC = (char *)"绑定成功";

static const int16 RES_IMAGE_X = 161;
static const int16 RES_IMAGE_Y = 110;
static const int16 RES_IMAGE_WIDTH = 135;
static const int16 RES_IMAGE_HEIGHT = 135;

AlipayBindRes::AlipayBindRes(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::AlipayBindRes");
    presenter_ = presenter;
}

AlipayBindRes::~AlipayBindRes()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::~AlipayBindRes");
    RemoveAll();

    if (alipayLabel != nullptr) {
        delete alipayLabel;
        alipayLabel = nullptr;
    }

    if (alipayImage != nullptr) {
        delete alipayImage;
        alipayImage = nullptr;
    }
}

void AlipayBindRes::AlipayRefreshBindRes(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::AlipayRefreshBindRes");

    if (alipayLabel == nullptr || alipayImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::AlipayRefreshBindRes nullptr");
        return;
    }
    // alipayLabel->Set
    if (presenter_->AlipayGetBindFlag()) {
        alipayLabel->SetText(EXIT_LABEL_TEXT_SEC);
        alipayImage->SetSrc(PNG_ALIPAY_BIND_SUCCESS);
    } else {
        alipayLabel->SetText(EXIT_LABEL_TEXT_FIRST);
        alipayImage->SetSrc(PNG_ALIPAY_BIND_FAILURE);
    }
    alipayLabel->Invalidate();
    alipayImage->Invalidate();

    return;
}

bool AlipayBindRes::AlipayInitLabel(const char *resStr)
{
    alipayLabel = new UILabel();
    if (alipayLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::InitExit new alipayLabel fail");
        return false;
    }
    alipayLabel->SetPosition(EXIT_LABEL_X_FIRST, EXIT_LABEL_Y_FIRST, EXIT_LABEL_WIDTH_FIRST, EXIT_LABEL_HEIGHT_FIRST);
    alipayLabel->SetText(resStr);
    alipayLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_FIRST);
    alipayLabel->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_FIRST);
    alipayLabel->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayLabel->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);

    Add(alipayLabel);

    return true;
}

bool AlipayBindRes::AlipayInitImage(const char *imageSrc)
{
    alipayImage = new UIImageView();
    if (alipayImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::AlipayInitImage new alipayImage fail");
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

bool AlipayBindRes::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::InitExit");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    const char *resStr = EXIT_LABEL_TEXT_SEC;
    if (!AlipayInitLabel(resStr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitImage(PNG_ALIPAY_BIND_SUCCESS)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindRes::InitView AlipayInitImage() false");
        return false;
    }
    return true;
}
}