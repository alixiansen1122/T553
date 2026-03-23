/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list adapt.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_LIST_ADAPTER_H
#define ALIPAY_TRANS_LIST_ADAPTER_H

#include <list>
#include <string>
#include "components/ui_list.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/abstract_adapter.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "alipay/AlipayTrans/AlipayTransListItem.h"

namespace OHOS {
class AlipayTransListAdapter : public AbstractAdapter {
public:
    explicit AlipayTransListAdapter();
    ~AlipayTransListAdapter() override;

    /* *
     * @brief Sets the UIView adapter data, which is a AlipayTransListItem linked list.
     *
     * @param data Indicates the AlipayTransListItem linked list data.
     * @since 1.0
     * @version 1.0
     */
    void SetData(std::list<AlipayTransListItem> &data);

    /* *
     * @brief Obtains the <b>data</b> size of the UILabel adapter.
     *
     * @return Returns the <b>data</b> size of the UILabel adapter.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetCount(void) override;

    UIView *GetView(UIView *inView, int16_t index) override;

    void AddListItem(const AlipayTransListItem &item);

    void SetItemClickListener(UIView::OnClickListener *itemclicklistener)
    {
        itemClickListener = itemclicklistener;
    }

    void ClearItem(void);

private:
    std::list<AlipayTransListItem> listData;
    UIView::OnClickListener *itemClickListener { nullptr };
};
}

#endif