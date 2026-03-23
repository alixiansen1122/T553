/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingReconnection
 * Created: 2025-06-05
 */
#ifndef SETTING_RECONNECTION_H
#define SETTING_RECONNECTION_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_fragment.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingPresenter.h"
#include "settings/page/SettingReconnectSecondConfirm.h"
#include "settings/page/UnPairSecondConfirm.h"

namespace OHOS {
class SettingReconnection : public UIFragment,
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
    void InitReconnectionFragment();
    UILabel* watchNameLabel_ = nullptr;
    UILabel* connectStatusLabel_ = nullptr;
    UILabelButton* reconnectionButton_ = nullptr;
    UIImageView* unpair_ = nullptr;
};
} // OHOS
#endif