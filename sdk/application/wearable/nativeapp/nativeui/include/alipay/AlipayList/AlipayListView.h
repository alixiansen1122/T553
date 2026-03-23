/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list.
 * Author:
 * Create:
 */

#ifndef ALIPAY_LIST_H
#define ALIPAY_LIST_H

#include <cstdint>
#include <map>
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_list.h"
#include "components/icon_adapter.h"
#include "alipay/AlipayPresenter.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "alipay/AlipayList/AlipayListAdapter.h"
#include "alipay/AlipayList/AlipayListItem.h"

namespace OHOS {
class AlipayListAdapter;
class AlipayList : public UIViewGroup {
public:
    explicit AlipayList(AlipayPresenter *presenter);
    ~AlipayList() override;
    bool InitListView();

private:
    void InitAlipayListItem();
    bool AlipayInitList();
    void AddAlipayListItemToList(void);
    AlipayPresenter *presenter_;
    AlipayListAdapter *listAdapter { nullptr };
    UIList *contentList { nullptr };
    AlipayListItem alipayLists[ALIPAY_LIST_ITEM_MAX];
};
}
#endif