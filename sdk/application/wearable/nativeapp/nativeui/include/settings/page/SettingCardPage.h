/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardPage
 * Create: 2025-06-01
 */
#ifndef SETTING_CARD_PAGE_H
#define SETTING_CARD_PAGE_H

#include "components/ui_scroll_view_nested.h"
#include "components/ui_simple_list.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingCardPage : public SlicePage<SettingPresenter>,
                        public UIView::OnClickListener {
public:
    void OnStart(void* data) override;
    void OnResume() override;
    void OnStop() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
protected:
    static void OnSimpleListUpdate(UIView* view, void* data, uint8_t type);
private:
    UIScrollViewNested* scroll_ = nullptr;
    UISimpleList* content_ = nullptr;
};
} // OHOS
#endif