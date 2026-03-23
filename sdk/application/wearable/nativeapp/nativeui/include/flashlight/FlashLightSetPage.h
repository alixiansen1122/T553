/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightSetPage
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_SET_PAGE_H
#define FLASHLIGHT_SET_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_label_button_ext.h"
#include "components/ui_scroll_view.h"
#include "components/ui_simple_list.h"
#include "SlicePage.h"
#include "FlashLightPresenter.h"
#include "FlashLightSetItemView.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_flashlight.h"

namespace OHOS {
class FlashLightSetPage : public SlicePage<FlashLightPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    FlashLightSetPage();
    ~FlashLightSetPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    FlashLightSetPage(const FlashLightSetPage &) = delete;
    FlashLightSetPage &operator=(const FlashLightSetPage &) = delete;
    FlashLightSetPage(FlashLightSetPage &&) = delete;
    FlashLightSetPage &operator=(FlashLightSetPage &&) = delete;

    UIScrollView *container_ = nullptr;
    UILabel *titleLabel_ = nullptr;
    UILabelButtonExt *recoverButton_ = nullptr;
    UISimpleList* setItemList_ = nullptr;
};
}  // namespace OHOS
#endif /* FLASHLIGHT_SET_PAGE_H */
