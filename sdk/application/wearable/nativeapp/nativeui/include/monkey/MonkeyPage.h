/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MonkeyPage
 * Create: 2025-04-24
 */
#ifndef MONKEY_PAGE_H
#define MONKEY_PAGE_H

#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button_ext.h"
#include "SlicePage.h"
#include "MonkeyPresenter.h"

namespace OHOS {
enum MONKEY_PAGES {
    MONKEY_MAIN_PAGE = 1, // 0 is invalid
};

class MonkeyPage : public SlicePage<MonkeyPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    MonkeyPage();
    ~MonkeyPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    MonkeyPage(const MonkeyPage &) = delete;
    MonkeyPage &operator=(const MonkeyPage &) = delete;
    MonkeyPage(MonkeyPage &&) = delete;
    MonkeyPage &operator=(MonkeyPage &&) = delete;

    UIScrollView *container_ = nullptr;
    UILabel* monkeyLabel_ = nullptr;
    UILabelButtonExt* startMonkeyButton_ = nullptr;
    UILabelButtonExt* endMonkeyButton_ = nullptr;
};
}  // namespace OHOS
#endif /* MONKEY_PAGE_H */
