/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: Restore Defaults Presenter
 * Create: 2022-01-19
 */

#ifndef RESTORE_DEFAULTS_PRESENTER_H
#define RESTORE_DEFAULTS_PRESENTER_H

#include "Presenter.h"
#include "NativeLauncher.h"
#include "NativeAbility.h"

namespace OHOS {
enum RestoreDefaultsPresenterNum : uint8_t {
    RESTORESELECTVIEW = 0,
    RESTOREDEFAULTS_MAX,
};

#define RESTORE_CANCEL_BUTTON "restoreCancelButton"
#define RESTORE_CONFIRM_BUTTON "restoreConfirmButton"

#define RESTORE_SELECT_VIEW "restoreSelectView"
#define RESTORE_OPEN_VIEW "restoreOpenView"

class RestoreDefaultsView;
class RestoreDefaultsPresenter : public Presenter<RestoreDefaultsView>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    RestoreDefaultsPresenter();
    ~RestoreDefaultsPresenter();
    static RestoreDefaultsPresenter *GetInstance();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void ConfirmOnClick();

private:
};
} // namespace OHOS
#endif