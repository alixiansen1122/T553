/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay binding process exit binding view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "alipay/AlipayExitBind.h"

namespace OHOS {
static const int16 EXIT_LABEL_X_FIRST = 132;
static const int16 EXIT_LABEL_X_SEC = 0;
static const int16 EXIT_LABEL_WIDTH_FIRST = 189;
static const int16 EXIT_LABEL_HEIGHT_FIRST = 48;

static const int16 EXIT_LABEL_Y_FIRST = 57;
static const int16 EXIT_LABEL_Y_SECOND = 154;
static const int16 EXIT_LABEL_WIDTH_SEC = 454;
static const int16 EXIT_LABEL_HEIGHT_SEC = 83;
static const int16 EXIT_LABEL_FONT_FIRST = 38;
static const int16 EXIT_LABEL_FONT_SEC = 30;

static const char *EXIT_LABEL_TEXT_FIRST = (char *)"未完成绑定";
static const char *EXIT_LABEL_TEXT_SECOND = (char *)"当前未完成绑定，退出在进来\n需要重新扫码绑定";

static const int16 EXIT_BUTTON_EXIT_X = 50;
static const int16 EXIT_BUTTON_EXIT_Y = 312;

static const int16 EXIT_BUTTON_BIND_X = 233;
static const int16 EXIT_BUTTON_BIND_Y = 312;

static const char *EXIT_BUTTON_TEXT_FIRST = (char *)"退出";
static const char *EXIT_BUTTON_TEXT_SECOND = (char *)"去绑定";

static const int16 EXIT_BUTTON_WIDTH = 169;
static const int16 EXIT_BUTTON_HEIGHT = 73;
static const int16 EXIT_BUTTON_FONT = 38;

AlipayExitBind::AlipayExitBind(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::AlipayExitBind");
    presenter_ = presenter;
}

AlipayExitBind::~AlipayExitBind()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::~AlipayExitBind");
    RemoveAll();
    for (int32_t labelNum = ALIPAY_SET_ZERO; labelNum < ALIPAY_SET_TWO; labelNum++) {
        if (alipayLabel[labelNum] != nullptr) {
            delete alipayLabel[labelNum];
            alipayLabel[labelNum] = nullptr;
        }
    }
    for (int32_t buttonNum = ALIPAY_SET_ZERO; buttonNum < ALIPAY_SET_TWO; buttonNum++) {
        if (alipayButton[buttonNum] != nullptr) {
            delete alipayButton[buttonNum];
            alipayButton[buttonNum] = nullptr;
        }
    }
}

bool AlipayExitBind::AlipayInitLabel()
{
    alipayLabel[ALIPAY_SET_ZERO] = new UILabel();
    if (alipayLabel[ALIPAY_SET_ZERO] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::InitExit new alipayLabel[ALIPAY_SET_ZERO] fail");
        return false;
    }

    alipayLabel[ALIPAY_SET_ZERO]->SetPosition(EXIT_LABEL_X_FIRST, EXIT_LABEL_Y_FIRST, EXIT_LABEL_WIDTH_FIRST,
        EXIT_LABEL_HEIGHT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetText(EXIT_LABEL_TEXT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayLabel[ALIPAY_SET_ZERO]->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);

    Add(alipayLabel[ALIPAY_SET_ZERO]);

    alipayLabel[ALIPAY_SET_ONE] = new UILabel();
    if (alipayLabel[ALIPAY_SET_ONE] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::InitExit new alipayLabel[ALIPAY_SET_ONE] fail");
        return false;
    }
    alipayLabel[ALIPAY_SET_ONE]->SetPosition(EXIT_LABEL_X_SEC, EXIT_LABEL_Y_SECOND, EXIT_LABEL_WIDTH_SEC,
        EXIT_LABEL_HEIGHT_SEC);
    alipayLabel[ALIPAY_SET_ONE]->SetText(EXIT_LABEL_TEXT_SECOND);
    alipayLabel[ALIPAY_SET_ONE]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_SEC);
    alipayLabel[ALIPAY_SET_ONE]->SetStyle(STYLE_LINE_SPACE, 6);
    alipayLabel[ALIPAY_SET_ONE]->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_SEC);
    alipayLabel[ALIPAY_SET_ONE]->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayLabel[ALIPAY_SET_ONE]->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    Add(alipayLabel[ALIPAY_SET_ONE]);

    return true;
}

bool AlipayExitBind::InitSimilarButton(int16 num, int16 alipayX, int16 alipayY, const char *buttonStr,
    const char *viewId)
{
    alipayButton[num] = new UILabelButton();
    if (alipayButton[num] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::InitSimilarButton new alipayButton[%d] fail", num);
        return false;
    }
    int64_t backColor = 0;

    if (num == 0) {
        backColor = 0xff4a83bf;
        alipayButton[num]->SetTextColor(Color::GetColorFromRGBA(0x00, 0x00, 0x00, 0xff));
        alipayButton[num]->SetStyle(STYLE_BACKGROUND_COLOR, 0xffffffff);
    } else if (num == 1) {
        backColor = 0xff0b1928;
    }
    alipayButton[num]->SetPosition(alipayX, alipayY, EXIT_BUTTON_WIDTH, EXIT_BUTTON_HEIGHT);
    alipayButton[num]->SetText(buttonStr);
    alipayButton[num]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_BUTTON_FONT);
    alipayButton[num]->SetViewId(viewId);
    alipayButton[num]->SetStyleForState(STYLE_BORDER_COLOR, backColor, UIButton::RELEASED);
    alipayButton[num]->SetOnClickListener(presenter_);

    Add(alipayButton[num]);

    return true;
}

bool AlipayExitBind::AlipayInitButton()
{
    if (!InitSimilarButton(ALIPAY_SET_ZERO, EXIT_BUTTON_EXIT_X, EXIT_BUTTON_EXIT_Y, EXIT_BUTTON_TEXT_FIRST,
        ALIPAY_INTER_EXIT)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlipayExitBind::AlipayInitButton new alipayButton[ALIPAY_SET_ZERO] fail");
        return false;
    }
    if (!InitSimilarButton(ALIPAY_SET_ONE, EXIT_BUTTON_BIND_X, EXIT_BUTTON_BIND_Y, EXIT_BUTTON_TEXT_SECOND,
        ALIPAY_BIND_CONTINUE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlipayExitBind::AlipayInitButton new alipayButton[ALIPAY_SET_ONE] fail");
        return false;
    }

    return true;
}

bool AlipayExitBind::InitView()
{
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayExitBind::InitView AlipayInitButton() false");
        return false;
    }
    return true;
}
}