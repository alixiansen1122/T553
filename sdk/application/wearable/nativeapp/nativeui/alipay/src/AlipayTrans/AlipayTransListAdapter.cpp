/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans list adapt.
 * Author:
 * Create:
 */

#include "alipay/AlipayTrans/AlipayTransListAdapter.h"
#include "alipay/AlipayTrans/AlipayTransListItemView.h"

namespace OHOS {
AlipayTransListAdapter::AlipayTransListAdapter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListAdapter::AlipayTransListAdapter()");
}

AlipayTransListAdapter::~AlipayTransListAdapter()
{
    listData.clear();
}

uint16_t AlipayTransListAdapter::GetCount(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListAdapter::GetCount(), size:%u", listData.size());
    return listData.size();
}

void AlipayTransListAdapter::ClearItem(void)
{
    listData.clear();
}

void AlipayTransListAdapter::SetData(std::list<AlipayTransListItem> &data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListAdapter::SetData()");
    if (!listData.empty()) {
        listData.clear();
    }
    std::list<AlipayTransListItem>::iterator it;
    for (it = data.begin(); it != data.end(); it++) {
        listData.push_back(*it);
    }
}

UIView *AlipayTransListAdapter::GetView(UIView *inView, int16_t index)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListAdapter::GetView(), addr %x, index:%u", inView, index);
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

    std::list<AlipayTransListItem>::iterator it = listData.begin();
    for (int i = 0; i < index; i++) {
        it++;
    }
    AlipayTransListItem itemData = *it;
    AlipayTransListItemView *item = nullptr;
    if (inView == nullptr) {
        item = new AlipayTransListItemView();
    } else {
        item = static_cast<AlipayTransListItemView *>(inView);
    }

    item->SetItemInfo(itemData);
    item->SetViewId(itemData.viewId);
    item->SetOnClickListener(itemClickListener);
    return item;
}

void AlipayTransListAdapter::AddListItem(const AlipayTransListItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListAdapter::AddListItem()");
    listData.push_back(item);
}
} // namespace OHOS
