/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopDialPage
 * Create: 2025-06-01
 */
#include "common/screen.h"
#include "components/ui_swipe_view_nested.h"
#include "gfx_utils/graphic_math.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingDesktopModel.h"
#include "settings/page/SettingDesktopDialPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DESKTOP_DIAL_PAGE, SettingDesktopDialPage, false);

static constexpr int16_t SETTING_ROLLER_ITEM_WIDTH = 250;
static constexpr int16_t SETTING_ROLLER_ITEM_HEIHT = 250;
static constexpr int16_t SETTING_ROLLER_ITEM_PADDING = 80;
static constexpr float SETTING_ROLLER_BASE_ANGLE = 360.0;
static constexpr float SETTING_ROLLER_HALF_BASE_ANGLE = 180.0;

static constexpr int16_t SETTING_DIAL_BUTTON_X = -454;
static constexpr int16_t SETTING_DIAL_BUTTON_Y = 363;
static constexpr int16_t SETTING_DIAL_BUTTON_WDITH = 454*3;
static constexpr int16_t SETTING_DIAL_BUTTON_HEIGHT = 454*3;
static constexpr int16_t SETTING_DIAL_BUTTON_RADIUS = 454*3;
#define  SETTING_DIAL_BUTTON_COLOR Color::GetColorFromRGB(13, 159, 251) // 0xFF0D9FFB

static constexpr int16_t SETTING_DIAL_BUTTON_LABEL_X = 105;
static constexpr int16_t SETTING_DIAL_BUTTON_LABEL_Y = 375;
static constexpr int16_t SETTING_DIAL_BUTTON_LABEL_WIDTH = 244;
static constexpr int16_t SETTING_DIAL_BUTTON_LABEL_HEIGHT = 54;
static constexpr int16_t SETTING_DIAL_BUTTON_LABEL_FSIZE = 38;
static const char* SETTING_DIAL_BUTTON_LABEL_TEXT = "设置";

static constexpr int16_t SETTING_DIAL_TITLE_X = 100;
static constexpr int16_t SETTING_DIAL_TITLE_Y = 39;
static constexpr int16_t SETTING_DIAL_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_DIAL_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_DIAL_TITLE_FSZIE = 38;

void SettingDesktopDialPage::OnStart(void* data)
{
    InitDialView();
    InitCtrlView();
    AddViewToPageContainer(coverflow_);
    AddViewToPageContainer(rollerView_);
    AddViewToPageContainer(itemLabel_);
}

void SettingDesktopDialPage::OnResume()
{
    GetSlicePageContainer()->SetIntercept(true);
    GetSlicePageContainer()->SetOnDragListener(nullptr); // 取消跟手返回
    SettingDesktopModel::GetInstance().LoadAllDial();
    const auto& dialInfo = SettingDesktopModel::GetInstance().GetDialInfo();
    itemIndex_ = SettingDesktopModel::GetInstance().GetPageIndex();

    if (SettingDesktopModel::GetInstance().GetDialStyle() == DialStyle::ROLLER) {
        itemCount_ = 0; // 重置计数
        for (const auto& iter : dialInfo) {
            rollerView_->AddItem(iter.dialName, iter.preImageInfo) ? itemCount_++ : itemCount_;
        }
        rollerView_->SetCurrentItem(dialInfo[itemIndex_].dialName);
        rollerView_->SetVisible(true);
    } else { // DialStyle::COVERFLOW风格
        for (const auto& iter : dialInfo) {
            coverflow_->Add(iter.dialName.c_str(), iter.preImageInfo);
        }
        for (uint16_t i = 0; i < coverflow_->GetChildrenNumber(); i++) {
            std::string curDialName = coverflow_->GetViewByIndex(i)->GetViewId();
            if (dialInfo[itemIndex_].dialName == curDialName) {
                coverflow_->SetCurrentPage(i);
                break;
            }
        }
        coverflow_->SetVisible(true);
    }
    itemLabel_->SetText(dialInfo[itemIndex_].dialName.c_str());
}

void SettingDesktopDialPage::OnPause()
{
    if (SettingDesktopModel::GetInstance().GetDialStyle() == DialStyle::COVERFLOW) {
        coverflow_->ClearAll();
    } else {
        rollerView_->ClearAll();
    }
    SettingDesktopModel::GetInstance().ClearDialInfo();
}

void SettingDesktopDialPage::OnStop()
{
    if (coverflow_ != nullptr) {
        delete coverflow_;
        coverflow_ = nullptr;
    }
    if (rollerView_ != nullptr) {
        delete rollerView_;
        rollerView_ = nullptr;
    }
    if (itemLabel_ != nullptr) {
        delete itemLabel_;
        itemLabel_ = nullptr;
    }
}

