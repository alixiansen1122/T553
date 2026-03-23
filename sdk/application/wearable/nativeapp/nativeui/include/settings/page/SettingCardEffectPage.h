/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardEffectPage
 * Create: 2025-04
 */
#ifndef SETTING_CARD_EFFECT_PAGE_H
#define SETTING_CARD_EFFECT_PAGE_H

#include <list>
#include "View.h"
#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_simple_list.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCardEffectSample.h"
#include "settings/model/SettingCardModel.h"
#include "settings/common/SettingCardEffectItemGroup.h"

namespace OHOS {
using CardEffectPair = std::pair<SettingCardEffectSample, SettingCardEffectSample>;

class SettingCardEffecPage : public SlicePage<SettingPresenter>,
                              public UIView::OnClickListener,
                              public ListScrollListener {
public:
    SettingCardEffecPage();
    ~SettingCardEffecPage() override;
    void OnStart(void* data) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnItemSelected(int16 index, UIView* view) override;
    void SetCardEffectListData(const std::map<CardEffectID, SettingCardEffectSample> &effectMap);
protected:
    static UIView* OnSimpleListCreate(uint8_t type);
    static void OnSimpleListUpdate(UIView* view, void* data, uint8_t type);
private:
    UILabel* titleText_{nullptr};
    UISimpleList* contentList_{nullptr};
    std::list<CardEffectPair> effectListDatas_;
    SettingCardEffectItemGroup* itemGroupSele_{nullptr};
    SettingCardEffectItemGroup* preItemGroupSele_{nullptr};
};
}
#endif