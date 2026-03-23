/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightRecoverPage
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_RECOVER_PAGE_H
#define FLASHLIGHT_RECOVER_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_label_button_ext.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "FlashLightPresenter.h"

namespace OHOS {
class FlashLightRecoverPage : public SlicePage<FlashLightPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    FlashLightRecoverPage();
    ~FlashLightRecoverPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    FlashLightRecoverPage(const FlashLightRecoverPage &) = delete;
    FlashLightRecoverPage &operator=(const FlashLightRecoverPage &) = delete;
    FlashLightRecoverPage(FlashLightRecoverPage &&) = delete;
    FlashLightRecoverPage &operator=(FlashLightRecoverPage &&) = delete;

    UIScrollView *container_ = nullptr;
    UILabel *recoverLabel_ = nullptr;
    UILabelButtonExt *cancelButton_ = nullptr;
    UILabelButtonExt *confirmButton_ = nullptr;
};
}  // namespace OHOS
#endif /* FLASHLIGHT_RECOVER_PAGE_H */
