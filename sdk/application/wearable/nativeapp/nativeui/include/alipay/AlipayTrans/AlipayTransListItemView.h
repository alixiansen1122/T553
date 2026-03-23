/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list item view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_LIST_ITEM_VIEW_H
#define ALIPAY_TRANS_LIST_ITEM_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"

#include "UiConfig.h"
#include "alipay/AlipayTrans/AlipayTransListItem.h"
#include "alipay/AlipayTrans/AlipayTransListView.h"

namespace OHOS {
class AlipayTransListItemView : public UIViewGroup {
public:
    explicit AlipayTransListItemView();
    ~AlipayTransListItemView() override;
    void SetItemInfo(const AlipayTransListItem &itemInfo);
    void SetUpChild();
    uint8_t GetAlipayTransListIndex();

private:
    UILabel sLabel;
    UIImageView sNextImage;
    AlipayTransListItem sItemInfo;
};
}

#endif
