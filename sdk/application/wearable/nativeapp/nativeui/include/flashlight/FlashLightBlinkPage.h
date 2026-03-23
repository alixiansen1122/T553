/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightBlinkPage
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_BLINK_PAGE_H
#define FLASHLIGHT_BLINK_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_label_button_ext.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "FlashLightPresenter.h"
#include "components/ui_picker.h"

namespace OHOS {
class FlashLightBlinkPage : public SlicePage<FlashLightPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    FlashLightBlinkPage();
    ~FlashLightBlinkPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    FlashLightBlinkPage(const FlashLightBlinkPage &) = delete;
    FlashLightBlinkPage &operator=(const FlashLightBlinkPage &) = delete;
    FlashLightBlinkPage(FlashLightBlinkPage &&) = delete;
    FlashLightBlinkPage &operator=(FlashLightBlinkPage &&) = delete;

    UIScrollView *container_ = nullptr;
    UILabel *blinkLabel_ = nullptr;
    UIPicker* blinkPicker_ = nullptr;
    UILabelButtonExt* blinkButton_ = nullptr;
};
}  // namespace OHOS
#endif /* FLASHLIGHT_BLINK_PAGE_H */
