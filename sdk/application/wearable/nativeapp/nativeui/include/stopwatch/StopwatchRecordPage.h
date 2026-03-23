/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchRecordPage
 * Create: 2025-06-06
 */

#ifndef STOPWATCH_RECORD_PAGE_H
#define STOPWATCH_RECORD_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "ui_resource_stopwatch.h"
#include "StopwatchPresenter.h"
#include "StopwatchModel.h"

namespace OHOS {
class StopwatchRecordPage : public SlicePage<StopwatchPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    StopwatchRecordPage() {}
    ~StopwatchRecordPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitBackgroundView();
    void CreateUILabel(void);
    void CreateUIList(void);
    UIViewGroup *group_{nullptr};
    UIImageView *background_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *recordList_{nullptr};
};
}  // namespace OHOS
#endif  // STOPWATCH_RECORD_PAGE_H