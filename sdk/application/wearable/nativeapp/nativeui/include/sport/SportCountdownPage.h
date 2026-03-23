/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportCountdownPage
 * Create: 2025-06-06
 */

#ifndef SPORT_COUNTDOWN_PAGE_H
#define SPORT_COUNTDOWN_PAGE_H

#include "animator/animator.h"
#include "components/ui_label.h"
#include "components/ui_circle_progress.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {
class SportCountdownPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportCountdownPage() {}
    ~SportCountdownPage();
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void UpdateCountdownView(uint64_t time);
    class SportCountdownAnimatorCallback : public AnimatorCallback {
    public:
        SportCountdownAnimatorCallback(SportCountdownPage* page):page_(page) {}
        virtual ~SportCountdownAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        SportCountdownPage* page_{nullptr};
    };
    Animator* animator_{nullptr};
    SportCountdownAnimatorCallback* callback_{nullptr};
    UIViewGroup *group_{nullptr};
    UILabel *countdownText_{nullptr};
    UICircleProgress *countdownProgress_{nullptr};
    struct timeval startTime_{0};
};
}  // namespace OHOS
#endif  // SPORT_COUNTDOWN_PAGE_H