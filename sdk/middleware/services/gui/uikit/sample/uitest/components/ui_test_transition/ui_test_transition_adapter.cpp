/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionAdapter
 * Create: 2025-04
 */

#include "ui_test_transition_adapter.h"
#include "ui_test_transition_itemview.h"

namespace OHOS {
    UITestTransitionAdapter::UITestTransitionAdapter() {}

    UITestTransitionAdapter::~UITestTransitionAdapter()
    {
        listData_.clear();
    }

    uint16_t UITestTransitionAdapter::GetCount(void)
    {
        return listData_.size();
    }

    void UITestTransitionAdapter::SetData(std::list<UITestTransitionSample> &data)
    {
        if (!listData_.empty()) {
            listData_.clear();
        }
        std::list<UITestTransitionSample>::iterator it;
        UITestTransitionSample temp;
        for (it = data.begin(); it != data.end(); it++) {
            listData_.push_back(std::make_pair(*it, temp));
        }
    }

    UIView* UITestTransitionAdapter::GetView(UIView* inView, int16_t index)
    {
        if (listData_.empty()) {
            return nullptr;
        }
        if (index < 0) {
            return nullptr;
        }
        size_t uindex = index;
        if (uindex >= listData_.size()) {
            return nullptr;
        }

        auto it = listData_.begin();
        for (int i = 0; i < index; i++) {
            it++;
        }
        UITestTransitionSample leftItemData = it->first;
        UITestTransitionSample rightItemData = it->second;
        UITestTransitionItemView* item = nullptr;
        if (inView == nullptr) {
            item = new UITestTransitionItemView();
        } else {
            item = static_cast<UITestTransitionItemView* >(inView);
        }

        item->SetItemInfo(leftItemData, rightItemData);
        item->SetOnClickListener(itemClickListener_);
        return item;
    }

    void UITestTransitionAdapter::AddListItem(const UITestTransitionSample &litem, const UITestTransitionSample &ritem)
    {
        listData_.push_back(std::make_pair(litem, ritem));
    }
} // namespace OHOS
