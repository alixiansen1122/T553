/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MonkeyPage
 * Create: 2025-04-24
 */
#include <iostream>
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "monkey/MonkeyPresenter.h"
#include "monkey/MonkeyModel.h"
#include "monkey/MonkeyPage.h"

namespace OHOS {
static constexpr uint16_t MONKEY_LABEL_POS_X = 137;
static constexpr uint16_t MONKEY_LABEL_POS_Y = 66;
static constexpr uint16_t MONKEY_LABEL_WIDTH = 200;
static constexpr uint16_t MONKEY_LABEL_HEIGHT = 90;
static constexpr uint16_t START_MONKEY_BUTTON_POS_X = 142;
static constexpr uint16_t START_MONKEY_BUTTON_POS_Y = 165;
static constexpr uint16_t START_MONKEY_BUTTON_WIDTH = 182;
static constexpr uint16_t START_MONKEY_BUTTON_HEIGHT = 47;
static constexpr uint16_t END_MONKEY_BUTTON_POS_X = 142;
static constexpr uint16_t END_MONKEY_BUTTON_POS_Y = 247;
static constexpr uint16_t END_MONKEY_BUTTON_WIDTH = 182;
static constexpr uint16_t END_MONKEY_BUTTON_HEIGHT = 47;
static constexpr uint16_t MONKEY_FONT_SIZE = 34;
static constexpr char *START_MONKEY = "startMonkey";
static constexpr char *END_MONKEY = "endMonkey";

REGIST_SLICE_PAGE(VIEW_MONKEY, MONKEY_PAGES::MONKEY_MAIN_PAGE, MonkeyPage, true);

void MonkeyPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new container_ fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetStyle(STYLE_BACKGROUND_OPA, 0);

    monkeyLabel_ = new UILabel();
    if (monkeyLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new monkeyLabel_ fail");
        return;
    }
    monkeyLabel_->SetPosition(MONKEY_LABEL_POS_X, MONKEY_LABEL_POS_Y, MONKEY_LABEL_WIDTH, MONKEY_LABEL_HEIGHT);
    monkeyLabel_->SetViewId("label1");
    monkeyLabel_->SetStyle(STYLE_TEXT_FONT, MONKEY_FONT_SIZE);
    monkeyLabel_->SetText("Monkey测试");
    monkeyLabel_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    monkeyLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    monkeyLabel_->SetRollSpeed(0);
    monkeyLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MONKEY_FONT_SIZE);
    container_->Add(monkeyLabel_);
    monkeyLabel_->SetVisible(true);

    endMonkeyButton_ = new UILabelButtonExt();
    if (endMonkeyButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new endMonkeyButton_ fail");
        return;
    }
    endMonkeyButton_->SetPosition(END_MONKEY_BUTTON_POS_X, END_MONKEY_BUTTON_POS_Y, END_MONKEY_BUTTON_WIDTH,
                                  END_MONKEY_BUTTON_HEIGHT);
    endMonkeyButton_->SetText("结束测试");
    endMonkeyButton_->SetViewId(END_MONKEY);
    endMonkeyButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MONKEY_FONT_SIZE);
    endMonkeyButton_->SetStyleForState(STYLE_BORDER_COLOR, Color::Blue().full, UIButton::PRESSED);
    container_->Add(endMonkeyButton_);
    endMonkeyButton_->SetVisible(true);

    startMonkeyButton_ = new UILabelButtonExt();
    if (startMonkeyButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new startMonkeyButton_ fail");
        return;
    }
    startMonkeyButton_->SetPosition(START_MONKEY_BUTTON_POS_X, START_MONKEY_BUTTON_POS_Y, START_MONKEY_BUTTON_WIDTH,
                                    START_MONKEY_BUTTON_HEIGHT);
    startMonkeyButton_->SetText("开始测试");
    startMonkeyButton_->SetViewId(START_MONKEY);
    startMonkeyButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MONKEY_FONT_SIZE);
    startMonkeyButton_->SetStyleForState(STYLE_BORDER_COLOR, Color::Blue().full, UIButton::PRESSED);
    container_->Add(startMonkeyButton_);
    startMonkeyButton_->SetVisible(true);

    endMonkeyButton_->SetOnClickListener(this);
    startMonkeyButton_->SetOnClickListener(this);
    container_->SetOnDragListener(this);
    AddViewToPageContainer(container_);
}

MonkeyPage::MonkeyPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MonkeyPage::MonkeyPage");
}

MonkeyPage::~MonkeyPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (monkeyLabel_ != nullptr) {
        delete monkeyLabel_;
        monkeyLabel_ = nullptr;
    }
    if (endMonkeyButton_ != nullptr) {
        delete endMonkeyButton_;
        endMonkeyButton_ = nullptr;
    }
    if (startMonkeyButton_ != nullptr) {
        delete startMonkeyButton_;
        startMonkeyButton_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MonkeyPage::~MonkeyPage");
}

bool MonkeyPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        MonkeyModel::GetInstance().SetStart(false);
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return false;
}

bool MonkeyPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), START_MONKEY) == 0) {
        MonkeyModel::GetInstance().SetStart(true);
    } else if (strcmp(view.GetViewId(), END_MONKEY) == 0) {
        MonkeyModel::GetInstance().SetStart(false);
    }
    return false;
}
}