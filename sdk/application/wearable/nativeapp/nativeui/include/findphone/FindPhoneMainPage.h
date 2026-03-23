/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FindPhoneMainPage
 * Create: 2025-03-23
 */

#ifndef FINDPHONE_MAIN_PAGE_H
#define FINDPHONE_MAIN_PAGE_H

#include <string>
#include "View.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_list.h"
#include "components/ui_image_animator.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "diag_service.h"
#include "ohos_timer.h"
#include "FindPhoneModel.h"
#include "FindPhonePresenter.h"

namespace OHOS {
static constexpr uint16_t FINDPHONE_IMAGE_COUNT = 28;

class FindPhonePresenter;
class FindPhoneModel;
class FindPhoneMainPage : public SlicePage<FindPhonePresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    FindPhoneMainPage();
    ~FindPhoneMainPage() override;
    static FindPhoneMainPage *GetInstance();
    void OnStart(void* data) override;
    void OnStop() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool ClickButtonChange();
    void InitImageAnimator();
    void InitButton();
    void InitImageInfo();
    void ShowReset();
    void DisableFindPhoneTimer();
    void InitFindPhoneTimer();
    bool StopFindPhone();
    uint8_t GetConnectStatus();
    void ChangeToDisConnect();

private:
    UIViewGroup* container_ = nullptr;
    UILabel* labelDisconnect_ = nullptr;
    UILabel* labelTitle_ = nullptr;
    UIButton* buttonFind_ = nullptr;
    UIButton* buttonRetry_ = nullptr;
    UILabel *labelRing_ = nullptr;
    UIImageAnimatorView* imageAnimator_ = nullptr;
    GraphicTimer *startCount_ = nullptr;
    ImageAnimatorInfo searchImageInfo_[FINDPHONE_IMAGE_COUNT];
};
}
#endif // FINDPHONE_MAIN_PAGE_H
