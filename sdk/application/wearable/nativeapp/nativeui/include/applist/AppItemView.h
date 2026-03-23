/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AppItemView.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef APP_ITEM_VIEW_H
#define APP_ITEM_VIEW_H

#include "components/ui_label_ext.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "applist/ApplistModel.h"
#include "UiConfig.h"

namespace OHOS {
class AppItemView : public UIViewGroup {
public:
    explicit AppItemView();
    ~AppItemView() override;
    void SetItemInfo(const AppItem &itemInfo);
    AppViewId GetViewId();
    const char *GetAppUid();
    void SetUpChild();
    void SelectedChange();
    void Reset();
private:
    AppItemView(const AppItemView &);
    AppItemView &operator = (const AppItemView &);

    UIImageView icon_;
    UILabelExt label_;
    AppItem itemInfo_;
};
}

#endif
