/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingConnectBlueFail
 * Created: 2025-06-05
 */
#ifndef SETTING_CONNECT_BLUE_FAIL_H
#define SETTING_CONNECT_BLUE_FAIL_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_fragment.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingConnectBlueFail : public UIFragment,
                               public UIView::OnClickListener,
                               public UIView::OnDragListener {
public:
    void RefreshFragment();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    void InitFailFragment();
    UIImageView* img_ = nullptr;
    UILabel* watchNameLabel_ = nullptr;
    UILabel* connectStatusLabel_ = nullptr;
    UILabelButton* confirmButton_ = nullptr;
};
} // OHOS
#endif