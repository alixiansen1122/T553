/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuContactsPage
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_CONTACTS_PAGE_H
#define PHONE_MENU_CONTACTS_PAGE_H

#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "View.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "PhoneMenuPresenter.h"
#include "PhoneMenuContactsItemView.h"

namespace OHOS {
class PhoneMenuContactsPage : public SlicePage<PhoneMenuPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    PhoneMenuContactsPage();
    ~PhoneMenuContactsPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIScrollView* container_ = nullptr;
    UISimpleList* contactsList_ = nullptr;
    UIImageView* image_ = nullptr;
    UILabel* contactsLabel_ = nullptr;
};
}
#endif  // PHONE_MENU_CONTACTS_PAGE_H
