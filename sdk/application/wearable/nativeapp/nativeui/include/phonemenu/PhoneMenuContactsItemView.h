/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuContactsItemView
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_CONTACTS_ITEM_VIEW_H
#define PHONE_MENU_CONTACTS_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    std::string cid;
    std::string name;
    std::string phoneNumber;
} PhoneMenuContactsItem;

class PhoneMenuContactsItemView : public UIViewGroup {
public:
    PhoneMenuContactsItemView();
    ~PhoneMenuContactsItemView() override;
    virtual void SetItemInfo(const PhoneMenuContactsItem &itemInfo);
    const char *GetContactsName() const;
    const char *GetContactsCid() const;
    const char *GetContactsPhoneNumber() const;

private:
    UIViewGroup bg_;
    UIImageView icon_;
    UILabel label_;
    PhoneMenuContactsItem itemInfo_;
};
}
#endif  // PHONE_MENU_CONTACTS_ITEM_VIEW_H