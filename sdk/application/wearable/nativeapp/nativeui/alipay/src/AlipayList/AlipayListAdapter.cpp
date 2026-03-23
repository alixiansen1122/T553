/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list adapt.
 * Author:
 * Create:
 */

#include "alipay/AlipayList/AlipayListAdapter.h"
#include "alipay/AlipayList/AlipayListItemView.h"

namespace OHOS {
AlipayListAdapter::AlipayListAdapter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListAdapter::AlipayListAdapter()");
}

AlipayListAdapter::~AlipayListAdapter()
{
    listData.clear();
}

uint16_t AlipayListAdapter::GetCount(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListAdapter::GetCount(), size:%u", listData.size());
    return listData.size();
}

void AlipayListAdapter::SetData(std::list<AlipayListItem> &data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListAdapter::SetData()");
    if (!listData.empty()) {
        listData.clear();
    }
    std::list<AlipayListItem>::iterator it;
    for (it = data.begin(); it != data.end(); it++) {
        listData.push_back(*it);
    }
}

UIView *AlipayListAdapter::GetView(UIView *inView, int16_t index)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListAdapter::GetView(), addr %x, index:%u", inView, index);
    if (listData.empty()) {
        return nullptr;
    }
    if (index < 0) {
        return nullptr;
    }
    size_t uindex = index;
    if (uindex >= listData.size()) {
        return nullptr;
    }

    std::list<AlipayListItem>::iterator it = listData.begin();
    for (int i = 0; i < index; i++) {
        it++;
    }
    AlipayListItem itemData = *it;
    AlipayListItemView *item = nullptr;
    if (inView == nullptr) {
        item = new AlipayListItemView();
    } else {
        item = static_cast<AlipayListItemView *>(inView);
    }

    item->SetItemInfo(itemData);
    item->SetViewId(itemData.viewId);
    item->SetOnClickListener(itemClickListener);
    return item;
}

void AlipayListAdapter::AddListItem(const AlipayListItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListAdapter::AddListItem()");
    listData.push_back(item);
}
} // namespace OHOS
