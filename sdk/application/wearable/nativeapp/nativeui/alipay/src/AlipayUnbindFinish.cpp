/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay removie bind finish view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayUnbindFinish.h"

namespace OHOS {
static AlipayUnbindFinish *g_alipayUnbindFinish = nullptr;

static const char *UNBIND_FINISH_TITLE = (char *)"解绑成功";
static const char *UNBIND_FINISH_TEXT =
    (char *)"设备侧已解绑，请到手机支付宝搜索进入“智能设备”小程序查看，若手机侧未解绑，则需要操作解绑";

static const char *UNBIND_FINISH_BUTTON = (char *)"确认";

static const int16 UNBIND_FINISH_TITLE_FONT = 38;
static const int16 UNBIND_FINISH_TITLE_X = 0;
static const int16 UNBIND_FINISH_TITLE_Y = 35;
static const int16 UNBIND_FINISH_TITLE_WIDTH = 454;
static const int16 UNBIND_FINISH_TITLE_HEIGHT = 53;

static const int16 UNBIND_FINISH_TEXT_FONT = 30;
static const int16 UNBIND_FINISH_TEXT_X = 38;
static const int16 UNBIND_FINISH_TEXT_Y = 126;
static const int16 UNBIND_FINISH_TEXT_WIDTH = 380;
static const int16 UNBIND_FINISH_TEXT_HEIGHT = 166;

static const int16 UNBIND_FINISH_BUTTON_FONT = 38;
static const int16 UNBIND_FINISH_BUTTON_X = 120;
static const int16 UNBIND_FINISH_BUTTON_Y = 320;
static const int16 UNBIND_FINISH_BUTTON_WIDTH = 212;
static const int16 UNBIND_FINISH_BUTTON_HEIGHT = 73;

AlipayUnbindFinish::AlipayUnbindFinish(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::AlipayUnbindFinish");
    presenter_ = presenter;
}

AlipayUnbindFinish::~AlipayUnbindFinish()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::~AlipayUnbindFinish");
    RemoveAll();

    if (titleUnbind != nullptr) {
        delete titleUnbind;
        titleUnbind = nullptr;
    }
    if (textUnbind != nullptr) {
        delete textUnbind;
        textUnbind = nullptr;
    }
    if (buttonUnbind != nullptr) {
        delete buttonUnbind;
        buttonUnbind = nullptr;
    }
}

bool AlipayUnbindFinish::AlipayInitLabel(void)
{
    titleUnbind = new UILabel();
    if (titleUnbind == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::AlipayInitLabel new label fail");
        return false;
    }
    titleUnbind->SetText(UNBIND_FINISH_TITLE);
    titleUnbind->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNBIND_FINISH_TITLE_FONT);
    titleUnbind->SetPosition(UNBIND_FINISH_TITLE_X, UNBIND_FINISH_TITLE_Y, UNBIND_FINISH_TITLE_WIDTH,
        UNBIND_FINISH_TITLE_HEIGHT);
    titleUnbind->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    titleUnbind->SetStyle(STYLE_TEXT_FONT, UNBIND_FINISH_TITLE_FONT);
    titleUnbind->SetAlign(TEXT_ALIGNMENT_CENTER);

    Add(titleUnbind);

    titleUnbind = new UILabel();
    if (titleUnbind == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::AlipayInitLabel new label fail");
        return false;
    }
    titleUnbind->SetText(UNBIND_FINISH_TEXT);
    titleUnbind->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNBIND_FINISH_TEXT_FONT);
    titleUnbind->SetPosition(UNBIND_FINISH_TEXT_X, UNBIND_FINISH_TEXT_Y, UNBIND_FINISH_TEXT_WIDTH,
        UNBIND_FINISH_TEXT_HEIGHT);
    titleUnbind->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    titleUnbind->SetStyle(STYLE_TEXT_FONT, UNBIND_FINISH_TEXT_FONT);
    titleUnbind->SetStyle(STYLE_LINE_SPACE, 6);

    Add(titleUnbind);

    return true;
}

bool AlipayUnbindFinish::AlipayInitButton()
{
    buttonUnbind = new UILabelButton();
    if (buttonUnbind == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::AlipayInitButton new label fail");
        return false;
    }
    buttonUnbind->SetPosition(UNBIND_FINISH_BUTTON_X, UNBIND_FINISH_BUTTON_Y, UNBIND_FINISH_BUTTON_WIDTH,
        UNBIND_FINISH_BUTTON_HEIGHT);
    buttonUnbind->SetText(UNBIND_FINISH_BUTTON);
    buttonUnbind->SetAlign(TEXT_ALIGNMENT_CENTER);
    buttonUnbind->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNBIND_FINISH_BUTTON_FONT);
    buttonUnbind->SetViewId(ALIPAY_SETTING_FINISH);
    buttonUnbind->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);
    buttonUnbind->SetOnClickListener(presenter_);
    Add(buttonUnbind);

    return true;
}

bool AlipayUnbindFinish::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbindFinish::InitView AlipayInitButton() false");
        return false;
    }

    return true;
}
}