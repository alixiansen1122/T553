/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallerLogPage
 * Create: 2025-06-06
 */

#ifndef PHONE_MENU_CALLER_LOG_PAGE_H
#define PHONE_MENU_CALLER_LOG_PAGE_H

#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_simple_list.h"
#include "phoneservice/PhoneService.h"
#include "SlicePage.h"
#include "PhoneMenuPresenter.h"

namespace OHOS {
class PhoneMenuCallerLogPage : public SlicePage<PhoneMenuPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    PhoneMenuCallerLogPage();
    ~PhoneMenuCallerLogPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    UIScrollView *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UILabel *noCallLabel_{nullptr};
    UILabelButton *clearButton_{nullptr};
    UISimpleList *callerLogList_{nullptr};
    CallLogInfo *info_{nullptr};
};
}  // namespace OHOS
#endif  // PHONE_MENU_CALLER_LOG_PAGE_H