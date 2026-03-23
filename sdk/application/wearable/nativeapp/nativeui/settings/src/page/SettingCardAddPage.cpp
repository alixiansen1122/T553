/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardAddPage
 * Create: 2025-06-01
 */
#include "components/ui_list_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingCardModel.h"
#include "settings/page/SettingCardAddPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::CARD_ADD_PAGE, SettingCardAddPage, false);

static constexpr int16_t SETTING_CARD_ADD_LIST_Y = 125;
static constexpr int16_t SETTING_CARD_ADD_LIST_HEIGHT = 397;
static constexpr int16_t SETTING_CARD_ADD_LIST_DISTANCE = 1000;
static constexpr int16_t SETTING_CARD_ADD_LIST_BLANK = 40;
static constexpr int16_t SETTING_CARD_ADD_LIST_SELECTPOS = HORIZONTAL_RESOLUTION / 4;
static constexpr int16_t SETTING_CARD_ADD_TITLE_X = 100;
static constexpr int16_t SETTING_CARD_ADD_TITLE_Y = 39;
static constexpr int16_t SETTING_CARD_ADD_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_CARD_ADD_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_CARD_ADD_TITLE_FSIZE = 38;
static const char* SETTING_CARD_ADD_TITLE_TEXT = "添加卡片";

void SettingCardAddPage::OnStart(void* data)
{
    list_ = new UISimpleList();
    if (list_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardAddPage::OnStart list_ new fail");
        return;
    }
    list_->SetDirection(UIAbstractScroll::HORIZONTAL);
    list_->SetBoundaryIntercept(DragEvent::DIRECTION_LEFT_TO_RIGHT, false);
    list_->SetPosition(0, SETTING_CARD_ADD_LIST_Y, HORIZONTAL_RESOLUTION, SETTING_CARD_ADD_LIST_HEIGHT);
    list_->SetScrollBlankSize(SETTING_CARD_ADD_LIST_BLANK);
    list_->SetSelectPosition(SETTING_CARD_ADD_LIST_SELECTPOS);
    list_->SetMaxScrollDistance(SETTING_CARD_ADD_LIST_DISTANCE);
    list_->SetDraggable(true);
    list_->SetThrowDrag(true);
    list_->EnableAutoAlign(true);
    list_->SetIntercept(false);

    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardAddPage::OnStart title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_CARD_ADD_TITLE_X, SETTING_CARD_ADD_TITLE_Y);
    title_->Resize(SETTING_CARD_ADD_TITLE_WIDTH, SETTING_CARD_ADD_TITLE_HEIGHT);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_CARD_ADD_TITLE_FSIZE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetText(SETTING_CARD_ADD_TITLE_TEXT);

    AddViewToPageContainer(list_);
    AddViewToPageContainer(title_);
}

void SettingCardAddPage::OnResume()
{
    std::vector<SettingCardItemData>& cardData = SettingCardModel::GetInstance().GetCardData();
    if (cardData.size() == 0) {
        list_->SetVisible(false);
    } else {
        list_->ClearAll();
        for(auto& it : cardData) {
            it.contextPtr = this;

            Contents cont;
            cont.createFunc = SettingCardAddPage::OnSimpleListCreate;
            cont.updateFunc = SettingCardAddPage::OnSimpleListUpdate;
            cont.type = 0;
            cont.data = &it;
            list_->AddContent(cont);
        }
        list_->SetVisible(true);
        list_->RefreshList();
    }
}

void SettingCardAddPage::OnStop()
{
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
}

bool SettingCardAddPage::OnClick(UIView& view, const ClickEvent& event)
{
    int16_t index = view.GetViewIndex();
    std::vector<SettingCardItemData>& cardData = SettingCardModel::GetInstance().GetCardData();
    SettingCardItemData& currData = cardData[index];

    if (!currData.isSelect) {
        SettingCardModel::GetInstance().AddCard(currData.index);
        SettingCardModel::GetInstance().RefreshCardData();

        if (cardData.size() == 0) {
            list_->SetVisible(false);
        }
        list_->RefreshList();
    }
    return true;
}

UIView* SettingCardAddPage::OnSimpleListCreate(uint8_t type)
{
    UNUSED(type);
    return new SettingCardAddItemGroup();
}

void SettingCardAddPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
{
    UNUSED(type);
    if (view == nullptr || data == nullptr) {
        return;
    }
    SettingCardAddItemGroup* itemGroup = dynamic_cast<SettingCardAddItemGroup*>(view);
    if (itemGroup == nullptr) {
        return;
    }
    SettingCardItemData* itemData = static_cast<SettingCardItemData*>(data);
    SettingCardAddPage* page = static_cast<SettingCardAddPage*>(itemData->contextPtr);
    itemGroup->UpdateItemGroup(itemData);
    itemGroup->SetOnClickListener(page);
}
}