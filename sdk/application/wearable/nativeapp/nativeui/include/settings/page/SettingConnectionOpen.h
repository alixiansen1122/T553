/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingConnectionOpen
 * Created: 2025-06-05
 */
#ifndef SETTING_CONNECTION_OPEN_H
#define SETTING_CONNECTION_OPEN_H

#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_fragment.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingPresenter.h"

namespace OHOS {
class SettingConnectionOpen : public UIFragment,
							  public UIView::OnClickListener,
                              public UIView::OnDragListener {
public:
    void RefreshFragment();
    bool OnClick(UIView &view, const ClickEvent &event);
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    void InitConnectionOpenFragment();
    UIImageView* img_ = nullptr;
    UILabel* watchNameLabel_ = nullptr;
    UILabel* connectStatusLabel_ = nullptr;
    UILabelButton* confirmButton_ = nullptr;
};
} // OHOS
#endif