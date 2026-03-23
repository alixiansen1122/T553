/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathResult
 * Create: 2025-5-13
 */
#ifndef BREATH_RESULT_PAGE_H
#define BREATH_RESULT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_picker.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label_button.h"
#include "components/ui_toggle_button.h"
#include "View.h"
#include "power_display_service.h"
#include "UiConfig.h"
#include "SlicePage.h"
#include "SlicePageFactory.h"
#include "BreathModel.h"
#include "BreathPresenter.h"
#ifndef _WIN32
#include "time64.h"
#endif

namespace OHOS {
static constexpr uint16_t LABEL_COUNT = 5;

class BreathResultPage : public SlicePage<BreathPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BreathResultPage();
    ~BreathResultPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void InitBtn();
    void InitBtnBg();
    void InitLabelDes();
    void InitLabelCount();
    void InitLabelUnit();
    uint8_t GetTimeScour();
private:
    UIScrollView *group_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelTimeAndRhythmTitle_ = nullptr;
    UIButton *buttonBg_[LABEL_COUNT]{nullptr};
    UILabel *labelDes_[LABEL_COUNT]{nullptr};
    UILabel *labelValue_[LABEL_COUNT]{nullptr};
    UILabel *labelUnit_[LABEL_COUNT]{nullptr};
    UIImageView *scores_ = nullptr;
    UIImageView *pageBgImg_ = nullptr;
    UIButton *buttonComplete_ = nullptr;
    UIButton *buttonRestart_ = nullptr;
};
}
#endif