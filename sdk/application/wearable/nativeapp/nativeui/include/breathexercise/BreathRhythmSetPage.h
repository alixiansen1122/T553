/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathRhythmSetPage
 * Create: 2025-5-13
 */
#ifndef BREATH_RHYTHM_SET_PAGE_H
#define BREATH_RHYTHM_SET_PAGE_H

#include "components/ui_label.h"
#include "components/ui_picker.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "View.h"
#include "SlicePage.h"
#include "SlicePageFactory.h"
#include "BreathModel.h"
#include "BreathPresenter.h"

namespace OHOS {
class BreathRhythmSetPage : public SlicePage<BreathPresenter>,
                    public UIView::OnClickListener,
                    public UIView::OnDragListener,
                    public UIPicker::SelectedListener {
public:
    BreathRhythmSetPage();
    ~BreathRhythmSetPage() override;
    void OnStart(void* data) override;
    void OnResume() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;

private:
    UILabel *lableRhythm_ = nullptr;
    UIPicker *pickerRhythm_ = nullptr;
    UIButton *buttonOk_ = nullptr;
    UIViewGroup *group_ = nullptr;
    UIImageView *pickerImg_ = nullptr;
};
}  // namespace OHOS

#endif