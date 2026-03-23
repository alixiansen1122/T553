/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay remove bind view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayUnbind.h"

namespace OHOS {
static const int16 EXIT_LABEL_X_FIRST = 132;
static const int16 EXIT_LABEL_Y_FIRST = 57;
static const int16 EXIT_LABEL_WIDTH_FIRST = 189;
static const int16 EXIT_LABEL_HEIGHT_FIRST = 48;

static const int16 EXIT_LABEL_X_SEC = 44;
static const int16 EXIT_LABEL_Y_SECOND = 126;
static const int16 EXIT_LABEL_WIDTH_SEC = 365;
static const int16 EXIT_LABEL_HEIGHT_SEC = 128;

static const int16 EXIT_LABEL_FONT_FIRST = 38;
static const int16 EXIT_LABEL_FONT_SEC = 30;

static const char *UNBIND_LABEL_TEXT_FIRST = (char *)"解除绑定";
static const char *UNBIND_LABEL_TEXT_SECOND = (char *)"解绑后将无法使用支付宝各项解锁功能，确认解绑？";

static const int16 EXIT_BUTTON_EXIT_X = 50;
static const int16 EXIT_BUTTON_EXIT_Y = 312;

static const int16 EXIT_BUTTON_BIND_X = 233;
static const int16 EXIT_BUTTON_BIND_Y = 312;

static const char *UNBIND_BUTTON_TEXT_FIRST = (char *)"确认";
static const char *UNBIND_BUTTON_TEXT_SECOND = (char *)"取消";

static const int16 EXIT_BUTTON_WIDTH = 169;
static const int16 EXIT_BUTTON_HEIGHT = 73;
static const int16 EXIT_BUTTON_FONT = 38;

AlipayUnbind::AlipayUnbind(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::AlipayUnbind");
    presenter_ = presenter;
}

AlipayUnbind::~AlipayUnbind()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::~AlipayUnbind");
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

bool AlipayUnbind::AlipayInitLabel()
{
    alipayLabel[ALIPAY_SET_ZERO] = new UILabel();
    if (alipayLabel[ALIPAY_SET_ZERO] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::InitExit new alipayLabel[ALIPAY_SET_ZERO] fail");
        return false;
    }

    alipayLabel[ALIPAY_SET_ZERO]->SetPosition(EXIT_LABEL_X_FIRST, EXIT_LABEL_Y_FIRST, EXIT_LABEL_WIDTH_FIRST,
        EXIT_LABEL_HEIGHT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetText(UNBIND_LABEL_TEXT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_FIRST);
    alipayLabel[ALIPAY_SET_ZERO]->SetAlign(TEXT_ALIGNMENT_CENTER);
    alipayLabel[ALIPAY_SET_ZERO]->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);

    Add(alipayLabel[ALIPAY_SET_ZERO]);

    alipayLabel[ALIPAY_SET_ONE] = new UILabel();
    if (alipayLabel[ALIPAY_SET_ONE] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::InitExit new alipayLabel[ALIPAY_SET_ONE] fail");
        return false;
    }
    alipayLabel[ALIPAY_SET_ONE]->SetPosition(EXIT_LABEL_X_SEC, EXIT_LABEL_Y_SECOND, EXIT_LABEL_WIDTH_SEC,
        EXIT_LABEL_HEIGHT_SEC);
    alipayLabel[ALIPAY_SET_ONE]->SetText(UNBIND_LABEL_TEXT_SECOND);
    alipayLabel[ALIPAY_SET_ONE]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_SEC);
    alipayLabel[ALIPAY_SET_ONE]->SetStyle(STYLE_LINE_SPACE, 12);
    alipayLabel[ALIPAY_SET_ONE]->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_SEC);
    alipayLabel[ALIPAY_SET_ONE]->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    Add(alipayLabel[ALIPAY_SET_ONE]);

    return true;
}

bool AlipayUnbind::InitSimilarButton(int16 num, int16 alipayX, int16 alipayY, const char *buttonStr, const char *viewId)
{
    alipayButton[num] = new UILabelButton();
    if (alipayButton[num] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::InitSimilarButton new alipayButton[%d] fail", num);
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

bool AlipayUnbind::AlipayInitButton()
{
    if (!InitSimilarButton(ALIPAY_SET_ZERO, EXIT_BUTTON_EXIT_X, EXIT_BUTTON_EXIT_Y, UNBIND_BUTTON_TEXT_FIRST,
        ALIPAY_SETTING_UNBIND)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::AlipayInitButton new alipayButton[ALIPAY_SET_ZERO] fail");
        return false;
    }
    if (!InitSimilarButton(ALIPAY_SET_ONE, EXIT_BUTTON_BIND_X, EXIT_BUTTON_BIND_Y, UNBIND_BUTTON_TEXT_SECOND,
        ALIPAY_SETTING_CANCEL)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::AlipayInitButton new alipayButton[ALIPAY_SET_ONE] fail");
        return false;
    }

    return true;
}

bool AlipayUnbind::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::InitView");
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    
    if (!AlipayInitLabel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::InitView AlipayInitLabel() false");
        return false;
    }
    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayUnbind::InitView AlipayInitButton() false");
        return false;
    }
    return true;
}
}