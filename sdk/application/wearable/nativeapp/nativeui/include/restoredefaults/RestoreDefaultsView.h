/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: Restore Defaults View
 * Create: 2022-01-19
 */

#ifndef RESTORE_DEFAULTS_VIEW_H
#define RESTORE_DEFAULTS_VIEW_H

#include "View.h"
#include "restoredefaults/RestoreDefaultsPresenter.h"
#include "restoredefaults/RestoreSelectView.h"
#include "main/AppGroupView.h"

namespace OHOS {
constexpr int16_t NUM_RESTORE_DEFAULTS_CONTAINER = 1;

class RestoreDefaultsPresenter;
class RestoreDefaultsView : public View<RestoreDefaultsPresenter> {
public:
    RestoreDefaultsView();
    ~RestoreDefaultsView() override;
    static RestoreDefaultsView *GetInstance();
    void OnStart() override;
    void ChangeView(int16_t hideView, int16_t showView);

private:
    bool ChangeRestoreSelectView(int16_t hideView, int16_t thisView);
    bool CheckViewParam(int16_t hideView, int16_t showView);
    RestoreSelectView *restoreSelectView { nullptr };
    UIViewGroup *container[NUM_RESTORE_DEFAULTS_CONTAINER] { nullptr };
    typedef bool (RestoreDefaultsView::*PreprosssChangeView)(int16_t hideView, int16_t thisView);
    PreprosssChangeView preProcessView[NUM_RESTORE_DEFAULTS_CONTAINER];
};
} // namespace OHOS
#endif // RESTORE_DEFAULTS_VIEW_H