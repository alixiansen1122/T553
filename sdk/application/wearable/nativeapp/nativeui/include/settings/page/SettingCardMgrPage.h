/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardMgrPage
 * Create: 2025-06-01
 */
#ifndef SETTING_CARD_MGR_PAGE_H
#define SETTING_CARD_MGR_PAGE_H

#include "components/ui_scroll_view_nested.h"
#include "components/ui_simple_list.h"
#include "components/ui_label.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingCardMgrPage : public SlicePage<SettingPresenter>,
                           public UIView::OnClickListener {
public:
    void OnStart(void* data) override;
    void OnResume() override;
    void OnStop() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
protected:
    void InitListView();
    void InitButtonView();
    static UIView* OnSimpleListCreate(uint8_t type);
    static void OnSimpleListUpdate(UIView* view, void* data, uint8_t type);
private:
    UISimpleList* list_ = nullptr;
    UIButton* addButton_ = nullptr;
    UIButton* delButton_ = nullptr;
};
} // OHOS
#endif