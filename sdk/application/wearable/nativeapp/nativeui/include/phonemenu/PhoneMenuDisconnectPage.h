/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuDisconnectPage
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_DISCONNECT_PAGE_H
#define PHONE_MENU_DISCONNECT_PAGE_H

#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "View.h"
#include "UiConfig.h"
#include "SlicePage.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "PhoneMenuPresenter.h"

namespace OHOS {
class PhoneMenuDisconnectPage : public SlicePage<PhoneMenuPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    PhoneMenuDisconnectPage();
    ~PhoneMenuDisconnectPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIScrollView *container_{nullptr};
    UIImageView *image_{nullptr};
    UILabel *text_{nullptr};
};
}
#endif