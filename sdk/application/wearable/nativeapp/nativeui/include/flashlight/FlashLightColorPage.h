/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightColorPage
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_COLOR_PAGE_H
#define FLASHLIGHT_COLOR_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_label_button_ext.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "FlashLightPresenter.h"

namespace OHOS {
class FlashLightColorPage : public SlicePage<FlashLightPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    FlashLightColorPage();
    ~FlashLightColorPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    FlashLightColorPage(const FlashLightColorPage &) = delete;
    FlashLightColorPage &operator=(const FlashLightColorPage &) = delete;
    FlashLightColorPage(FlashLightColorPage &&) = delete;
    FlashLightColorPage &operator=(FlashLightColorPage &&) = delete;

    UIScrollView *container_ = nullptr;
    UILabel *colorLabel_ = nullptr;
    UILabelButtonExt *colorWhiteButton_ = nullptr;
    UILabelButtonExt *colorRedButton_ = nullptr;
    UILabelButtonExt *colorOrangeButton_ = nullptr;
    UILabelButtonExt *colorYellowButton_ = nullptr;
    UILabelButtonExt *colorGreenButton_ = nullptr;
    UILabelButtonExt *colorLightBlueButton_ = nullptr;
    UILabelButtonExt *colorDarkBlueButton_ = nullptr;
    UILabelButtonExt *colorPurpleButton_ = nullptr;
    UILabelButtonExt *colorMultiButton_ = nullptr;
};
}  // namespace OHOS
#endif /* FLASHLIGHT_COLOR_PAGE_H */
