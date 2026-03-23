/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_LIST_ITEM_H
#define ALIPAY_TRANS_LIST_ITEM_H

#include <cstdint>

namespace OHOS {
/*
 * alipayTransList id.
 */

typedef struct {
    uint8_t index;
    char *viewId;
    char *cardNo;
    char *label;
    char *title;
} AlipayTransListItem;
}
#endif