/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#include "ui_test_button.h"

#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_radio_button.h"
#include "components/ui_toggle_button.h"
#include "font/ui_font.h"

namespace OHOS {
void UITestButton::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestButton::SubTearDown()
{
    if (checkBoxChangeListener_ != nullptr) {
        delete checkBoxChangeListener_;
        checkBoxChangeListener_ = nullptr;
    }
    if (checkBoxChangeListener1_ != nullptr) {
        delete checkBoxChangeListener1_;
        checkBoxChangeListener1_ = nullptr;
    }
    if (radioChangeListener_ != nullptr) {
        delete radioChangeListener_;
        radioChangeListener_ = nullptr;
    }
    if (radioChangeListener1_ != nullptr) {
        delete radioChangeListener1_;
        radioChangeListener1_ = nullptr;
    }

    if (toggleChangeListener_ != nullptr) {
        delete toggleChangeListener_;
        toggleChangeListener_ = nullptr;
    }
    if (toggleChangeListener1_ != nullptr) {
        delete toggleChangeListener1_;
        toggleChangeListener1_ = nullptr;
    }
    if (clickBigListener_ != nullptr) {
        delete clickBigListener_;
        clickBigListener_ = nullptr;
    }
    if (clickLeftListener_ != nullptr) {
        delete clickLeftListener_;
        clickLeftListener_ = nullptr;
    }
    if (clickRightListener_ != nullptr) {
        delete clickRightListener_;
        clickRightListener_ = nullptr;
    }
    if (clickUpListener_ != nullptr) {
        delete clickUpListener_;
        clickUpListener_ = nullptr;
    }
    if (clickDownListener_ != nullptr) {
        delete clickDownListener_;
        clickDownListener_ = nullptr;
    }
    if (clickSmallListener_ != nullptr) {
        delete clickSmallListener_;
        clickSmallListener_ = nullptr;
    }
}

void UITestButton::TearDown()
{
    SubTearDown();
    DeleteChildren(container_);
    container_ = nullptr;
}

class TestBtnOnStateChangeListener : public OHOS::UICheckBox::OnChangeListener {
public:
    explicit TestBtnOnStateChangeListener(UILabel *uiLabel) : uiLabel_(uiLabel)
    {}

    ~TestBtnOnStateChangeListener()
    {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        if (state == UICheckBox::UICheckBoxState::SELECTED) {
            uiLabel_->SetText("ON");
        } else {
            uiLabel_->SetText("OFF");
        }
        uiLabel_->Invalidate();
        return true;
    }

private:
    UILabel *uiLabel_;
};

const UIView *UITestButton::GetTestView()
{
    if (container_ != nullptr) {
        UILabel *empty = new UILabel();
        container_->Add(empty);
        empty->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 0, Screen::GetInstance().GetWidth(), 100);  // 100 : height
    }
    UIKitCheckBoxTest001();
    UIKitRadioButtonTest001();
    UIKitToggleButtonTest001();
    UIKitCheckBoxTest002();
    UIKitRadioButtonTest002();
    UIKitToggleButtonTest002();
    UIKitButtonTest001();
    return container_;
}

UIViewGroup *UITestButton::CreateButtonGroup(
    int16_t posX, int16_t posY, UICheckBox::OnChangeListener **listener, UIViewType type, const char *name)
{
    UIViewGroup *group = new UIViewGroup();
    group->SetPosition(posX, posY, 300, 150);  // 300: width, 150: height
    UILabel *label = new UILabel();
    group->Add(label);
    label->SetPosition(10, 30, 80, 20);  // 10: posX 30 posY 80 width 20 height
    label->SetText("State: ");

    UILabel *label1 = new UILabel();
    group->Add(label1);
    label1->SetPosition(70, 30, 40, 20);  // 70: posX 30 posY 40 width 20 height
    *listener = static_cast<UICheckBox::OnChangeListener *>(new TestBtnOnStateChangeListener(label1));

    UICheckBox *checkBox = nullptr;
    if (type == UIViewType::UI_TOGGLE_BUTTON) {
        checkBox = static_cast<UICheckBox *>(new UIToggleButton());
    } else if (type == UIViewType::UI_RADIO_BUTTON) {
        checkBox = static_cast<UICheckBox *>(new UIRadioButton(name));
    } else {
        checkBox = new UICheckBox();
        checkBox->SetImages("", "");
    }
    group->Add(checkBox);
    checkBox->SetOnChangeListener(*listener);
    checkBox->SetPosition(100, 0, 100, 100);  // 100: posX 0: posY; 100: width,height
    if (checkBox->GetState() == UICheckBox::SELECTED) {
        label1->SetText("ON");
    } else {
        label1->SetText("OFF");
    }
    return group;
}

