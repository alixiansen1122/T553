/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: Restore Defaults View
 * Create: 2022-03-22
 */

#ifndef RESTORE_SELECT_VIEW_H
#define RESTORE_SELECT_VIEW_H

#include "View.h"
#include "restoredefaults/RestoreDefaultsPresenter.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_view_group.h"

namespace OHOS {
class RestoreSelectView {
public:
    RestoreSelectView();
    ~RestoreSelectView();
    static RestoreSelectView *GetInstance();
    UIViewGroup *InitShowUpView(void);

private:
    void ShowUpTitleLabel(void);
    void CreateUIButton(void);
    UILabel *titleLabel{nullptr};
    UIButton *cancelButton{nullptr};
    UIButton *confirmButton{nullptr};
    UIViewGroup *group{nullptr};
};
} // namespace OHOS
#endif // RESTORE_SELECT_VIEW_H