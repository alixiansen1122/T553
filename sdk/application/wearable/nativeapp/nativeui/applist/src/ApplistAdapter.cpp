/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistAdapter.cpp
 * Author:
 * Create: 2021-09-18
 */

#include "applist/ApplistAdapter.h"

namespace OHOS {
ApplistAdapter::ApplistAdapter() {}

ApplistAdapter::~ApplistAdapter()
{
    listData_.clear();
}

uint16 ApplistAdapter::GetCount(void)
{
    return listData_.size();
}

void ApplistAdapter::ClearItem(void)
{
    listData_.clear();
}

UIView *ApplistAdapter::GetView(UIView *inView, int16 index)
{
    if (index >= (int16)listData_.size() || index < 0) {
        return nullptr;
    }

    std::list<AppItem>::iterator it = listData_.begin();
    for (int i = 0; i < index; i++) {
        it++;
    }
    AppItem itemData = *it;

    AppItemView *item = nullptr;
    if (inView == nullptr) {
        item = new AppItemView();
        if (item == nullptr) {
            return nullptr;
        }
    } else {
        item = static_cast<AppItemView *>(inView);
    }

    item->SetItemInfo(itemData);
    item->SetOnClickListener(itemClickListener_);
    return item;
}

void ApplistAdapter::AddListItem(const AppItem &item)
{
    listData_.push_back(item);
}
} // namespace OHOS
