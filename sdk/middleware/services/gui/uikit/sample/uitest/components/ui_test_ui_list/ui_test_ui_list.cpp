/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_ui_list.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
    static int16_t g_buttonH = 80;
    static int16_t g_buttonW = 200;
    static int16_t g_blank = 20;
    static int16_t g_listW = 200;
    static int16_t g_listH = 400;
    static int16_t g_selectPos = 150;
    static int16_t g_blankSize = 250;
}

void UITestUIList::SetUp()
{
    lastX_ = 0;
    lastY_ = 0;
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
    }
    if (uiViewGroup1_ == nullptr) {
        uiViewGroup1_ = new UIViewGroup();
        uiViewGroup1_->SetPosition(0, 20, Screen::GetInstance().GetWidth(), 250); // 20: y pos; 250: height
        container_->Add(uiViewGroup1_);
    }
    if (uiViewGroup2_ == nullptr) {
        uiViewGroup2_ = new UIViewGroup();
        uiViewGroup2_->SetPosition(0, 270, Screen::GetInstance().GetWidth(), 390); // 270: y pos; 390: height
        container_->Add(uiViewGroup2_);
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

void UITestUIList::TearDown()
{
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
    setBackAlignBtn_ = nullptr;
    setBackAlignOffBtn_ = nullptr;
    setAutoAlignACCIncBtn_ = nullptr;
    setAutoAlignACCDncBtn_ = nullptr;
    setYScrollBarVisableBtn_ = nullptr;
    setYScrollBarInvisableBtn_ = nullptr;
    uiViewGroup1_ = nullptr;
    uiViewGroup2_ = nullptr;
    scroll_ = nullptr;
}

const UIView* UITestUIList::GetTestView()
{
    UIKitListInitTestFullScreen001();
    UIKitListInitTesthalhScreen001();
    UIKitListScrollTestBlankSet001();
    return container_;
}

void UITestUIList::UIKitListInitTestFullScreen001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("adapter数据填满list");
    uiViewGroup1_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);

    if (adapter1_ == nullptr) {
        adapter1_ = new TextAdapter();
    }
    adapter1_->SetData(adapterData2_);

    UIList* list = new UIList(UIList::VERTICAL);
    list->SetIntercept(true);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE,
        g_listW - 30, g_listH - 200);   // 200: mean list reduce height , 30 offset
    list->SetStartIndex(5); // 5: list start index
    list->GetStartIndex();
    list->SetAdapter(adapter1_);
    list->SetYScrollBarVisible(true);
    uiViewGroup1_->Add(list);
}

void UITestUIList::UIKitListInitTesthalhScreen001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("adapter数据不填满list");
    uiViewGroup1_->Add(label);
    // 2: half of screen width
    label->SetPosition(Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);

    if (adapter2_ == nullptr) {
        adapter2_ = new TextAdapter();
    }
    adapter2_->SetData(adapterData1_);

    UIList* list = new UIList(UIList::VERTICAL);
    list->SetIntercept(true);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    // 2: half of screen width , 30 offset
    list->SetPosition(Screen::GetInstance().GetWidth() / 2 + VIEW_DISTANCE_TO_LEFT_SIDE - 30, VIEW_DISTANCE_TO_TOP_SIDE,
                      g_listW - 30, g_listH - 150); // 30 offset , 150: decrease height
    list->SetAdapter(adapter2_);
    list->SetYScrollBarVisible(true);
    uiViewGroup1_->Add(list);
}

void UITestUIList::SetControlButton()
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
    if (setBackAlignBtn_ == nullptr) {
        setBackAlignBtn_ = new UILabelButton();
    }
    if (setBackAlignOffBtn_ == nullptr) {
        setBackAlignOffBtn_ = new UILabelButton();
    }
    if (setAutoAlignACCIncBtn_ == nullptr) {
        setAutoAlignACCIncBtn_ = new UILabelButton();
    }
    if (setAutoAlignACCDncBtn_ == nullptr) {
        setAutoAlignACCDncBtn_ = new UILabelButton();
    }
    if (setYScrollBarVisableBtn_ == nullptr) {
        setYScrollBarVisableBtn_ = new UILabelButton();
    }
    if (setYScrollBarInvisableBtn_ == nullptr) {
        setYScrollBarInvisableBtn_ = new UILabelButton();
    }
    positionX_ += 5; // 5: increase y-coordinate
    positionY_ = 0;
    SetUpButton(setBlankBtn_, "开启blank");
    SetUpButton(setBlankOffBtn_, "关闭blank");
    SetUpButton(setThrowDragBtn_, "开启throwDrag");
    SetUpButton(setThrowDragOffBtn_, "关闭throwDrag");
    SetUpButton(setRefreshBtn_, "刷新list");
    SetUpButton(setLoopBtn_, "开启loop模式");
    SetUpButton(setLoopOffBtn_, "关闭loop模式");
    SetUpButton(setSelectBtn_, "select 150");
    SetUpButton(setSelectOffBtn_, "关闭select");
    SetUpButton(setAutoAlignBtn_, "开启自动对齐 ");
    SetUpButton(setAutoAlignOffBtn_, "关闭自动对齐 ");
    SetUpButton(setBackAlignBtn_, "开启弹回对齐 ");
    SetUpButton(setBackAlignOffBtn_, "关闭弹回对齐 ");
    SetUpButton(setAutoAlignACCIncBtn_, "增加自动对齐时间 ");
    SetUpButton(setAutoAlignACCDncBtn_, "减少自动对齐时间 ");
    SetUpButton(setYScrollBarVisableBtn_, "显示纵向滚动条");
    SetUpButton(setYScrollBarInvisableBtn_, "不显示纵向滚动条");
}

