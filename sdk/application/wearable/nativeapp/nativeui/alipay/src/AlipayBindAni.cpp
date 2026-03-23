/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bind animate view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "alipay/AlipayBindAni.h"
#include "common/image_cache_manager.h"
#include "graphic_service.h"

namespace OHOS {
static const int16 ANILABEL_X_FIRST = 151;
static const int16 ANILABEL_Y_FIRST = 257;
static const int16 ANILABEL_WIDTH_FIRST = 151;
static const int16 ANILABEL_HEIGHT_FIRST = 48;
static const char *ANILABEL_TEXT_FIRST = (char *)"正在绑定";
static const int16 ANILABEL_FONT_FIRST = 38;

const char *searchImagePath[ALIPAY_BIND_SEARCH_IMAGE_NUM] = {
    PNG_A019_080_IMAGE_PATH,
    PNG_A019_081_IMAGE_PATH,
    PNG_A019_082_IMAGE_PATH,
    PNG_A019_083_IMAGE_PATH,
    PNG_A019_084_IMAGE_PATH,
    PNG_A019_085_IMAGE_PATH,
    PNG_A019_086_IMAGE_PATH,
    PNG_A019_087_IMAGE_PATH,
};

static const int16 ALIPAY_BIND_IMAGE_X = 162;
static const int16 ALIPAY_BIND_IMAGE_Y = 112;
static const int16 ALIPAY_BIND_IMAGE_WIDTH = 128;
static const int16 ALIPAY_BIND_IMAGE_HEIGHT = 128;

static const int16 ALIPAY_BIND_IMAGE_DELAY = 200;

static const int16 RES_IMAGE_X = 162;
static const int16 RES_IMAGE_Y = 112;
static const int16 RES_IMAGE_WIDTH = 128;
static const int16 RES_IMAGE_HEIGHT = 128;

AlipayBindAni::AlipayBindAni(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::AlipayBindAni");
    presenter_ = presenter;
}

AlipayBindAni::~AlipayBindAni()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::~AlipayBindAni");
    RemoveAll();
    if (alipayLabel != nullptr) {
        delete alipayLabel;
        alipayLabel = nullptr;
    }

    if (alipayImageAni != nullptr) {
        delete alipayImageAni;
        alipayImageAni = nullptr;
    }
}

bool AlipayBindAni::AlipayInitLabel(void)
{
    alipayLabel = new UILabel();
    if (alipayLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::InitExit new alipayLabel fail");
        return false;
    }
    alipayLabel->SetPosition(ANILABEL_X_FIRST, ANILABEL_Y_FIRST, ANILABEL_WIDTH_FIRST, ANILABEL_HEIGHT_FIRST);
    alipayLabel->SetText(ANILABEL_TEXT_FIRST);
    alipayLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, ANILABEL_FONT_FIRST);
    alipayLabel->SetStyle(STYLE_TEXT_FONT, ANILABEL_FONT_FIRST);
    alipayLabel->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayLabel->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    Add(alipayLabel);
    return true;
}

bool AlipayBindAni::AlipayInitImageAni(void)
{
    alipayImageAni = new UIImageAnimatorView();
    if (alipayImageAni == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::AlipayInitImageAni new alipayImageAni fail");
        return false;
    }
    for (uint8_t i = 0; i < ALIPAY_BIND_SEARCH_IMAGE_NUM; i++) {
        searchImageInfo[i].imageInfo = ImageCacheManager::GetInstance().LoadSingleRes(searchImagePath[i]);
        searchImageInfo[i].imageType = IMG_SRC_IMAGE_INFO;
        searchImageInfo[i].pos.x = ALIPAY_BIND_IMAGE_X;
        searchImageInfo[i].pos.y = ALIPAY_BIND_IMAGE_Y;
        searchImageInfo[i].width = ALIPAY_BIND_IMAGE_WIDTH;
        searchImageInfo[i].height = ALIPAY_BIND_IMAGE_HEIGHT;
    }

    alipayImageAni->SetPosition(RES_IMAGE_X, RES_IMAGE_Y, RES_IMAGE_WIDTH, RES_IMAGE_HEIGHT);
    alipayImageAni->SetImageAnimatorSrc(searchImageInfo, ALIPAY_BIND_SEARCH_IMAGE_NUM, ALIPAY_BIND_IMAGE_DELAY);
    alipayImageAni->SetRepeat(true);
    alipayImageAni->SetAutoEnable(false);
    alipayImageAni->SetResizeMode(UIImageView::FILL);
    Add(alipayImageAni);

    alipayImageAni->Start();
    return true;
}

bool AlipayBindAni::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::Init");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitImageAni()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayBindAni::InitView AlipayInitImageAni() false");
        return false;
    }

    return true;
}
}