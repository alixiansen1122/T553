/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_list.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "dock/focus_manager.h"
#include "font/ui_font.h"
namespace OHOS {
namespace {
    static int16_t g_listW = 100;
    static int16_t g_listH = 200;
    static int16_t g_selectPos = 150;
    static int16_t g_blankSize = 250;
    static int16_t g_width = 80;
    static int16_t g_height = 50;
}

static int16_t g_xCoord = TEXT_DISTANCE_TO_LEFT_SIDE;
static int16_t g_yCoord = 0;
void UITestList::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetThrowDrag(true);
    }
    if (adapterData1_ == nullptr) {
        adapterData1_ = new List<const char*>();
        adapterData1_->PushBack("abcd0");
        adapterData1_->PushBack("abcd1");
        adapterData1_->PushBack("abcd2");
        adapterData1_->PushBack("abcd3");
    }
    if (adapterData2_ == nullptr) {
        adapterData2_ = new List<const char*>();
        adapterData2_->PushBack("000");
        adapterData2_->PushBack("111");
        adapterData2_->PushBack("222");
        adapterData2_->PushBack("333");
        adapterData2_->PushBack("444");
        adapterData2_->PushBack("555");
        adapterData2_->PushBack("666");
        adapterData2_->PushBack("777");
        adapterData2_->PushBack("888");
        adapterData2_->PushBack("999");
    }
}

void UITestList::TearDown()
{
    FocusManager::GetInstance()->ClearFocus();
    DeleteChildren(container_);
    container_ = nullptr;
    if (adapterData1_ != nullptr) {
        adapterData1_->Clear();
        delete adapterData1_;
        adapterData1_ = nullptr;
    }
    if (adapterData2_ != nullptr) {
        adapterData2_->Clear();
        delete adapterData2_;
        adapterData2_ = nullptr;
    }
    if (adapter1_ != nullptr) {
        delete adapter1_;
        adapter1_ = nullptr;
    }
    if (adapter2_ != nullptr) {
        delete adapter2_;
        adapter2_ = nullptr;
    }
    if (adapter3_ != nullptr) {
        delete adapter3_;
        adapter3_ = nullptr;
    }
    if (adapter4_ != nullptr) {
        delete adapter4_;
        adapter4_ = nullptr;
    }
    setBlankBtn_ = nullptr;
    setBlankOffBtn_ = nullptr;
    setThrowDragBtn_ = nullptr;
    setThrowDragOffBtn_ = nullptr;
    scrollStateLabel_ = nullptr;
    scrollSelectLabel_ = nullptr;
    setRefreshBtn_ = nullptr;
    setLoopBtn_ = nullptr;
    setLoopOffBtn_ = nullptr;
    setSelectBtn_ = nullptr;
    setSelectOffBtn_ = nullptr;
    setAutoAlignBtn_ = nullptr;
    setAutoAlignOffBtn_ = nullptr;
    g_xCoord = TEXT_DISTANCE_TO_LEFT_SIDE;
    g_yCoord = VIEW_DISTANCE_TO_TOP_SIDE;
}

const UIView* UITestList::GetTestView()
{
    UiKitListInitTestFullScreen001();
    UiKitListInitTestHalhScreen001();
    UiKitListScrollTestBlankSet001();
    return container_;
}

void UITestList::UiKitListInitTestFullScreen001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst adapter data full");
    if (label == nullptr) {
        return;
    }
    container_->Add(label);
    label->SetPosition(g_xCoord, g_yCoord);
    g_yCoord += TITLE_LABEL_DEFAULT_HEIGHT;
    if (adapter1_ == nullptr) {
        adapter1_ = new TextAdapter();
    }
    adapter1_->SetData(adapterData2_);

    UIList* list = new UIList(UIList::VERTICAL);
    FocusManager::GetInstance()->RequestFocus(list);
    list->SetIntercept(true);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    list->SetPosition(g_xCoord, g_yCoord, g_listW, g_listH);
    g_yCoord += g_listH;
    list->SetStartIndex(5); // 5: list start index
    list->GetStartIndex();
    list->SetAdapter(adapter1_);
    container_->Add(list);
}