void UITestUIList::UIKitListScrollTestBlankSet001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst设置blank和throwDrag");
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 10); // 10: decrease y-coordinate
    uiViewGroup2_->Add(label);

    if (adapter4_ == nullptr) {
        adapter4_ = new TextAdapter();
    }
    adapter4_->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    adapter4_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    adapter4_->SetHeight(100);   // 100: mean adapter height
    adapter4_->SetWidth(100);   // 100: mean adapter width
    adapter4_->SetData(adapterData2_);

    UIList* list = new UIList(UIList::VERTICAL);
    list->SetIntercept(true);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, 60, g_listW, g_listH); // 60:  y-coordinate
    list->SetYScrollBarVisible(true);
    list->SetAdapter(adapter4_);
    list->SetScrollStateListener(this);
    currentList_ = list;

    if (scrollStateLabel_ == nullptr) {
        scrollStateLabel_ = new UILabel();
    }
    if (scrollSelectLabel_ == nullptr) {
        scrollSelectLabel_ = new UILabel();
    }

    scrollStateLabel_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    scrollSelectLabel_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    scrollStateLabel_->SetText("STOP");
    scrollSelectLabel_->SetText("NULL");

    scrollStateLabel_->SetPosition(list->GetWidth() + 53, list->GetY()); // 53: increase x-coordinate
    // 53: increase x-coordinate
    scrollSelectLabel_->SetPosition(list->GetWidth() + scrollStateLabel_->GetWidth() + g_blank + 53, list->GetY());
    if (list->GetSelectView() != nullptr) {
        scrollSelectLabel_->SetText(static_cast<UILabel*>(list->GetSelectView())->GetText());
    }
    positionX_ = list->GetWidth();
    positionY_ = scrollStateLabel_->GetY() + scrollStateLabel_->GetHeight();

    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        scroll_->SetPosition(g_listW + TEXT_DISTANCE_TO_LEFT_SIDE,
            TEXT_DISTANCE_TO_TOP_SIDE + 100,    // 100 : offset
            Screen::GetInstance().GetWidth() - g_listW - VIEW_DISTANCE_TO_LEFT_SIDE - 100,    // 100 : offset
            Screen::GetInstance().GetHeight() - 180);    // 180 : offset,
        // 200: width; 200: Adjust the visible height of scroll
        scroll_->Resize(200, Screen::GetInstance().GetHeight() - 200);
        scroll_->SetIntercept(true);
        uiViewGroup2_->Add(scroll_);
        SetControlButton();
    }

    UIView* selectBtnPos = new UIView();
    selectBtnPos->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    selectBtnPos->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    selectBtnPos->SetStyle(STYLE_BORDER_WIDTH, 0);
    selectBtnPos->SetStyle(STYLE_BORDER_RADIUS, 0);
    selectBtnPos->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, list->GetY() + g_selectPos, 100, 1); // 100: mean view width

    uiViewGroup2_->Add(list);
    uiViewGroup2_->Add(selectBtnPos);
    uiViewGroup2_->Add(scrollStateLabel_);
    uiViewGroup2_->Add(scrollSelectLabel_);
    SetLastPos(list);
}

bool UITestUIList::OnClick(UIView& view, const ClickEvent& event)
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
    } else if (&view == setBackAlignBtn_) {
        currentList_->EnableCrossDragBack(true);
    } else if (&view == setBackAlignOffBtn_) {
        currentList_->EnableCrossDragBack(false);
    } else if (&view == setAutoAlignACCIncBtn_) {
        autoAlignTime_ += ALINE_TIME_CHANGE_VALUE;
        currentList_->SetAutoAlignTime(autoAlignTime_);
    } else if (&view == setAutoAlignACCDncBtn_) {
        autoAlignTime_ -= ALINE_TIME_CHANGE_VALUE;
        currentList_->SetAutoAlignTime(autoAlignTime_);
    } else if (&view == setYScrollBarVisableBtn_) {
        currentList_->SetYScrollBarVisible(true);
    } else if (&view == setYScrollBarInvisableBtn_) {
        currentList_->SetYScrollBarVisible(false);
    }
    return true;
}

void UITestUIList::OnScrollStart(int16_t index, UIView* view)
{
    if (GetScrollState() == SCROLL_STATE_STOP) {
        scrollStateLabel_->SetText("STOP");
    } else {
        scrollStateLabel_->SetText("MOVE");
    }

    scrollStateLabel_->Invalidate();
}

void UITestUIList::OnScrollEnd(int16_t index, UIView* view)
{
    if (GetScrollState() == SCROLL_STATE_STOP) {
        scrollStateLabel_->SetText("STOP");
    } else {
        scrollStateLabel_->SetText("MOVE");
    }
    scrollStateLabel_->Invalidate();
}
void UITestUIList::OnItemSelected(int16_t index, UIView* view)
{
    if (view != nullptr) {
        scrollSelectLabel_->SetText(static_cast<UILabel*>(view)->GetText());
        view->Invalidate();
    } else {
        scrollSelectLabel_->SetText("NULL");
    }
}

void UITestUIList::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    scroll_->Add(btn);
    btn->SetPosition(10, positionY_, BUTTON_WIDHT3, BUTTON_HEIGHT3); // 10: button interval
    positionY_ += btn->GetHeight() + 10;    // 10: button interval
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    scroll_->Invalidate();
}

void UITestUIList::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS