/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list item view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_LIST_ITEM_VIEW_H
#define ALIPAY_LIST_ITEM_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"

#include "UiConfig.h"
#include "alipay/AlipayList/AlipayListItem.h"
#include "alipay/AlipayList/AlipayListView.h"

namespace OHOS {
class AlipayListItemView : public UIViewGroup {
public:
    explicit AlipayListItemView();
    ~AlipayListItemView() override;
    void SetItemInfo(const AlipayListItem &itemInfo);
    void SetUpChild();

private:
    UIImageView sIcon;
    UILabel sLabel;
    UIImageView sNextImage;
    AlipayListItem sItemInfo;
};
}

#endif
