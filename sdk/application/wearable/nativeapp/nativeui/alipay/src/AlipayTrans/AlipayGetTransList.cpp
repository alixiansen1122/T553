/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay get trans list view.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include <string>
#include "cmsis_os.h"
#include "alipay/AlipayTrans/AlipayGetTransList.h"

namespace OHOS {
static const int16 EXIT_LABEL_X_FIRST = 60;
static const int16 EXIT_LABEL_Y_FIRST = 79;
static const int16 EXIT_LABEL_WIDTH_FIRST = 333;
static const int16 EXIT_LABEL_HEIGHT_FIRST = 238;

static const int16 EXIT_LABEL_FONT_FIRST = 30;
static const char *GET_TRANS_LIST_TEXT_1 =
    (char *)"初次使用，完成以下步骤\n1.手机设置-蓝牙，打开并与手表连接配对；\n2.手机“运动健康”确保设备已连接；";
static const char *GET_TRANS_LIST_TEXT_2 =
    (char *)"完成以下步骤,请重试\n1.手机设置-蓝牙，打开并与手表连接配对；\n2.手机“运动健康”确保设备已连接；";
static const int16 EXIT_BUTTON_EXIT_X = 50;
static const int16 EXIT_BUTTON_EXIT_Y = 312;

static const int16 EXIT_BUTTON_BIND_X = 233;
static const int16 EXIT_BUTTON_BIND_Y = 312;

static const char *GET_TRANS_LISTBUTTON_TEXT_LEFT1 = (char *)"退出";
static const char *GET_TRANS_LISTBUTTON_TEXT_RIGHT1 = (char *)"已完成";

static const char *GET_TRANS_LISTBUTTON_TEXT_LEFT2 = (char *)"重试";
static const char *GET_TRANS_LISTBUTTON_TEXT_RIGHT2 = (char *)"详情";

static const int16 EXIT_BUTTON_WIDTH = 169;
static const int16 EXIT_BUTTON_HEIGHT = 73;
static const int16 EXIT_BUTTON_FONT = 38;

typedef struct AlipayTransGetListInfo {
    const char *str;
    const char *button_left;
    const char *button_right;
    const char *label_left;
    const char *label_right;
} AlipayTransGetListInfo;
static const AlipayTransGetListInfo g_AlipayTransGetListMapper[] = {
    {GET_TRANS_LIST_TEXT_1, ALIPAY_TRANS_CERTAIN_BACK_LIST, ALIPAY_TRANS_GET_LIST_FINISH, GET_TRANS_LISTBUTTON_TEXT_LEFT1, GET_TRANS_LISTBUTTON_TEXT_RIGHT1},
    {GET_TRANS_LIST_TEXT_2, ALIPAY_TRANS_GET_LIST_FINISH, ALIPAY_TRANS_GET_LIST_DETAIL, GET_TRANS_LISTBUTTON_TEXT_LEFT2, GET_TRANS_LISTBUTTON_TEXT_RIGHT2},
    {GET_TRANS_LIST_TEXT_2, ALIPAY_TRANS_RETRY_TRANS_CODE, ALIPAY_TRANS_GET_LIST_DETAIL, GET_TRANS_LISTBUTTON_TEXT_LEFT2, GET_TRANS_LISTBUTTON_TEXT_RIGHT2},

};

AlipayGetTransList::AlipayGetTransList(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::AlipayGetTransList");
    presenter_ = presenter;
}

AlipayGetTransList::~AlipayGetTransList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::~AlipayGetTransList");
    RemoveAll();
    int ButtonNum;

    if (alipayLabel != nullptr) {
        delete alipayLabel;
        alipayLabel = nullptr;
    }

    for (ButtonNum = ALIPAY_SET_ZERO; ButtonNum < ALIPAY_SET_TWO; ButtonNum++) {
        if (alipayButton[ButtonNum] != nullptr) {
            delete alipayButton[ButtonNum];
            alipayButton[ButtonNum] = nullptr;
        }
    }
}

bool AlipayGetTransList::AlipayInitLabel(uint32_t index)
{
    alipayLabel = new UILabel();
    if (alipayLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::InitExit new alipayLabel fail");
        return false;
    }

    alipayLabel->SetPosition(EXIT_LABEL_X_FIRST, EXIT_LABEL_Y_FIRST, EXIT_LABEL_WIDTH_FIRST, EXIT_LABEL_HEIGHT_FIRST);
    alipayLabel->SetText(g_AlipayTransGetListMapper[index].str);
    alipayLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, EXIT_LABEL_FONT_FIRST);
    alipayLabel->SetStyle(STYLE_TEXT_FONT, EXIT_LABEL_FONT_FIRST);
    alipayLabel->SetAlign(TEXT_ALIGNMENT_LEFT);
    alipayLabel->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    alipayLabel->SetStyle(STYLE_LINE_SPACE, 6);

    Add(alipayLabel);

    return true;
}

bool AlipayGetTransList::InitSimilarButton(int16 num, int16 alipayX, int16 alipayY, const char *buttonStr,
    const char *viewId)
{
    alipayButton[num] = new UILabelButton();
    if (alipayButton[num] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::InitSimilarButton new alipayButton[%d] fail", num);
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

bool AlipayGetTransList::AlipayInitButton(uint32_t index)
{
    if (!InitSimilarButton(ALIPAY_SET_ZERO, EXIT_BUTTON_EXIT_X, EXIT_BUTTON_EXIT_Y,
        g_AlipayTransGetListMapper[index].label_left, g_AlipayTransGetListMapper[index].button_left)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlipayGetTransList::AlipayInitButton new alipayButton[ALIPAY_SET_ZERO] fail");
        return false;
    }
    if (!InitSimilarButton(ALIPAY_SET_ONE, EXIT_BUTTON_BIND_X, EXIT_BUTTON_BIND_Y,
        g_AlipayTransGetListMapper[index].label_right, g_AlipayTransGetListMapper[index].button_right)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlipayGetTransList::AlipayInitButton new alipayButton[ALIPAY_SET_ONE] fail");
        return false;
    }

    return true;
}

bool AlipayGetTransList::InitGetView(uint32_t index)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::InitGetView index:%u", index);
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    if (!AlipayInitLabel(index)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::InitView AlipayInitLabel false");
        return false;
    }
    if (!AlipayInitButton(index)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayGetTransList::InitView AlipayInitButton false");
        return false;
    }
    return true;
}

}