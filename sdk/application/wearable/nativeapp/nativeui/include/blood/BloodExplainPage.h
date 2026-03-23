/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodExplainPage
 * Create: 2025-06
 */

#ifndef BLOOD_EXPLAIN_PAGE_H
#define BLOOD_EXPLAIN_PAGE_H

#include "View.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "ui_test.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "BloodView.h"
#include "BloodModel.h"
#include "BloodPresenter.h"

namespace OHOS {
static constexpr uint8_t LABEL_DES_COUNT = 4;
class BloodPresenter;
class BloodModel;
class BloodExplainPage : public SlicePage<BloodPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    BloodExplainPage();
    ~BloodExplainPage() override;
    static BloodExplainPage *GetInstance();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool InitSimilarLabel(int16 num, int16 labelX, int16 labelY, const char *labelText);
private:
    UIScrollView *scrollGroup_ = nullptr;
    UIImageView *imgNormal_ = nullptr;
    UIImageView *imgGeneral_ = nullptr;
    UIImageView *imgAbnormal_ = nullptr;
    UILabel *labelNormal_ = nullptr;
    UILabel *labelGeneral_ = nullptr;
    UILabel *labelAbnormal_ = nullptr;
    UILabel *labelDes_[LABEL_DES_COUNT]{nullptr};
};
}
#endif // BLOOD_EXPLAIN_PAGE_H