void UITestList::UiKitListInitTestHalhScreen001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst adapter data half");
    if (label == nullptr) {
        return;
    }
    container_->Add(label);
    // 2: half of screen width
    label->SetPosition(g_xCoord, g_yCoord);
    g_yCoord += TITLE_LABEL_DEFAULT_HEIGHT;

    if (adapter2_ == nullptr) {
        adapter2_ = new TextAdapter();
    }
    adapter2_->SetData(adapterData1_);

    UIList* list = new UIList(UIList::VERTICAL);
    list->SetIntercept(true);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    // 2: half of screen width
    list->SetPosition(g_xCoord, g_yCoord, g_listW, g_listH);
    g_yCoord += g_listH;
    list->SetAdapter(adapter2_);
    container_->Add(list);
}

void UITestList::SetControlButton()
{
    if (setBlankBtn_ == nullptr) {
        setBlankBtn_ = new UILabelButton();
    }
    if (setBlankOffBtn_ == nullptr) {
        setBlankOffBtn_ = new UILabelButton();
    }
    if (setThrowDragBtn_ == nullptr) {
        setThrowDragBtn_ = new UILabelButton();
    }
    if (setThrowDragOffBtn_ == nullptr) {
        setThrowDragOffBtn_ = new UILabelButton();
    }
    if (setRefreshBtn_ == nullptr) {
        setRefreshBtn_ = new UILabelButton();
    }
    if (setLoopBtn_ == nullptr) {
        setLoopBtn_ = new UILabelButton();
    }
    if (setLoopOffBtn_ == nullptr) {
        setLoopOffBtn_ = new UILabelButton();
    }
    if (setSelectBtn_ == nullptr) {
        setSelectBtn_ = new UILabelButton();
    }
    if (setSelectOffBtn_ == nullptr) {
        setSelectOffBtn_ = new UILabelButton();
    }
    if (setAutoAlignBtn_ == nullptr) {
        setAutoAlignBtn_ = new UILabelButton();
    }
    if (setAutoAlignOffBtn_ == nullptr) {
        setAutoAlignOffBtn_ = new UILabelButton();
    }
    positionX_ += 10; // 10: X increment
    positionY_ += 20; // 20: Y increment
    SetUpButton(setBlankBtn_, "blank on", BUTTON_WIDHT2);
    SetUpButton(setBlankOffBtn_, "blank off", BUTTON_WIDHT2);
    SetUpButton(setThrowDragBtn_, "throwDrag on", BUTTON_WIDHT2);
    SetUpButton(setThrowDragOffBtn_, "throwDrag off", BUTTON_WIDHT2);
    SetUpButton(setRefreshBtn_, "refresh list", BUTTON_WIDHT2);
    SetUpButton(setLoopBtn_, "loop on", BUTTON_WIDHT2);
    SetUpButton(setLoopOffBtn_, "loop off", BUTTON_WIDHT2);
    positionX_ += 120; // 120: X increment
    positionY_ = setBlankBtn_->GetY();
    SetUpButton(setSelectBtn_, "select 150", BUTTON_WIDHT3);
    SetUpButton(setSelectOffBtn_, "select off", BUTTON_WIDHT3);
    SetUpButton(setAutoAlignBtn_, "autoAlign on", BUTTON_WIDHT3);
    SetUpButton(setAutoAlignOffBtn_, "autoAlign Off", BUTTON_WIDHT3);
}

