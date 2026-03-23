/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistPresenter.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef APPLIST_PRESENTER_H
#define APPLIST_PRESENTER_H

#include "Presenter.h"
#include "ApplistView.h"
#include "ApplistModel.h"

namespace OHOS {
class ApplistPresenter : public Presenter<ApplistView> {
public:
    ApplistPresenter();
    ~ApplistPresenter() override;
    void OnStart() override;
    void OnResume() override;
    ApplistView* GetAppListView();

private:
    ApplistModel *model{nullptr};
};
}

#endif // APPLIST_PRESENTER_H
