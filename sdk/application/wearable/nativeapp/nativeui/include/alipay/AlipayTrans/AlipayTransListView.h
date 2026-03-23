/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_LIST_H
#define ALIPAY_TRANS_LIST_H

#include <cstdint>
#include <map>
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_list.h"
#include "components/ui_button.h"
#include "components/icon_adapter.h"
#include "alipay/AlipayPresenter.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "alipay/AlipayTrans/AlipayTransListAdapter.h"
#include "alipay/AlipayTrans/AlipayTransListItem.h"

namespace OHOS {

/* alipay sdk 支持最多20张公交卡 */
static const uint8_t ALIPAY_TRANS_LIST_ITEM_LEN = 20;

class AlipayTransListAdapter;
class AlipayTransList : public UIViewGroup {
public:
    explicit AlipayTransList(AlipayPresenter *presenter);
    ~AlipayTransList() override;
    bool InitListView();
    void AlipayTransListUpdate();

private:
    void InitAlipayTransListItem();
    bool AlipayInitList();
    bool AlipayInitButton();
    bool AlipayInitScroll(void);
    void AddAlipayTransListItemToList(void);
    AlipayPresenter *presenter_;
    AlipayTransListAdapter *listAdapter { nullptr };
    UIList *contentList { nullptr };
    AlipayTransListItem alipayTransLists[ALIPAY_TRANS_LIST_ITEM_LEN];
    uint8_t listLen = 0;
    UILabelButton *buttonRefresh { nullptr };
};
}
#endif