/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathStartPage
 * Create: 2025-5-13
 */
#ifndef BREATH_START_PAGE_H
#define BREATH_START_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "View.h"
#include "power_display_service.h"
#include "SlicePage.h"
#include "graphic_timer.h"
#include "SlicePageFactory.h"
#include "BreathModel.h"
#include "BreathPresenter.h"
#ifndef _WIN32
#include "time64.h"
#endif

namespace OHOS {
class BreathStartPage : public SlicePage<BreathPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BreathStartPage();
    ~BreathStartPage() override;
    void OnStart(void* data) override;
    void OnTimer();
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void OnPause() override;
private:
    UILabel *titleLable_ = nullptr;
    UILabel *guideLable_ = nullptr;
    UIImageView *centerImg_ = nullptr;
    UIViewGroup *group_ = nullptr;
    UIImageView *bgImg_ = nullptr;

    class BreathStartCallback : public AnimatorCallback {
    public:
        BreathStartCallback(BreathStartPage* page) : BreathStartPage(page) {}
        virtual ~BreathStartCallback() override {}
        void Callback(UIView* view) override;
    private:
        BreathStartPage* BreathStartPage{nullptr};
    };
    BreathStartCallback* animatorCallBack_{nullptr};
    Animator *animator_ = nullptr;
};
}  // namespace OHOS

#endif