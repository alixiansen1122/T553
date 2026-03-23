/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingConnectNewPhone
 * Created: 2025-06-05
 */
#ifndef SETTING_CONNECT_NEW_PHONE_H
#define SETTING_CONNECT_NEW_PHONE_H

#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_fragment.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingPresenter.h"
#include "settings/page/SettingConnectionOpen.h"

namespace OHOS {
class SettingConnectNewPhone : public UIFragment,
                               public UIView::OnClickListener,
                               public UIView::OnDragListener {
public:
    bool OnClick(UIView &view, const ClickEvent &event);
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    void InitNewPhoneFragment();
    UILabel* textFirst_ = nullptr;
    UILabel* textSecond_ = nullptr;
    UILabel* textThird_ = nullptr;
    UIImageView* cancel_ = nullptr;
    UIButton* confirm_ = nullptr;
};
} // OHOS
#endif