/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportPausePage
 * Create: 2025-06-06
 */

#ifndef SPORT_PAUSE_PAGE_H
#define SPORT_PAUSE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {
const char* SPORT_PAUSE_CONTINUE_BUTTON = "sportPauseContinueButton";
const char* SPORT_PAUSE_FINISH_BUTTON = "sportPauseFinishButton";

class SportPausePage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportPausePage() {}
    ~SportPausePage();
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    class SportPauseAnimatorCallback : public AnimatorCallback {
    public:
        SportPauseAnimatorCallback() {}
        virtual ~SportPauseAnimatorCallback() override {}
        void Callback(UIView* view) override;
    };
    SportPauseAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UILabel *timeLabel_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabelButton *continueButton_{nullptr};
    UILabelButton *finishButton_{nullptr};
};
}  // namespace OHOS
#endif  // SPORT_PAUSE_PAGE_H