/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UISAMPLE_MAIN_PAGE_H
#define UISAMPLE_MAIN_PAGE_H

#include <sys/time.h>
#include "ChangeSliceListener.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "components/ui_simple_list.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "ui_resource_timer.h"
#include "uisample/UISamplePresenter.h"
#include "ui_test_group.h"
#include "View.h"

namespace OHOS {
class UISampleMainPage : public SlicePage<UISamplePresenter>, public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    UISampleMainPage();
    ~UISampleMainPage() override;
    static UISampleMainPage* GetInstance(void);

    void OnStart(void *data) override;;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
private:
    void CreateTitle();
    void CreateMainList();

    UIScrollView* container_ = nullptr;
    UILabel *title_ = nullptr;
    UISimpleList* mainList_ = nullptr;
};
} // OHOS
#endif // UISAMPLE_MAIN_PAGE_H