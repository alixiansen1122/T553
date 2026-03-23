/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuMainPage
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_MAIN_PAGE_H
#define PHONE_MENU_MAIN_PAGE_H

#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "View.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "SlicePage.h"
#include "ui_resource_phonemenu.h"
#include "PhoneMenuPresenter.h"
#include "PhoneMenuMainItemView.h"

namespace OHOS {
constexpr uint8_t PHOEN_MENU_ITEM_NUM = 3;
const char *PHONE_MENU_ITEM_CONTACTS_BOOKS = "phoneMenuItemContactsBooks";
const char *PHONE_MENU_ITEM_RECENT_CALL = "phoneMenuItemRecentCall";
const char *PHONE_MENU_ITEM_DIAL = "phoneMenuItemDial";

class PhoneMenuMainPage : public SlicePage<PhoneMenuPresenter>,
                          public UIView::OnClickListener,
                          public UIView::OnDragListener {
public:
    PhoneMenuMainPage();
    ~PhoneMenuMainPage() override;
    void OnStart(void *data) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;

private:
    UIScrollView *container_ = nullptr;
    UILabel *titleLabel_ = nullptr;
    UISimpleList *phoneMenuList_ = nullptr;
    PhoneMenuItem phoneMenu_[PHOEN_MENU_ITEM_NUM] = {
        {IMAGE_PHONEMENU_CONTACTS_BOOKS, "通讯录", PHONE_MENU_ITEM_CONTACTS_BOOKS},
        {IMAGE_PHONEMENU_RECENT_CALL, "最近通话", PHONE_MENU_ITEM_RECENT_CALL},
        {IMAGE_PHONEMENU_DIAL, "拨号", PHONE_MENU_ITEM_DIAL},
    };
};
}  // namespace OHOS
#endif  // PHONE_MENU_MAIN_PAGE_H
