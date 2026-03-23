/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuDialPage
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_DIAL_PAGE_H
#define PHONE_MENU_DIAL_PAGE_H

#include <string>
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "View.h"
#include "UiConfig.h"
#include "SlicePage.h"
#include "PhoneMenuPresenter.h"

namespace OHOS {
class PhoneMenuDialPage : public SlicePage<PhoneMenuPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    PhoneMenuDialPage();
    ~PhoneMenuDialPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIScrollView* container_ = nullptr;
    UILabel* dialNumberLabel_ = nullptr;
    UILabelButton* key0Button_ = nullptr;
    UILabelButton* key1Button_ = nullptr;
    UILabelButton* key2Button_ = nullptr;
    UILabelButton* key3Button_ = nullptr;
    UILabelButton* key4Button_ = nullptr;
    UILabelButton* key5Button_ = nullptr;
    UILabelButton* key6Button_ = nullptr;
    UILabelButton* key7Button_ = nullptr;
    UILabelButton* key8Button_ = nullptr;
    UILabelButton* key9Button_ = nullptr;
    UILabelButton* deleteButton_ = nullptr;
    UILabelButton* keyStarButton_ = nullptr;
    UILabelButton* keyPoundButton_ = nullptr;
    UILabelButton* diagButton_ = nullptr;
    std::string dialNumber_;
};
}
#endif  // PHONE_MENU_DIAL_PAGE_H
