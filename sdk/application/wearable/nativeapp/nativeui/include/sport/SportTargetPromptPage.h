/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetPromptPage
 * Create: 2025-06-06
 */

#ifndef SPORT_TARGET_PROMPT_PAGE_H
#define SPORT_TARGET_PROMPT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {

class SportTargetPromptPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportTargetPromptPage() {}
    ~SportTargetPromptPage();
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreatePromptView(void);
    class SportPromptAnimatorCallback : public AnimatorCallback {
    public:
        SportPromptAnimatorCallback(SportTargetPromptPage* page):page_(page) {}
        virtual ~SportPromptAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        SportTargetPromptPage* page_{nullptr};
    };
    SportPromptAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UIImageView *promptIcon_{nullptr};
    UILabel *promptLabel_{nullptr};
    time_t startTime_{0};
};
}  // namespace OHOS
#endif  // SPORT_TARGET_PROMPT_PAGE_H