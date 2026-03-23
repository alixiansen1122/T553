/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportPromptPage
 * Create: 2025-06-06
 */

#ifndef SPORT_PROMPT_PAGE_H
#define SPORT_PROMPT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "SportPresenter.h"

namespace OHOS {
const char* SPORT_PROMPT_CANCEL_BUTTON = "sportPromptCancelButton";
const char* SPORT_PROMPT_CONFIRM_BUTTON = "sportPromptConfirmButton";

class SportPromptPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportPromptPage() {}
    ~SportPromptPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UIImageView *promptIcon_{nullptr};
    UILabel *promptLabel_{nullptr};
    UIButton *cancelButton_{nullptr};
    UIButton *confirmButton_{nullptr};
};
}  // namespace OHOS
#endif  // SPORT_PROMPT_PAGE_H