/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ace scale-list
 * Author: SoftwarePlatform Group
 * Create: 2025-08-8
 */

#ifndef OHOS_ACELITE_LIST_EXT_COMPONENT_H
#define OHOS_ACELITE_LIST_EXT_COMPONENT_H

#include "component.h"
#include "list_adapter.h"
#include "ui_transform_list.h"

namespace OHOS {
namespace ACELite {
class ListExtComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ListExtComponent);
    ListExtComponent() = delete;
    ListExtComponent(JSValue options, JSValue children, AppStyleManager *styleManager);
    ~ListExtComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    UIView *GetComponentRootView() const override;
    bool ApplyPrivateStyle(const AppStyleItem* style) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, JSValue funcValue, bool isStopPropagation) override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool ProcessChildren() override;
    void OnViewAttached() override;
    bool UpdateForView() override;

private:
    UITransformList list_;
    ListAdapter adapter_;
    ListEventListener listEventListener_;
    static const char * const FUNC_SCROLLTO;
    static const char * const INDEX_SCROLLTO;

    int16_t scaleFactor_;
    UIView::ViewExtraMsg scaleMsg_;
    static bool ScaleListItem(UITransformList* list, UIView* item);

    static const char * const FUNC_SCROLLTO_END;
    static JSValue ListScrollTo(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
    static JSValue ListScrollToEnd(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
    void ComputeListLength();
    static JSValue ListScroll(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_LIST_EXT_COMPONENT_H
