/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: EmptyPage
 * Create: 2025-04-24
 */
#ifndef EMPTY_PAGE_H
#define EMPTY_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "SlicePage.h"
#include "EmptyPresenter.h"

namespace OHOS {

class EmptyPage : public SlicePage<EmptyPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    EmptyPage();
    ~EmptyPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;

private:
    EmptyPage(const EmptyPage &) = delete;
    EmptyPage &operator=(const EmptyPage &) = delete;
    EmptyPage(EmptyPage &&) = delete;
    EmptyPage &operator=(EmptyPage &&) = delete;

    UIViewGroup *container_ = nullptr;
    UIImageView *bgImg_{nullptr};
    UILabel *memUsedLabel_ = nullptr;
    UILabel *memFreeLabel_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif
#if !defined(_WIN32)
uint32_t test_get_mem_info(uint32_t *used, uint32_t *free);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS
#endif /* EMPTY_PAGE_H */
