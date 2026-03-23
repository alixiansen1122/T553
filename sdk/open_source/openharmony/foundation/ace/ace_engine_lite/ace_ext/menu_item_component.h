/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ace scale-list
 * Author: SoftwarePlatform Group item
 * Create: 2025-08-8
 */

#ifndef OHOS_ACELITE_MENU_ITEM_COMPONENT_H
#define OHOS_ACELITE_MENU_ITEM_COMPONENT_H
#include "component.h"
#include "ui_transform_group.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class MenuItemComponent final : public Component {
public:

    ACE_DISALLOW_COPY_AND_MOVE(MenuItemComponent);
    MenuItemComponent() = delete;
    MenuItemComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~MenuItemComponent() override {}

protected:
    UIView *GetComponentRootView() const override;
    bool ProcessChildren() override;
    void AttachView(const Component *child) override;

private:
    UITransformGroup item_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_MENU_ITEM_COMPONENT_H
