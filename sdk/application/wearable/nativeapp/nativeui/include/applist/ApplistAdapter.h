/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistAdapter.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef APPLIST_ADAPTER_H
#define APPLIST_ADAPTER_H

#include <list>
#include <string>
#include "components/ui_list.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/abstract_adapter.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "applist/AppItemView.h"
#include "applist/ApplistModel.h"

namespace OHOS {
class ApplistAdapter : public AbstractAdapter {
public:
    explicit ApplistAdapter();
    ~ApplistAdapter() override;

    /* *
     * @brief Obtains the <b>data</b> size of the UILabel adapter.
     *
     * @return Returns the <b>data</b> size of the UILabel adapter.
     * @since 1.0
     * @version 1.0
     */
    uint16 GetCount(void) override;

    UIView *GetView(UIView *inView, int16 index) override;

    void AddListItem(const AppItem &item);

    void SetItemClickListener(UIView::OnClickListener *itemClickListener)
    {
        itemClickListener_ = itemClickListener;
    }

    void ClearItem(void);

private:
    std::list<AppItem> listData_;
    UIView::OnClickListener *itemClickListener_;
};
}
#endif