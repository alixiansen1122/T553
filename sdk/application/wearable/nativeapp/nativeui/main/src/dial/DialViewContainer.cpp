/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialViewContainer
 * Author:
 * Create: 2025-02-06
 */

#include "main/dial/DialViewContainer.h"
#include "main/dial/DialDigitalImgView.h"
#include "main/dial/ModelDialDataFactory.h"

namespace OHOS {
DialViewContainer::~DialViewContainer()
{
    ListNode<SubView>* viewNode = dialViews_.Head();
    while (viewNode != dialViews_.End()) {
        UIViewGroup::Remove(viewNode->data_.view);
        delete viewNode->data_.view;
        viewNode = viewNode->next_;
    }
    dialViews_.Clear();
}

void DialViewContainer::AddView(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    Add(view);
    SubView info = {
        .view = view,
        .dialView = nullptr,
    };
    DialViewContainer *container = dynamic_cast<DialViewContainer*>(view);
    if (container != nullptr && container->HasInvalidBindData()) {
        hasInvalidBindData_ = true;
    }
    dialViews_.PushBack(info);
}

void DialViewContainer::AddView(DialDataType type, DialView* view, bool isPeriodic)
{
    if (view == nullptr) {
        return;
    }
    UIView *uiView = dynamic_cast<UIView*>(view);
    if (uiView == nullptr) {
        return;
    }
    Add(uiView);
    DialDigitalImgView *digital = dynamic_cast<DialDigitalImgView*>(view);
    if (digital != nullptr) {
        digital->SetParent(this);
    }
    SubView info = {
        .view = uiView,
        .dialView = view,
        .type = type,
        .isPeriodic = isPeriodic
    };
    if (ModelDialDataFactory::GetInstance().GetModuleByType(type) == nullptr) {
        hasInvalidBindData_ = true;
    }
    dialViews_.PushBack(info);
}

bool DialViewContainer::RemoveSubView(uint8_t index, SubView &subView)
{
    if (index >= dialViews_.Size()) {
        return false;
    }
    ListNode<SubView> *it = Begin();
    for (uint8_t i = 0; i < index; ++i) {
        it = it->next_;
    }
    subView = it->data_;
    Remove(subView.view);
    DialDigitalImgView *digital = dynamic_cast<DialDigitalImgView*>(subView.view);
    if (digital != nullptr) {
        digital->SetParent(nullptr);
    }
    dialViews_.Remove(it);
    return true;
}

bool DialViewContainer::IsInvalidateBindData(uint8_t index)
{
    if (index >= dialViews_.Size()) {
        return false;
    }
    ListNode<SubView> *it = Begin();
    for (uint8_t i = 0; i < index; ++i) {
        it = it->next_;
    }
    if (it->data_.dialView != nullptr &&
        ModelDialDataFactory::GetInstance().GetModuleByType(it->data_.type) == nullptr) {
        return true;
    }
    DialViewContainer *container = dynamic_cast<DialViewContainer*>(it->data_.view);
    if (container != nullptr) {
        return container->HasInvalidBindData();
    }
    return false;
}
} // namespace OHOS