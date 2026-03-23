/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistView.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef APPLIST_VIEW_H
#define APPLIST_VIEW_H

#include "View.h"
#include "UIDesktopFragment.h"

namespace OHOS {
class ApplistPresenter;
class ApplistView : public View<ApplistPresenter> {
public:
    ApplistView() {};
    ~ApplistView() override {};

    void OnStart() override;
    void OnStop() override;
    void OnResume();
    void AddAppItemToList(const AppItem &item);
    void ClearAppItemToList();
    void RefreshAppList();

private:
    UIDesktopFragment *desktopFragment_ = nullptr;
};
}
#endif // APPLIST_VIEW_H
