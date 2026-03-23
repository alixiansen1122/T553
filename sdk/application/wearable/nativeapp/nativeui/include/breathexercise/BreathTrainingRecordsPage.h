/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathTrainingRecordsPage
 * Create: 2025-5-13
 */
#ifndef BREATH_TRAINING_PAGE_H
#define BREATH_TRAINING_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "View.h"
#include "SlicePage.h"
#include "graphic_timer.h"
#include "SlicePageFactory.h"
#include "BreathModel.h"
#include "BreathPresenter.h"

namespace OHOS {
class BreathTrainingRecordsPage : public SlicePage<BreathPresenter>,
                                  public UIView::OnDragListener {
public:
    BreathTrainingRecordsPage();
    ~BreathTrainingRecordsPage() override;
    void OnStart(void* data) override;
    void OnResume() override;
    bool OnDrag(UIView &view, const DragEvent &event) override;

private:
    UILabel *labelTitle_ = nullptr;
    UILabel *labelTimeDes_ = nullptr;
    UILabel *labelTimeUnit_ = nullptr;
    UILabel *labelTimeValue_ = nullptr;
    UILabel *labelCountDes_ = nullptr;
    UILabel *labelCountUnit_ = nullptr;
    UILabel *labelCountValue_ = nullptr;
    UIViewGroup *group_ = nullptr;
    UIImageView *bgImg1_ = nullptr;
    UIImageView *bgImg2_ = nullptr;
    UIImageView *bgImg3_ = nullptr;
    UIImageView *bgImg4_ = nullptr;
};
}  // namespace OHOS

#endif