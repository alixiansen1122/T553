/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingReconnectSecondConfirm
 * Created: 2025-06-05
 */
#ifndef SETTING_RECONNECT_SECOND_CONFIRM_H
#define SETTING_RECONNECT_SECOND_CONFIRM_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_fragment.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingPresenter.h"

namespace OHOS {
class SettingReconnectSecondConfirm : public UIFragment,
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
    void InitSecondConfirmFragment();
    UILabel* title_ = nullptr;
    UILabel* contentFirst_ = nullptr;
    UILabel* contentSecond_ = nullptr;
    UILabel* contentThird_ = nullptr;
    UIImageView* cancel_ = nullptr;
    UILabelButton* confirm_ = nullptr;
};
} // OHOS
#endif