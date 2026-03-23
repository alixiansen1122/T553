/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: UIDesktopFragment.h
 * Author:
 * Create: 2025-07-07
 */

#ifndef UI_DESKTOP_FRAGMENT_H
#define UI_DESKTOP_FRAGMENT_H

#include "applist/AppItemView.h"
#include "components/ui_fragment.h"
#include "components/ui_list.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "settings/model/SettingDesktopModel.h"
#include "TransitionType.h"
#include "ohos_types.h"
#include "wearable_log.h"

namespace OHOS {
class UIDesktopFragment : public UIFragment, public UIView::OnClickListener,
                          public ListScrollListener, public UIView::OnRotateListener {
public:
    virtual void AddAppItemToList(const AppItem &item) = 0;
    virtual void ClearAppItemToList() = 0;
    virtual void RefreshAppList() = 0;
    void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height);

protected:
    uint16_t imgSize_ = 0;
    uint16_t imgDistance_ = 0;
    void OnCreateView(void* data) override {};
    void OnDestroyView() override {};
    bool SwitchView(AppViewId viewId, char *uid, TransitionType type, DesktopStyle style);
};
}
#endif // UI_DESKTOP_FRAGMENT_H