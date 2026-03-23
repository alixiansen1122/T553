/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list.
 * Author:
 * Create:
 */

#ifndef ALIPAY_LIST_ITEM_H
#define ALIPAY_LIST_ITEM_H

#include <cstdint>
#include <string>

namespace OHOS {
/*
 * alipayList id.
 */
typedef enum : uint16_t {
    // alipayList item index
    ALIPAY_LIST_ITEM_PAY,
    ALIPAY_LIST_ITEM_TRANS,
    ALIPAY_LIST_ITEM_SETTING,
    ALIPAY_LIST_ITEM_HELP,
    ALIPAY_LIST_ITEM_MAX
} AlipayListIndex;

typedef struct {
    AlipayListIndex index;
    const char *viewId;
    const char *icon;
    const char *label;
    bool hasSecondMenu;
} AlipayListItem;
}
#endif