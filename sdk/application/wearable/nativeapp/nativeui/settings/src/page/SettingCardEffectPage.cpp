/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardEffecPage
 * Create: 2025-04
 */
#include "SlicePageFactory.h"
#include "NativeRegisterManager.h"
#include "PageTransitionMgr.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "settings/SettingView.h"
#include "settings/common/SettingCommon.h"
#include "settings/page/SettingCardEffectPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::CARD_EFFECT_PAGE, SettingCardEffecPage, false);
static SettingCardEffecPage *g_pSettingCardEffecPage = nullptr;
static constexpr uint8_t SETTINGS_ITEM_FONT_SIZE = 40;
static constexpr int16_t SETTINGS_RESIZE_SIZE_WIDTH = HORIZONTAL_RESOLUTION;
static constexpr int16_t SETTINGS_RESIZE_SIZE_HEIGHT = 60;
static constexpr int16_t SETTING_TITLE_POSITION_X = 0;
static constexpr int16_t SETTING_TITLE_POSITION_Y = 20;
static constexpr int16_t SETTING_LIST_POSITION_X = 0;
static constexpr int16_t SETTING_LIST_POSITION_Y = 0;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 100;
static constexpr int16 MAX_SCR_DIS = 454;
static constexpr int16 SNUM_2 = 2;

SettingCardEffecPage::SettingCardEffecPage()
{
    g_pSettingCardEffecPage = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingCardEffecPage::SettingCardEffecPage");
}

SettingCardEffecPage::~SettingCardEffecPage()
{
    g_pSettingCardEffecPage = nullptr;
    if (titleText_ != nullptr) {
        delete titleText_;
        titleText_ = nullptr;
    }

    contentList_->ClearAll();
    if (contentList_ != nullptr) {
        delete contentList_;
        contentList_ = nullptr;
    }
    effectListDatas_.clear();
}

void SettingCardEffecPage::OnStart(void* data)
{
    titleText_ = new UILabel();
    if (titleText_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingCardEffecPage::OnStart::title is nullptr");
        return;
    }
    titleText_->SetPosition(SETTING_TITLE_POSITION_X, SETTING_TITLE_POSITION_Y);
    titleText_->Resize(SETTINGS_RESIZE_SIZE_WIDTH, SETTINGS_RESIZE_SIZE_HEIGHT);
    titleText_->SetText("卡片动效");
    titleText_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTINGS_ITEM_FONT_SIZE);
    titleText_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleText_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    titleText_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    titleText_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    titleText_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    contentList_ = new UISimpleList();
    if (contentList_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingCardEffecPage::OnStart::contentList_ is nullptr");
        return;
    }
    contentList_->SetDirection(UIAbstractScroll::HORIZONTAL);
    contentList_->SetBoundaryIntercept(DragEvent::DIRECTION_LEFT_TO_RIGHT, false);
    contentList_->SetPosition(SETTING_LIST_POSITION_X, SETTING_LIST_POSITION_Y);
    contentList_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    contentList_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2); // middle position
    contentList_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE, UIAbstractScroll::Direction::RIGHT);
    contentList_->SetDraggable(true);
    contentList_->SetThrowDrag(true);
    contentList_->SetLoopState(false);
    contentList_->EnableAutoAlign(true);
    contentList_->SetScrollStateListener(this);
    AddViewToPageContainer(contentList_);
    AddViewToPageContainer(titleText_);

    SettingCardModel::GetInstance().InitEffectItem();
    SetCardEffectListData(SettingCardModel::GetInstance().GetEffectListItems());
}

bool SettingCardEffecPage::OnClick(UIView& view, const ClickEvent& event)
{
    CardEffectID swipeId = CardEffectID::UI_INVALIDE;
    SettingCardEffectItemGroup* setSwipeEffectView = dynamic_cast<SettingCardEffectItemGroup*>(&view);
    if (setSwipeEffectView == nullptr) {
        return false;
    }
    swipeId = setSwipeEffectView->GetViewId(event);
    if (swipeId != CardEffectID::UI_INVALIDE) {
        SettingCardModel::GetInstance().SetSwipeEffectStyle(swipeId);
        NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
    }
    return true;
}

void SettingCardEffecPage::OnItemSelected(int16 index, UIView* view)
{
    UNUSED(index);
    if (view != nullptr) {
        itemGroupSele_ = static_cast<SettingCardEffectItemGroup*>(view);
        itemGroupSele_->SelectedChange();

        if (preItemGroupSele_ != nullptr && preItemGroupSele_ != itemGroupSele_) {
            preItemGroupSele_->Reset();
        }
        preItemGroupSele_ = itemGroupSele_;
    }
}

UIView* SettingCardEffecPage::OnSimpleListCreate(uint8_t type)
{
    SettingCardEffectItemGroup* itemGroup = new SettingCardEffectItemGroup();
    if (itemGroup == nullptr) {
        return nullptr;
    }
    itemGroup->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    itemGroup->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    itemGroup->SetTouchable(true);
    itemGroup->SetOnClickListener(g_pSettingCardEffecPage);
    return itemGroup;
}

void SettingCardEffecPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SettingCardEffectItemGroup* itemGroup = dynamic_cast<SettingCardEffectItemGroup*>(view);
    if (itemGroup == nullptr) {
        return;
    }
    CardEffectPair* content = static_cast<CardEffectPair*>(data);
    itemGroup->SetItemInfo(content->first, content->second);
}

void SettingCardEffecPage::SetCardEffectListData(const std::map<CardEffectID, SettingCardEffectSample> &effectMap)
{
    contentList_->ClearAll();
    uint16_t selectIndex = static_cast<uint16_t>(
        SettingCardModel::GetInstance().GetSwipeEffectStyle()) / SNUM_2;
    effectListDatas_.clear();
    for (auto currentIt = effectMap.begin(); currentIt != effectMap.end(); ++currentIt) {
        auto nextIt = std::next(currentIt);
        CardEffectPair swipeEffectData;
        swipeEffectData.first = currentIt->second;
        if (nextIt != effectMap.end()) {
            swipeEffectData.second = nextIt->second;
            currentIt = nextIt;
        } else {
            SettingCardEffectSample sample;
            swipeEffectData.second = sample;
        }
        effectListDatas_.push_back(swipeEffectData);
    }
    int index = 1;
    for (auto currentIt = effectListDatas_.begin(); currentIt != effectListDatas_.end(); ++currentIt) {
        Contents content;
        content.createFunc = OnSimpleListCreate;
        content.updateFunc = OnSimpleListUpdate;
        content.type = index++;
        content.data = reinterpret_cast<void *>(&*currentIt);
        contentList_->AddContent(content);
    }

    contentList_->ScrollTo(selectIndex);
    contentList_->ScrollBy(-SNUM_2);
    contentList_->RefreshList();
}
}