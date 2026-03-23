/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: RollerRecentAppFragment.h
 * Author:
 * Create: 2025-07-17
 */

#ifndef ROLLER_RECENT_APP_FRAGMENT_H
#define ROLLER_RECENT_APP_FRAGMENT_H

#include <string>
#include "animator/animator.h"
#include "components/ui_fragment.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_roller_view.h"
#include "gfx_utils/vector.h"

namespace OHOS {
static constexpr const char* RECENT_APP_DELETE_BTN_ID = "RecentAppDeleteBtn";
static std::string RECENT_ITEM_NATIVE_PREFIX = "NA_";
static std::string RECENT_ITEM_JS_PREFIX = "JS_";
static constexpr const uint8_t RECENT_ITEM_PREFIX_LEN = 3;

class RollerRecentAppFragment : public UIFragment, public UIView::OnClickListener {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    void LoadRecentAppContent();
    void InitRecentApps();
    void AddRecentAppToRoller();

    UILabel* noRecentAppLabel_ = nullptr;
    UIRollerView* rollerContainer_ = nullptr;
    UILabelButton* delBtn_ = nullptr;
};
} // namespace OHOS
#endif // ROLLER_RECENT_APP_FRAGMENT_H