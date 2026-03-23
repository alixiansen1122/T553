/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: SlipFlowRecentAppFragment.h
 * Author:
 * Create: 2025-07-17
 */

#ifndef SLIPFLOW_RECENT_APP_FRAGMENT_H
#define SLIPFLOW_RECENT_APP_FRAGMENT_H

#include <string>
#include "animator/animator.h"
#include "components/ui_fragment.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_slipflow_view.h"
#include "gfx_utils/vector.h"

namespace OHOS {
static std::string RECENT_ITEM_NATIVE_PREFIX = "NA_";
static std::string RECENT_ITEM_JS_PREFIX = "JS_";
static constexpr const uint8_t RECENT_ITEM_PREFIX_LEN = 3;

class SlipFlowRecentAppFragment : public UIFragment, public UIView::OnClickListener,
                                  public UISlipflowView::OnSlipflowScrollListener {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void OnScrollUpStart() override;
    void OnScrollUpEnd() override;
    bool OnRemove(UIView* view) override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    void LoadRecentAppContent();
    void InitRecentApps();
    void AddRecentAppToSlipFlow();
    void AddImageToSlipFlow(int index);

    UILabel* noRecentAppLabel_ = nullptr;
    UISlipflowView* slipFlowContainer_ = nullptr;
    UILabel* slipUpNoticeLabel_ = nullptr;
    Graphic::Vector<std::string> appNames_;
    Graphic::Vector<ImageInfo*> appImgInfos_;
};
} // namespace OHOS
#endif // SLIPFLOW_RECENT_APP_FRAGMENT_H