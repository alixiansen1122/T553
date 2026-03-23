/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ace scroll view
 * Author: SoftwarePlatform Group item
 * Create: 2025-08-12
 */

#ifndef OHOS_ACELITE_SCROLL_COMPONENT_H
#define OHOS_ACELITE_SCROLL_COMPONENT_H
#include "component.h"
#include "ui_scroll_view.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class ScrollComponent final : public Component {
public:

    ACE_DISALLOW_COPY_AND_MOVE(ScrollComponent);
    ScrollComponent() = delete;
    ScrollComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~ScrollComponent() override {}

protected:
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool ProcessChildren() override;
    void AttachView(const Component *child) override;

private:
    UIScrollView scrolliew_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_SCROLL_COMPONENT_H
