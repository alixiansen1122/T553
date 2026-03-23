/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardModel
 * Create: 2025-06-01
 */
#include <cstdint>
#include <algorithm>
#include "dfx_reboot.h"
#include "kv_store.h"
#include "UiConfig.h"
#include "main/MainModel.h"
#include "settings/common/SettingCardEffectResourceImage.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingCardModel.h"

namespace OHOS {
static const int STYLE_VALUE_LEN = 128;
static const char* SWIPE_EFFECT = "swipe_effect";

static const std::map<CardEffectID, std::pair<const char*, std::vector<int>>> g_effectMap = {
    {CardEffectID::UI_CUBE, std::make_pair("3D折叠", UI_3D_FOLD_RESIDS)},
    {CardEffectID::UI_CARD_FLIP, std::make_pair("十字平滑", UI_CARD_FLIP_RESIDS)},
    {CardEffectID::UI_PUSH_PRESS, std::make_pair("推压", UI_PUSH_PRESS_RESIDS)},
    {CardEffectID::UI_TURN_PAGE, std::make_pair("翻页", UI_TURN_PAGE_RESIDS)},
    {CardEffectID::UI_WINDMILL, std::make_pair("风车", UI_WINDMILL_RESIDS)},
    {CardEffectID::UI_ZOOM, std::make_pair("缩放", UI_ZOOM_RESIDS)},
};

SettingCardModel::SettingCardModel()
{
    std::vector<SettingUnionItemData> tempUnionData = {{
        PlainTextItemData{"卡片"},
        IconTextNavigationItemData{SettingPageId::CARD_EFFECT_PAGE, IMAGE_CARD_LIST_CARDANIMATOR, "卡片动效",
            nullptr,true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::CARD_MGR_PAGE, IMAGE_CARD_LIST_CARDMANAGER, "卡片管理",
            nullptr,true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
    }};
    cardUnionData_.swap(tempUnionData);

    selectStyle_ = CardEffectID::UI_INVALIDE;
    int backgroundImageId = IMAGE_DESKTOP_BG;
    const char* resourceImage = SETTING_IMAGE;
    for (auto it = g_effectMap.begin(); it != g_effectMap.end(); ++it) {
        effects_[it->first] = SettingCardEffectSample(it->first,
            it->second.second, it->second.first,
            backgroundImageId, resourceImage, false);
    }
}

std::vector<SettingUnionItemData>& SettingCardModel::GetCardUnionData()
{
    return cardUnionData_;
}

std::vector<SettingCardItemData>& SettingCardModel::GetCardData()
{
    RefreshCardData();
    return cardData_;
}

void SettingCardModel::RefreshCardData()
{
    MainModel::GetInstance().InitCardSettings();
    uint8_t selectedCardCount = MainModel::GetInstance().GetCardSettingCount();
    CardId *selectedCardSettings = MainModel::GetInstance().GetCardSettings();

    if (cardData_.size() != (MAX_CARD - 1)) {
        cardData_.resize(MAX_CARD - 1); // 系统中的card总个数是固定的, WATCH_FACE不属于card范畴
    }
    for (uint8_t i = 0; i < cardData_.size(); i++) {
        CardId cardId = (CardId)(i+1); // CardId是定义的枚举, WATCH_FACE=0, COMPASS=1;
        uint8_t cardIndex = i;         // CardIndex是cardInfoMap中的index, cardInfoMap[0]=COMPASS

        const CardInfo *info = MainModel::GetInstance().GetCardInfo(cardId);
        if (info == nullptr) {
            continue;
        }

        SettingCardItemData& data = cardData_[cardIndex];
        data.index = cardIndex;
        data.id = info->id;
        data.image = info->cardImg;
        data.isSelect = false;
        for (uint8_t j = 0; j < selectedCardCount; j++) {
            if (selectedCardSettings[j] == cardId) {
                data.isSelect = true;
                break;
            }
        }
    }
}

void SettingCardModel::AddCard(uint8_t index)
{
    MainModel::GetInstance().AddCardSetting(index);
}

void SettingCardModel::DeleteCard(uint8_t index)
{
    MainModel::GetInstance().DeleteCardSetting(index);
}

void SettingCardModel::SetSelectedCard(uint8_t cardId)
{
    selectedCardId_ = cardId;
}

uint8_t SettingCardModel::GetSelectedCard(void)
{
    return selectedCardId_;
}

void SettingCardModel::InitEffectItem(void)
{
    for (auto& pair : effects_) {
        pair.second.selected = false;
    }
    CardEffectID selectStyle = this->GetSwipeEffectStyle();
    effects_[selectStyle].selected = true;
}

std::map<CardEffectID, SettingCardEffectSample>& SettingCardModel::GetEffectListItems(void)
{
    return effects_;
}

CardEffectID SettingCardModel::GetSwipeEffectStyle(void)
{
    if (selectStyle_ != CardEffectID::UI_INVALIDE) {
        return selectStyle_;
    }
    selectStyle_ = CardEffectID::UI_ZOOM;
    char temp[STYLE_VALUE_LEN] = { 0 };
    (void)UtilsGetValue(SWIPE_EFFECT, temp, STYLE_VALUE_LEN);
    auto it = std::find_if(effects_.begin(), effects_.end(),
        [temp](const std::pair<CardEffectID, SettingCardEffectSample>& pair) {
            return strcmp(temp, pair.second.label) == 0;
        });
    if (it != effects_.end()) {
        selectStyle_ = it->first;
    }
    return selectStyle_;
}

void SettingCardModel::SetSwipeEffectStyle(CardEffectID swipeId)
{
    if (swipeId == CardEffectID::UI_INVALIDE) {
        // If the corresponding style is not found, set the default value to UI_ZOOM.
        swipeId = CardEffectID::UI_ZOOM;
    }
    (void)UtilsSetValue(SWIPE_EFFECT, effects_[swipeId].label);
    selectStyle_ = swipeId;
}
} // OHOS