void UITestList::UiKitListScrollTestBlankSet001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst set blank and throwDrag");
    if (label == nullptr) {
        return;
    }
    container_->Add(label);
    label->SetPosition(g_xCoord, g_yCoord);
    g_yCoord += TITLE_LABEL_DEFAULT_HEIGHT;

    if (adapter4_ == nullptr) {
        adapter4_ = new TextAdapter();
    }
    adapter4_->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    adapter4_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    adapter4_->SetHeight(100); // 100: height
    adapter4_->SetWidth(100); // 100: width
    adapter4_->SetData(adapterData2_);

    UIList* list = new UIList(UIList::VERTICAL);
    list->SetIntercept(true);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    list->SetPosition(g_xCoord, g_yCoord, g_listW, g_listH + 200); // 200: y offset
    g_yCoord += g_listH;
    list->SetAdapter(adapter4_);
    list->SetScrollStateListener(this);
    currentList_ = list;

    if (scrollStateLabel_ == nullptr) {
        scrollStateLabel_ = new UILabel();
    }
    if (scrollSelectLabel_ == nullptr) {
        scrollSelectLabel_ = new UILabel();
    }

    scrollStateLabel_->SetPosition(list->GetWidth() + 65, list->GetY(), g_width, g_height); // 65: offset

    scrollSelectLabel_->SetPosition(list->GetWidth() + g_width + 70, list->GetY(), g_width, g_height); // 70: offset
    if (list->GetSelectView() != nullptr) {
        scrollSelectLabel_->SetText(static_cast<UILabel*>(list->GetSelectView())->GetText());
    }

    positionX_ = list->GetWidth() + list->GetX();
    positionY_ = scrollStateLabel_->GetY() + scrollStateLabel_->GetHeight();
    scrollStateLabel_->SetText("STOP");
    scrollSelectLabel_->SetText("NULL");
    SetControlButton();

    container_->Add(list);
    UIView* selectBtnPos = new UIView();
    selectBtnPos->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    selectBtnPos->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    selectBtnPos->SetStyle(STYLE_BORDER_WIDTH, 0);
    selectBtnPos->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, list->GetY() + g_selectPos, 100, 1); // 100: width
    container_->Add(selectBtnPos);
    container_->Add(scrollStateLabel_);
    container_->Add(scrollSelectLabel_);
}

bool UITestList::OnClick(UIView& view, const ClickEvent& event)
{
    if (currentList_ == nullptr) {
        return true;
    }
    if (&view == setBlankBtn_) {
        currentList_->SetScrollBlankSize(g_blankSize);
    } else if (&view == setBlankOffBtn_) {
        currentList_->SetScrollBlankSize(0);
    } else if (&view == setThrowDragBtn_) {
        currentList_->SetThrowDrag(true);
    } else if (&view == setThrowDragOffBtn_) {
        currentList_->SetThrowDrag(false);
    } else if (&view == setRefreshBtn_) {
        adapter4_->SetData(adapterData1_);
        currentList_->RefreshList();
    } else if (&view == setLoopBtn_) {
        currentList_->SetLoopState(true);
    } else if (&view == setLoopOffBtn_) {
        currentList_->SetLoopState(false);
    } else if (&view == setSelectBtn_) {
        currentList_->SetSelectPosition(g_selectPos);
    } else if (&view == setSelectOffBtn_) {
        currentList_->SetSelectPosition(0);
    } else if (&view == setAutoAlignBtn_) {
        currentList_->EnableAutoAlign(true);
    } else if (&view == setAutoAlignOffBtn_) {
        currentList_->EnableAutoAlign(false);
    }
    return true;
}

void UITestList::OnScrollStart(int16_t index, UIView* view)
{
    if (GetScrollState() == SCROLL_STATE_STOP) {
        scrollStateLabel_->SetText("STOP");
    } else {
        scrollStateLabel_->SetText("MOVE");
    }
    scrollStateLabel_->Invalidate();
}

void UITestList::OnScrollEnd(int16_t index, UIView* view)
{
    if (GetScrollState() == SCROLL_STATE_STOP) {
        scrollStateLabel_->SetText("STOP");
    } else {
        scrollStateLabel_->SetText("MOVE");
    }
    scrollStateLabel_->Invalidate();
}

void UITestList::OnItemSelected(int16_t index, UIView* view)
{
    if (view != nullptr) {
        scrollSelectLabel_->SetText(static_cast<UILabel*>(view)->GetText());
        view->Invalidate();
    } else {
        scrollSelectLabel_->SetText("NULL");
    }
}

void UITestList::SetUpButton(UILabelButton* btn, const char* title, int16_t width)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(positionX_, positionY_, width, 80); // 80: button height
    positionY_ += btn->GetHeight() + 10; // 10: button interval
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    container_->Invalidate();
}
} // namespace OHOS