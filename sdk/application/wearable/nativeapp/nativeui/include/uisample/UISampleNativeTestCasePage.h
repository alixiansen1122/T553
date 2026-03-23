/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UISAMPLE_OPEN_HARMONY_PAGE_H
#define UISAMPLE_OPEN_HARMONY_PAGE_H

#include <sys/time.h>
#include "ChangeSliceListener.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_simple_list.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "ui_resource_timer.h"
#include "uisample/UISamplePresenter.h"
#include "ui_test_group.h"
#include "View.h"

namespace OHOS {
class UISampleNativeTestCasePage : public SlicePage<UISamplePresenter>, public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    UISampleNativeTestCasePage();
    ~UISampleNativeTestCasePage() override;
    static UISampleNativeTestCasePage *GetInstance(void);
    void OnStart(void *data) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;

private:
    void CreateTitle();
    void CreateMainList();
    void CreateBackButton();
    bool BackMenuPage(UIView& view);

    UIScrollView* container_ = nullptr;
    UILabel *title_{nullptr};
    UISimpleList* mainList_ = nullptr;
    UIButton* backBtn_{nullptr};
    bool isInTestCase_ = false;
};
} // OHOS
#endif // UISAMPLE_OPEN_HARMONY_PAGE_H