bool SettingDesktopDialPage::OnClick(UIView& view, const ClickEvent& event)
{
    SettingDesktopModel::GetInstance().SaveDialSetting(itemIndex_);
    NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
    return true;
}

bool SettingDesktopDialPage::OnScroll()
{
    int16_t windowCenterX = HORIZONTAL_RESOLUTION / 2;
    int16_t windowDistance = windowCenterX;
    int16_t currItemDistance = 0;
    std::string curDialName;
    for (uint16_t i = 0; i < coverflow_->GetChildrenNumber(); i++) {
        Rect itemRect = coverflow_->GetViewByIndex(i)->GetOrigRect();
        int16_t itemCenterX = itemRect.GetLeft() + itemRect.GetWidth() / 2;
        int16_t itemDistance = MATH_ABS(itemCenterX - windowCenterX);
        if (windowDistance > itemDistance) { // 和中心点距离最小的组件是当前显示页面
            curDialName = coverflow_->GetViewByIndex(i)->GetViewId();
            currItemDistance = itemDistance;
            windowDistance = itemDistance;
        }
    }
    const auto& dialInfo = SettingDesktopModel::GetInstance().GetDialInfo();
    for (uint8_t i = 0; i < dialInfo.size(); ++i) {
        if (dialInfo[i].dialName == curDialName) {
            itemIndex_ = i;
            itemLabel_->SetText(dialInfo[itemIndex_].dialName.c_str());
            break;
        }
    }
    // 公式: 透明度 = 最大透明度- 最大透明度*(组件中心点/窗口中心点); 当组件中心点=窗口中心点时, OPA的值最大
    uint16_t opa = OPA_OPAQUE - (uint16_t)((int32_t)OPA_OPAQUE * currItemDistance / windowCenterX);
    itemLabel_->SetStyle(STYLE_TEXT_OPA, opa);
    return true;
}

void SettingDesktopDialPage::OnScroll(UIRollerView* roller, std::string currentItemName, float currentItemAngle)
{
    const float baseAngle = SETTING_ROLLER_BASE_ANGLE;
    const float halfBaseAngle = SETTING_ROLLER_HALF_BASE_ANGLE;
    if (FloatMoreEqual(currentItemAngle, halfBaseAngle)) {
        currentItemAngle -= baseAngle;
    }
    if (currentItemAngle < 0) {
        currentItemAngle = -currentItemAngle;
    }
    float maxAngle = baseAngle / itemCount_;
    float coef = 1.0 - 1.0 / (maxAngle) * currentItemAngle;
    itemLabel_->SetStyle(STYLE_TEXT_OPA, coef * OPA_OPAQUE);
    itemLabel_->SetText(currentItemName.c_str());
    auto& dialInfo = SettingDesktopModel::GetInstance().GetDialInfo();
    for (uint8_t i = 0; i < dialInfo.size(); ++i) {
        if (dialInfo[i].dialName == currentItemName) {
            itemIndex_ = i;
        }
    }
}

void SettingDesktopDialPage::InitDialView()
{
    coverflow_ = new UICoverflowView();
    if (coverflow_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopDialPage::InitDialView  coverflow_ new fail");
        return;
    }
    coverflow_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    coverflow_->SetOnCoverflowListener(this);
    coverflow_->SetOnClickListener(this);
    coverflow_->SetThrowDrag(true);
    coverflow_->SetIntercept(true);
    coverflow_->SetVisible(false);

    rollerView_ = new UIRollerView();
    if (rollerView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopDialPage::InitDialView  rollerView_ new fail");
        return;
    }
    rollerView_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    rollerView_->SetItemSize(SETTING_ROLLER_ITEM_WIDTH, SETTING_ROLLER_ITEM_HEIHT);
    rollerView_->SetThrowDrag(true);
    rollerView_->SetIntercept(true);
    rollerView_->SetItemPadding(SETTING_ROLLER_ITEM_PADDING);
    rollerView_->SetOnScrollListener(this);
    rollerView_->SetOnClickListener(this);
    rollerView_->SetVisible(false);
}

void SettingDesktopDialPage::InitCtrlView()
{
    itemLabel_ = new UILabel();
    if (itemLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopDialPage::InitCtrlView  itemLabel_ new fail");
        return;
    }
    itemLabel_->SetPosition(SETTING_DIAL_TITLE_X, SETTING_DIAL_TITLE_Y);
    itemLabel_->Resize(SETTING_DIAL_TITLE_WIDTH, SETTING_DIAL_TITLE_HEIGHT);
    itemLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    itemLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_DIAL_TITLE_FSZIE);
    itemLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
}
}
