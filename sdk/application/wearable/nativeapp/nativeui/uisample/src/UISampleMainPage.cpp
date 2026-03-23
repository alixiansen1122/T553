/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#include "uisample/UISampleMainPage.h"
#include "uisample/UISampleModel.h"
#include "uisample/UISampleView.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "common/image_cache_manager.h"
#include "ui_resource_date.h"
#include "main/LoadImg.h"
#include "components/ui_label_button.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_test.h"

namespace OHOS {
namespace {
const uint16_t TESTCASE_BUTTON_HEIGHT = 100;
const uint16_t STYLE_BORDER_WIDTH_VALUE = 4;
static int16_t SAMPLE_TITLE_TEXT_SIZE = 38;
} // namespace
REGIST_SLICE_PAGE(VIEW_UISAMPLE, SAMPLE_PAGES::SAMPLE_MAIN_PAGE, UISampleMainPage, true);
static UISampleMainPage* g_instance = nullptr;

UISampleMainPage::UISampleMainPage()
{
    g_instance = this;
}

UISampleMainPage* UISampleMainPage::GetInstance()
{
    return g_instance;
}

UISampleMainPage::~UISampleMainPage()
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
    g_instance = nullptr;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    UNUSED(type);
    UILabelButton* item = dynamic_cast<UILabelButton*>(view);
    if (item != nullptr) {
        TestCaseInfo* info = static_cast<TestCaseInfo *>(data);
        if (info != nullptr) {
            item->SetText(info->sliceId);
            item->SetViewId(info->sliceId);
        }
    }
}

static UIView* CreateViewCb(uint8_t type)
{
    UILabelButton* item = new UILabelButton();
    if (item == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Create item faild!");
        return nullptr;
    }

    item->SetPosition(0, 0);
    item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::INACTIVE);
    item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::INACTIVE);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::INACTIVE);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    item->Resize(HORIZONTAL_RESOLUTION - TEXT_DISTANCE_TO_LEFT_SIDE, TESTCASE_BUTTON_HEIGHT);
    item->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40); // 40: means font size
    item->SetAlign(TEXT_ALIGNMENT_LEFT);
    item->SetLabelPosition(150, 0); // 150: label x-coordinate
    item->SetOnClickListener(UISampleMainPage::GetInstance());
    return item;
}

void UISampleMainPage::OnStart(void *data)
{
    if (container_ == nullptr) {
        container_ = new UIScrollViewNested();
        if (container_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " UISampleMainPage::OnStart::container_ is nullptr");
            return;
        }
    }
    container_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    container_->SetVisible(true);

    CreateTitle();
    CreateMainList();
    AddViewToPageContainer(container_);
    container_->Invalidate();
}

void UISampleMainPage::CreateTitle()
{
    if (title_ == nullptr) {
        title_ = new UILabel();
        if (title_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " UISampleMainPage::OnStart::title_ is nullptr");
            return;
        }
    }

    title_->SetPosition(130, 0, 185, 80); // tile position x:130 size w: 185 h: 80
    title_->SetText("用例");
    title_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SAMPLE_TITLE_TEXT_SIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    title_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(title_);
}

void UISampleMainPage::CreateMainList()
{
    if (mainList_ == nullptr) {
        mainList_ = new UISimpleList();
        if (mainList_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " UISampleMainPage::OnStart::mainList_ is nullptr");
            return;
        }
    }
    mainList_->SetPosition(0, 100, 600, 600); // position: y: 100 w: 600 h:600
    mainList_->SetThrowDrag(true);
    mainList_->SetReboundSize(100); // 100: reboind size
    mainList_->SetElastic(true);
    container_->Add(mainList_);

    if (PageTransitionMgr::GetInstance().GetTopSlideBackImage() == nullptr) {
        mainList_->SetOnDragListener(this);
    }
    UITestGroup::GetInstance()->SetUpTestCase();
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

bool UISampleMainPage::OnClick(UIView& view, const ClickEvent& event)
{
    if (strcmp(view.GetViewId(), "自研用例") == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SAMPLE_PAGES::SAMPLE_PROPRIETARY_TESTCASE_PAGE);
    } else if (strcmp(view.GetViewId(), "开源用例") == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SAMPLE_PAGES::SAMPLE_NATIVE_TESTCASE_PAGE);
    }
    return true;
}

bool UISampleMainPage::OnDrag(UIView& view, const DragEvent& event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
        (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}
}

