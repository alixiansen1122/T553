/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardMgrPage
 * Create: 2025-06-01
 */
#include "components/ui_list_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingCardModel.h"
#include "settings/page/SettingCardMgrPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::CARD_MGR_PAGE, SettingCardMgrPage, false);

static constexpr int16_t SETTING_CARD_MGR_LIST_X = 0;
static constexpr int16_t SETTING_CARD_MGR_LIST_Y = 110;
static constexpr int16_t SETTING_CARD_MGR_LIST_WIDTH = 454;
static constexpr int16_t SETTING_CARD_MGR_LIST_HEIGHT = 245;
static constexpr int16_t SETTING_CARD_MGR_LIST_BLANK = 40;
static constexpr int16_t SETTING_CARD_MGR_LIST_SELECTPOS = HORIZONTAL_RESOLUTION / 2;
static constexpr int16_t SETTING_CARD_MGR_LIST_DISTANCE = 1000;

static constexpr int16_t SETTING_CARD_MGR_ADD_X = 184;
static constexpr int16_t SETTING_CARD_MGR_ADD_Y = 16;
static constexpr int16_t SETTING_CARD_MGR_ADD_WIDTH = 85;
static constexpr int16_t SETTING_CARD_MGR_ADD_HEIGHT = 85;

static constexpr int16_t SETTING_CARD_MGR_DEL_X = 179;
static constexpr int16_t SETTING_CARD_MGR_DEL_Y = 354;
static constexpr int16_t SETTING_CARD_MGR_DEL_WIDTH = 96;
static constexpr int16_t SETTING_CARD_MGR_DEL_HEIGHT = 96;

void SettingCardMgrPage::OnStart(void* data)
{
    InitListView();
    InitButtonView();
    AddViewToPageContainer(list_);
    AddViewToPageContainer(addButton_);
    AddViewToPageContainer(delButton_);
}

void SettingCardMgrPage::OnResume()
{
    std::vector<SettingCardItemData>& cardData = SettingCardModel::GetInstance().GetCardData();
    list_->SetVisible(false);
    list_->ClearAll();
    for (auto& it : cardData) {
        if (!it.isSelect) {
            continue;
        }
        Contents cont;
        cont.createFunc = SettingCardMgrPage::OnSimpleListCreate;
        cont.updateFunc = SettingCardMgrPage::OnSimpleListUpdate;
        cont.type = 0;
        cont.data = &it;
        list_->AddContent(cont);
        list_->SetVisible(true);
    }
    list_->RefreshList();
}

void SettingCardMgrPage::OnStop()
{
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }
    if (addButton_ != nullptr) {
        delete addButton_;
        addButton_ = nullptr;
    }
    if (delButton_ != nullptr) {
        delete delButton_;
        delButton_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_CARD_MGR_ADD, SETTING_IMAGE);
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_CARD_MGR_DEL, SETTING_IMAGE);
}

bool SettingCardMgrPage::OnClick(UIView& view, const ClickEvent& event)
{
    if (strcmp(view.GetViewId(), "add") == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SettingPageId::CARD_ADD_PAGE,
            TransitionType::TRANSITION_INVALID, true);
    } else if (strcmp(view.GetViewId(), "del") == 0) {
        uint16_t selectIndex = list_->GetSelectView()->GetViewIndex();
        std::vector<SettingCardItemData>& cardData = SettingCardModel::GetInstance().GetCardData();

        // 更新系统数据
        SettingCardModel::GetInstance().DeleteCard(cardData[selectIndex].index);
        SettingCardModel::GetInstance().RefreshCardData();

        // 更新界面数据
        list_->RemoveContent(selectIndex);
        list_->SetVisible(false);
        for (auto& it : cardData) {
            if (it.isSelect) {
                list_->SetVisible(true);
                break;
            }
        }
        list_->RefreshList();
    }
    return true;
}

void SettingCardMgrPage::InitListView()
{
    list_ = new UISimpleList();
    if (list_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardMgrPage::InitListView list_ new fail");
        return;
    }
    list_->SetDirection(UIAbstractScroll::HORIZONTAL);
    list_->SetBoundaryIntercept(DragEvent::DIRECTION_LEFT_TO_RIGHT, false);
    list_->SetPosition(SETTING_CARD_MGR_LIST_X, SETTING_CARD_MGR_LIST_Y);
    list_->Resize(SETTING_CARD_MGR_LIST_WIDTH, SETTING_CARD_MGR_LIST_HEIGHT);
    list_->SetScrollBlankSize(SETTING_CARD_MGR_LIST_BLANK);
    list_->SetScrollBlankSize(0, UIAbstractScroll::Direction::LEFT);
    list_->SetSelectPosition(SETTING_CARD_MGR_LIST_SELECTPOS);
    list_->SetMaxScrollDistance(SETTING_CARD_MGR_LIST_DISTANCE);
    list_->SetDraggable(true);
    list_->SetThrowDrag(true);
    list_->EnableAutoAlign(true);
    list_->SetIntercept(false);
}

void SettingCardMgrPage::InitButtonView()
{
    addButton_ = new UIButton();
    if (addButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardMgrPage::InitButtonView addButton_ new fail");
        return;
    }
    addButton_->SetPosition(SETTING_CARD_MGR_ADD_X, SETTING_CARD_MGR_ADD_Y);
    addButton_->Resize(SETTING_CARD_MGR_ADD_WIDTH, SETTING_CARD_MGR_ADD_HEIGHT);
    addButton_->SetViewId("add");
    addButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    addButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    addButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    ImageInfo* addImage = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_CARD_MGR_ADD, SETTING_IMAGE);
    addButton_->SetImageSrc(addImage, addImage);
    addButton_->SetOnClickListener(this);

    delButton_ = new UIButton();
    if (delButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardMgrPage::InitButtonView delButton_ new fail");
        return;
    }
    delButton_->SetPosition(SETTING_CARD_MGR_DEL_X, SETTING_CARD_MGR_DEL_Y);
    delButton_->Resize(SETTING_CARD_MGR_DEL_WIDTH, SETTING_CARD_MGR_DEL_HEIGHT);
    delButton_->SetViewId("del");
    ImageInfo* delImage = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_CARD_MGR_DEL, SETTING_IMAGE);
    delButton_->SetImageSrc(delImage, delImage);
    delButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    delButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    delButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    delButton_->SetOnClickListener(this);
}

UIView* SettingCardMgrPage::OnSimpleListCreate(uint8_t type)
{
    UNUSED(type);
    return new SettingCardMgrItemGroup();
}

void SettingCardMgrPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
{
    UNUSED(type);
    if (view == nullptr || data == nullptr) {
        return;
    }
    SettingCardMgrItemGroup* itemGroup = dynamic_cast<SettingCardMgrItemGroup*>(view);
    if (itemGroup == nullptr) {
        return;
    }
    SettingCardItemData* itemData = static_cast<SettingCardItemData*>(data);
    itemGroup->UpdateItemGroup(itemData);
}
}