void UITestButton::UIKitCheckBoxTest001()
{
    if (container_ != nullptr) {
        UILabel *empty = new UILabel();
        container_->Add(empty);
        empty->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 0, Screen::GetInstance().GetWidth(), 100);  // 100: height

        UILabel *label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE,
            100,  // 100: y-coordinate
            Screen::GetInstance().GetWidth(),
            TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("checkbox功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        // 40: posX 35 posY 100 width 100 height
        UIViewGroup *group = CreateButtonGroup(40, 135, &checkBoxChangeListener_);
        // 250: posX 35 posY 100 width 100 height
        UIViewGroup *group1 = CreateButtonGroup(250, 135, &checkBoxChangeListener1_);
        container_->Add(group);
        container_->Add(group1);
    }
}

void UITestButton::UIKitRadioButtonTest001()
{
    if (container_ != nullptr) {
        UILabel *label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 230,     // 230:y-coordinate
            Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("radiobutton功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        // 40: posX 260 posY 100 width 100 height
        UIViewGroup *group = CreateButtonGroup(40, 260, &radioChangeListener_, UIViewType::UI_RADIO_BUTTON, "bb");
        // 250: posX 260 posY 100 width 100 height
        UIViewGroup *group1 = CreateButtonGroup(250, 260, &radioChangeListener1_, UIViewType::UI_RADIO_BUTTON, "bb");
        container_->Add(group);
        container_->Add(group1);
    }
}

void UITestButton::UIKitToggleButtonTest001()
{
    if (container_ != nullptr) {
        UILabel *label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 370, // 370:y-coordinate
            Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("togglebutton功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        // 40: posX 300 posY 100 width 100 height
        UIViewGroup *group = CreateButtonGroup(40, 400, &toggleChangeListener_, UIViewType::UI_TOGGLE_BUTTON);
        // 250: posX 300 posY 100 width 100 height
        UIViewGroup *group1 = CreateButtonGroup(250, 400, &toggleChangeListener1_, UIViewType::UI_TOGGLE_BUTTON);
        container_->Add(group);
        container_->Add(group1);
    }
}
void UITestButton::UIKitCheckBoxTest002() const
{
    if (container_ != nullptr) {
        UILabel *label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 566, // 566:y-coordinate
            Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("checkbox SetImage功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UICheckBox *checkbox1 = new UICheckBox();
        checkbox1->SetPosition(50, 620);  // 50: x-coordinate, 620: y-coordinate
        UICheckBox *checkbox2 = new UICheckBox();
        checkbox2->SetPosition(100, 620);  // 100: x-coordinate, 620: y-coordinate
        checkbox2->SetWidth(130);          // 130: width
        checkbox2->SetHeight(130);         // 130: height
        checkbox2->SetImages(RES_PATH"blue.png", RES_PATH"yellow.png");

        UICheckBox *checkbox3 = new UICheckBox();
        checkbox3->SetPosition(150, 620);  // 150: x-coordinate, 620: y-coordinate
        checkbox3->SetWidth(130);          // 130: width
        checkbox3->SetHeight(130);         // 130: height
        checkbox3->SetImages(RES_PATH"green.png", RES_PATH"red.png");

        container_->Add(checkbox1);
        container_->Add(checkbox2);
        container_->Add(checkbox3);
    }
}

void UITestButton::UIKitRadioButtonTest002() const
{
    if (container_ != nullptr) {
        UILabel *label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 670, // 670:y-coordinate
            Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("radiobutton SetImage功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIRadioButton *radioButton = new UIRadioButton("aaa");
        radioButton->SetPosition(30, 10);  // 30: x-coordinate, 10: y-coordinate
        radioButton->SetWidth(130);        // 130: width
        radioButton->SetHeight(130);       // 130: height
        radioButton->SetImages(RES_PATH"blue.png", RES_PATH"yellow.png");

        UIRadioButton *radioButton2 = new UIRadioButton("aaa");
        radioButton2->SetPosition(200, 10);  // 200: x-coordinate, 10: y-coordinate
        radioButton2->SetWidth(130);         // 130: width
        radioButton2->SetHeight(130);        // 130: height
        radioButton2->SetImages(RES_PATH"green.png", RES_PATH"red.png");

        OHOS::UIViewGroup *viewGroup = new UIViewGroup();
        viewGroup->SetPosition(0, 700);                               // 0: x-coordinate, 700: y-coordinate
        viewGroup->SetWidth(Screen::GetInstance().GetWidth());        // 2: half width
        viewGroup->SetHeight(Screen::GetInstance().GetHeight() / 2);  // 2: half height

        viewGroup->Add(radioButton);
        viewGroup->Add(radioButton2);
        viewGroup->SetStyle(STYLE_BACKGROUND_OPA, 0);

        container_->Add(viewGroup);
    }
}

void UITestButton::UIKitToggleButtonTest002()
{
    if (container_ != nullptr) {
        UILabel *label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 840, // 840: y-coordinate
            Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("togglebutton SetImage功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIToggleButton *togglebutton = new UIToggleButton();
        togglebutton->SetPosition(30, 870);               // 30: x-coordinate, 870: y-coordinate
        togglebutton->SetStyle(STYLE_BACKGROUND_OPA, 0);  // 0: opacity
        togglebutton->SetState(true);
        togglebutton->SetWidth(130);   // 130: width
        togglebutton->SetHeight(130);  // 130: height
        togglebutton->SetImages(RES_PATH"blue.png", RES_PATH"yellow.png");

        UIToggleButton *togglebutton2 = new UIToggleButton();
        togglebutton2->SetPosition(200, 870);              // 200: x-coordinate, 870: y-coordinate
        togglebutton2->SetStyle(STYLE_BACKGROUND_OPA, 0);  // 0: opacity
        togglebutton2->SetState(false);
        togglebutton2->SetWidth(130);   // 130: width
        togglebutton2->SetHeight(130);  // 130: height
        togglebutton2->SetImages(RES_PATH"green.png", RES_PATH"red.png");

        container_->Add(togglebutton);
        container_->Add(togglebutton2);
    }
}

class TestBtnOnClickShapeChangeListener : public UIView::OnClickListener {
public:
    TestBtnOnClickShapeChangeListener(UIView *uiView, int16_t grid) : uiView_(uiView), changeGrid_(grid)
    {}

    ~TestBtnOnClickShapeChangeListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        int16_t width = uiView_->GetWidth() + changeGrid_;
        int16_t height = uiView_->GetHeight() + changeGrid_;
        if (changeGrid_ < 0) {
            uiView_->Invalidate();
        }
        uiView_->Resize(width, height);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    int16_t changeGrid_;
};

class TestBtnOnClickPositionChangeListener : public OHOS::UIView::OnClickListener {
public:
    enum class MoveType {
        MOVE_LEFT,
        MOVE_RIGHT,
        MOVE_TOP,
        MOVE_BOTTOM,
    };
    TestBtnOnClickPositionChangeListener(UIView *uiView, MoveType moveType, uint16_t grid)
        : uiView_(uiView), moveType_(moveType), grid_(grid)
    {}

    ~TestBtnOnClickPositionChangeListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        int16_t xPos = uiView_->GetX();
        int16_t yPos = uiView_->GetY();
        uiView_->Invalidate();
        if (moveType_ == MoveType::MOVE_LEFT) {
            xPos = xPos - grid_;
        } else if (moveType_ == MoveType::MOVE_RIGHT) {
            xPos = xPos + grid_;
        } else if (moveType_ == MoveType::MOVE_TOP) {
            yPos = yPos - grid_;
        } else if (moveType_ == MoveType::MOVE_BOTTOM) {
            yPos = yPos + grid_;
        }
        uiView_->SetX(xPos);
        uiView_->SetY(yPos);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    MoveType moveType_;
    uint16_t grid_;
};

class TestBtnOnClickVisableListener : public OHOS::UIView::OnClickListener {
public:
    TestBtnOnClickVisableListener(UIView *uiView, bool visible) : uiView_(uiView), visible_(visible)
    {}

    ~TestBtnOnClickVisableListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        uiView_->SetVisible(visible_);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    bool visible_;
};

class TestBtnOnClickTouchableListener : public UIView::OnClickListener {
public:
    TestBtnOnClickTouchableListener(UIView *uiView, bool touchable) : uiView_(uiView), touchable_(touchable)
    {}

    ~TestBtnOnClickTouchableListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        uiView_->SetTouchable(touchable_);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    bool touchable_;
};

class TestBtnOnClickChangeColorListener : public OHOS::UIView::OnClickListener {
public:
    TestBtnOnClickChangeColorListener(UIView *uiView, uint16_t red, uint16_t green, uint16_t blue)
        : uiView_(uiView), red_(red), green_(green), blue_(blue)
    {}

    ~TestBtnOnClickChangeColorListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        ColorType color;
        color.full = uiView_->GetStyle(STYLE_BACKGROUND_COLOR);
        color.red = red_;
        color.green = green_;
        color.blue = blue_;
        uiView_->SetStyle(STYLE_BACKGROUND_COLOR, color.full);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    uint16_t red_;
    uint16_t green_;
    uint16_t blue_;
};

class TestBtnOnClickRevertColorListener : public UIView::OnClickListener {
public:
    explicit TestBtnOnClickRevertColorListener(UIView *uiView) : uiView_(uiView), originColor_(0)
    {
        if (uiView != nullptr) {
            originColor_ = uiView->GetStyle(STYLE_BACKGROUND_COLOR);
        }
    }

    ~TestBtnOnClickRevertColorListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        uiView_->SetStyle(STYLE_BACKGROUND_COLOR, originColor_);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    uint32_t originColor_;
};

class TestBtnOnClickRevertToOriginStateListener : public UIView::OnClickListener {
public:
    explicit TestBtnOnClickRevertToOriginStateListener(UIView *uiView)
        : uiView_(uiView), originColor_(0), width_(0), height_(0), visible_(false), touchable_(false)
    {
        if (uiView != nullptr) {
            originColor_ = uiView->GetStyle(STYLE_BACKGROUND_COLOR);
            width_ = uiView->GetWidth();
            height_ = uiView->GetHeight();
            visible_ = uiView->IsVisible();
            touchable_ = uiView->IsTouchable();
        }
    }

    ~TestBtnOnClickRevertToOriginStateListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        uiView_->Invalidate();
        uiView_->SetStyle(STYLE_BACKGROUND_COLOR, originColor_);
        uiView_->SetWidth(width_);
        uiView_->SetHeight(height_);
        uiView_->SetVisible(visible_);
        uiView_->SetTouchable(touchable_);
        ((UIButton *)uiView_)->Invalidate();
        return true;
    }

private:
    UIView *uiView_;
    uint32_t originColor_;
    uint32_t width_;
    uint32_t height_;
    bool visible_;
    bool touchable_;
};

#if DEFAULT_ANIMATION
class TestBtnAnimationListener : public UIView::OnClickListener {
public:
    TestBtnAnimationListener(UIView *uiView, bool enableAnimation) : uiView_(uiView), enableAnimation_(enableAnimation)
    {}

    ~TestBtnAnimationListener()
    {}

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        static_cast<UIButton *>(uiView_)->EnableButtonAnimation(enableAnimation_);
        return true;
    }

private:
    UIView *uiView_;
    bool enableAnimation_;
};
#endif

UILabel *GetTestUILabel(const char *titlename)
{
    if (titlename == nullptr) {
        return nullptr;
    }

    UILabel *label = new UILabel();
    // 900:y-coordinate
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 900, Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(titlename);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

UILabelButton *GetTestUIButton(const char *buttonText, int16_t x, int16_t y, UIButton *button)
{
    if (buttonText == nullptr) {
        return nullptr;
    }

    UILabelButton *labelButton = new UILabelButton();
    // 150: x-coordinate, 440: y-coordinate
    labelButton->SetPosition(x, y);
    labelButton->Resize(BUTTON_WIDHT1, BUTTON_HEIGHT1);
    labelButton->SetText(buttonText);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    return labelButton;
}

void InitButton(
    UILabelButton *labelButton, UIView::OnClickListener *&listener, UIView::OnClickListener *&targetListener)
{
    if (listener == nullptr) {
        listener = targetListener;
    }
    labelButton->SetOnClickListener(listener);
}

void UITestButton::UIKitButtonTest001()
{
    if (container_ != nullptr) {
        UILabel *label = GetTestUILabel("普通button功能效果");
        container_->Add(label);

        UIButton *button = new UIButton();
        button->SetPosition(10, 1035);  // 10: x-coordinate, 1035: y-coordinate
        button->SetWidth(80);           // 80: width
        button->SetHeight(60);          // 60: height
        button->SetImageSrc(RES_PATH"blue.png", RES_PATH"yellow.png");

        UILabelButton *button2 = GetTestUIButton("放大", 160, 940, button);  // 160: x-coordinate, 940: y-coordinate
        UIView::OnClickListener *l2 = new TestBtnOnClickShapeChangeListener(button, CHANGE_SIZE);
        InitButton(button2, clickBigListener_, l2);

        UILabelButton *button3 = GetTestUIButton("缩小", 160, 990, button);  // 160: x-coordinate, 990: y-coordinate
        UIView::OnClickListener *l3 = new TestBtnOnClickShapeChangeListener(button, -CHANGE_SIZE);
        InitButton(button3, clickSmallListener_, l3);

        UILabelButton *button4 = GetTestUIButton("左移", 250, 940, button);  // 250: x-coordinate, 940: y-coordinate
        UIView::OnClickListener *l4 = new TestBtnOnClickPositionChangeListener(
            (UIView *)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_LEFT, CHANGE_SIZE);
        InitButton(button4, clickLeftListener_, l4);

        UILabelButton *button5 = GetTestUIButton("右移", 250, 990, button);  // 250: x-coordinate, 990: y-coordinate
        UIView::OnClickListener *l5 = new TestBtnOnClickPositionChangeListener(
            (UIView *)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_RIGHT, CHANGE_SIZE);
        InitButton(button5, clickRightListener_, l5);

        UILabelButton *button6 = GetTestUIButton("上移", 340, 940, button);  // 340: x-coordinate, 940: y-coordinate
        UIView::OnClickListener *l6 = new TestBtnOnClickPositionChangeListener(
            (UIView *)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_TOP, CHANGE_SIZE);
        InitButton(button6, clickUpListener_, l6);

        UILabelButton *button7 = GetTestUIButton("下移", 340, 990, button);  // 340: x-coordinate, 990: y-coordinate
        UIView::OnClickListener *l7 = new TestBtnOnClickPositionChangeListener(
            (UIView *)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_BOTTOM, CHANGE_SIZE);
        InitButton(button7, clickDownListener_, l7);

        container_->Add(button);
        container_->Add(button2);
        container_->Add(button3);
        container_->Add(button4);
        container_->Add(button5);
        container_->Add(button6);
        container_->Add(button7);
    }
}
}  // namespace OHOS
