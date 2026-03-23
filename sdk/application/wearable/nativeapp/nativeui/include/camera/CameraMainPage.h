/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CameraMainPage
 * Created: 2025-06-05
 */

#ifndef CAMERA_MAIN_PAGE_H
#define CAMERA_MAIN_PAGE_H

#include <sys/time.h>
#include "View.h"
#include "components/root_view.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_list.h"
#include "components/ui_image_animator.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "diag_service.h"
#include "ohos_timer.h"
#include "components/ui_chart.h"
#include "camera/CameraModel.h"
#include "camera/CameraPresenter.h"

namespace OHOS {
class CameraPresenter;
class CameraModel;
class CameraMainPage : public SlicePage<CameraPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    CameraMainPage();
    ~CameraMainPage() override;
    static CameraMainPage *GetInstance();
    void OnStart(void* data) override;
    uint8_t GetConnectStatus();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void OnPause() override;
    bool GetFirstClickFlag();
    void SetFirstClickFlag(bool value);
    void SetUpButton();
    void InitCameraTimer();
    void InitCameraView();
    void DisableCameraTimer();
    void SetCameraTime(int time);
    void StopAnimator();
    void PhotoTaken();
    void Refresh_page();

private:
    UILabel *disconnectTitle_ = nullptr;
    UILabel *cameraFind_ = nullptr;
    UILabel *cameraTitle_ = nullptr;
    UILabel *cameraTime_ = nullptr;
    UIButton *takePhotoButton_ = nullptr;
    UIScrollView* container_ = nullptr;
    UIImageView *btDisconnected_ = nullptr;
    bool isFirstClick_;

class CameraAnimatorCallback : public AnimatorCallback {
public:
    explicit CameraAnimatorCallback(CameraMainPage* page) : page_(page) {}
    ~CameraAnimatorCallback() override {}
    void Callback(UIView *view) override;

private:
    CameraMainPage* page_{nullptr};
};
    Animator *animator_ = nullptr;
    CameraAnimatorCallback *callBack_ = nullptr;
};
}
#endif // CAMERA_MAIN_PAGE_H
