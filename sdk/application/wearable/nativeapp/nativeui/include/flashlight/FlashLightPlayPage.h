/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightPlayPage
 * Create: 2025-04-24
 */
#ifndef FLASHLIGHT_PLAY_PAGE_H
#define FLASHLIGHT_PLAY_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_image_view.h"
#include "components/ui_image_animator.h"
#include "SlicePage.h"
#include "FlashLightPresenter.h"
#include "FlashLightModel.h"

namespace OHOS {
static constexpr uint8_t IMAGE_NUM = 2;

class FlashLightPlayPage : public SlicePage<FlashLightPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    FlashLightPlayPage();
    ~FlashLightPlayPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    FlashLightPlayPage(const FlashLightPlayPage &) = delete;
    FlashLightPlayPage &operator=(const FlashLightPlayPage &) = delete;
    FlashLightPlayPage(FlashLightPlayPage &&) = delete;
    FlashLightPlayPage &operator=(FlashLightPlayPage &&) = delete;

    UIScrollView *container_ = nullptr;
    UIImageAnimatorView *playImageAnimal_ = nullptr;
    ImageAnimatorInfo playImageInfo_[IMAGE_NUM];
};
}  // namespace OHOS
#endif /* FLASHLIGHT_PLAY_PAGE_H */
