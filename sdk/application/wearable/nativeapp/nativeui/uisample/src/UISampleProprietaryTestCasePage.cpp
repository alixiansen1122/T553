/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#include "uisample/UISampleModel.h"
#include "uisample/UISampleProprietaryTestCasePage.h"
#include "uisample/UISampleView.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_date.h"
#include "main/LoadImg.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "View.h"
#include "ui_test.h"

namespace OHOS {
namespace {
const uint16_t TESTCASE_BUTTON_HEIGHT = 100;
static int16_t SAMPLE_TITLE_TEXT_SIZE = 38;
} // namespace

REGIST_SLICE_PAGE(VIEW_UISAMPLE, SAMPLE_PAGES::SAMPLE_PROPRIETARY_TESTCASE_PAGE,
    UISampleProprietaryTestCasePage, false);
static UISampleProprietaryTestCasePage* g_instance = nullptr;
static RootView::OnKeyActListener *g_keyListener = nullptr;

UISampleProprietaryTestCasePage::UISampleProprietaryTestCasePage()
{
    g_instance = this;
}

UISampleProprietaryTestCasePage* UISampleProprietaryTestCasePage::GetInstance()
{
    return g_instance;
}

UISampleProprietaryTestCasePage::~UISampleProprietaryTestCasePage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }

    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }

    if (mainList_ != nullptr) {
        delete mainList_;
        mainList_ = nullptr;
    }

    if (backBtn_ != nullptr) {
        delete backBtn_;
        backBtn_ = nullptr;
    }
    g_instance = nullptr;
    isInTestCase_ = false;
}

static UIView* CreateViewCb(uint8_t type)
{
    UILabel* item = new UILabel();
    if (item == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Create item faild!");
        return nullptr;
    }

    item->SetPosition(0, 0, Screen::GetInstance().GetWidth(), TESTCASE_BUTTON_HEIGHT);
    item->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    item->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40); // 40: means font size
    item->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    item->SetTouchable(true);
    item->SetOnClickListener(UISampleProprietaryTestCasePage::GetInstance());
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    UNUSED(type);
    UILabel* item = dynamic_cast<UILabel*>(view);
    if (item != nullptr) {
        TestCaseInfo* info = static_cast<TestCaseInfo *>(data);
        if (info != nullptr) {
            item->SetText(info->sliceId);
            item->SetViewId(info->sliceId);
        }
    }
}

bool UISampleProprietaryTestCasePage::BackMenuPage(UIView& view)
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        UITest* uiTest = UITestGroup::GetInstance()->GetTestCase(view.GetViewId());
        if (uiTest != nullptr) {
            uiTest->TearDown();
        }
        RootView::GetInstance()->SetOnKeyActListener(g_keyListener);
        g_keyListener = nullptr;
        isInTestCase_ = false;

        container_->Add(title_);
        container_->Add(mainList_);
        container_->Invalidate();
    }
    return true;
}

void UISampleProprietaryTestCasePage::OnStart(void *data)
{
    if (container_ == nullptr) {
        container_ = new UIScrollViewNested();
        if (container_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " UISampleProprietaryTestCasePage::OnStart::container_ is nullptr");
            return;
        }
    }
    container_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    container_->SetVisible(true);

    CreateTitle();
    CreateMainList();
    CreateBackButton();
    AddViewToPageContainer(container_);
    container_->Invalidate();
}

void UISampleProprietaryTestCasePage::CreateTitle()
{
    if (title_ == nullptr) {
        title_ = new UILabel();
        if (title_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " UISampleProprietaryTestCasePage::OnStart::title_ is nullptr");
            return;
        }
    }

    title_->SetPosition(130, 0, 185, 80); // tile position x:130 size w: 185 h: 80
    title_->SetText("自研用例");
    title_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SAMPLE_TITLE_TEXT_SIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    title_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(title_);
}

void UISampleProprietaryTestCasePage::CreateMainList()
{
    if (mainList_ == nullptr) {
        mainList_ = new UISimpleList();
        if (mainList_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " UISampleProprietaryTestCasePage::OnStart::mainList_ is nullptr");
            return;
        }
    }
    mainList_->SetPosition(0, 100, Screen::GetInstance().GetWidth(), 366); // position: y: 100 w: 600 h:600
    mainList_->SetViewId("sample");
    mainList_->SetThrowDrag(true);
    mainList_->SetReboundSize(100); // 100: reboind size
    mainList_->SetElastic(true);
    mainList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    container_->Add(mainList_);
    mainList_->SetOnDragListener(this);

    UITestGroup::GetInstance()->SetUpHSTestCase();
    mainList_->ClearAll();
    List<TestCaseInfo>* testCaseList = UITestGroup::GetInstance()->GetTestCase();
    Contents content = {};
    for (auto node = testCaseList->Begin(); node != testCaseList->End(); node = node->next_) {
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 1;
        content.data = reinterpret_cast<void *>(&(node->data_));
        mainList_->AddContent(content);
    }
    mainList_->RefreshList();
}

void UISampleProprietaryTestCasePage::CreateBackButton()
{
    if (backBtn_ == nullptr) {
        backBtn_ = new UIButton();
    }
    if (backBtn_ != nullptr) {
        backBtn_->SetPosition(0, 202, 50, 50); // 0: x position, 202: y position, 50: width and height
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
        backBtn_->SetOnClickListener(this);
    }
}

bool UISampleProprietaryTestCasePage::OnClick(UIView& view, const ClickEvent& event)
{
    if (isInTestCase_) {
       return BackMenuPage(view);
    }

    if (container_ == nullptr) {
        return false;
    }

    UITest* uiTest = UITestGroup::GetInstance()->GetTestCase(view.GetViewId());
    if (uiTest == nullptr) {
        return false;
    }
    uiTest->SetUp();
    container_->RemoveAll();

    UIView* tempView = const_cast<UIView*>(uiTest->GetTestView());
    if (tempView != nullptr) {
        tempView->SetTouchable(true);
        tempView->SetViewId(view.GetViewId());
        tempView->SetPosition(tempView->GetX(), tempView->GetY());
        container_->Add(tempView);

        g_keyListener = RootView::GetInstance()->GetOnKeyActListener();
        RootView::GetInstance()->SetOnKeyActListener(nullptr);
    }

    if (backBtn_ != nullptr) {
        backBtn_->SetViewId(view.GetViewId());
        container_->Add(backBtn_);
    }

    isInTestCase_ =true;
    container_->Invalidate();
    return true;
}

bool UISampleProprietaryTestCasePage::OnDrag(UIView& view, const DragEvent& event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
        (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SAMPLE_MAIN_PAGE);
    }
    return true;
}
}