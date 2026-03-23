/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBuletoothPage
 * Created: 2025-06-05
 */
#ifndef SETTING_BLUETOOTH_PAGE_H
#define SETTING_BLUETOOTH_PAGE_H

#include "components/ui_scroll_view_nested.h"
#include "components/ui_simple_list.h"
#include "components/ui_button.h"
#include "components/ui_label.h"
#include "SlicePage.h"
#include "common/image_cache_manager.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "graphic_timer.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingPresenter.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/page/SettingConnectNewPhone.h"
#include "settings/page/SettingReconnection.h"
#include "settings/page/SettingConnectBlueFail.h"
#include "settings/page/SettingConnectBlueSuccess.h"

namespace OHOS {
static constexpr uint16_t SEARCH_IMAGE_COUNT = 31;

class SettingBluetoothPage : public SlicePage<SettingPresenter>,
                             public UIView::OnClickListener,
                             public UIView::OnDragListener {
public:
    SettingBluetoothPage();
    ~SettingBluetoothPage() override;
    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    static SettingBluetoothPage *GetInstance(void);
    void InitSearchAni(void);
    void InitImageAnimator();
    void InitContentList();
    void BtScan();
    void StartBtScan();
    void StopBtScan();
    bool GetScanStatus(void);
    void BtAnimatorStart();
    void BtAnimatorStop();
    void SetScanStatus(bool value);
    void InitPageView();
    void DestoryPageView();
    void NotFindDevice();
    void LoadFindDeviceList();
    void UpdataImageAnimatorAndList();
    void TimerFindDevice();
    void UpdateBtListContent(int16_t viewIndex);
    bool IsDeviceConnect();
    bool GetImageAnimatorState();
    void ChangeView(int16_t value);
    void ShowResultGroup(int16_t value);

protected:
    static UIView* CreateForContent(uint8_t type);
    static void UpdateForContent(UIView* view, void* data, uint8_t type);
    static UIView* CreateForBtList(uint8_t type);
    static void UpdateForBtList(UIView* view, void* data, uint8_t type);

private:
    bool scanStart_{0};
    UIScrollViewNested *scroll_ = nullptr;
    UILabel *titleLabel_ = nullptr;
    UILabel *noDeviceLabel_ = nullptr;
    UILabel *availableDeviceLabel_ = nullptr;
    UILabel *searchBtnTextLabel_ = nullptr;
    UILabelButton *resetResearchButton_ = nullptr;
    UIImageAnimatorView *imageAnimator_;
    ImageAnimatorInfo searchImageInfo_[SEARCH_IMAGE_COUNT];
    UISimpleList *optionList_ = nullptr;
    UISimpleList *btEarPhoneList_ = nullptr;
    SettingConnectNewPhone *connectNewPhoneFragment_ = nullptr;
    SettingConnectionOpen* connectOpenFragment_ = nullptr;
    SettingReconnection *reconnectionFragment_ = nullptr;
    SettingReconnectSecondConfirm* secondConfirmFragment_ = nullptr;
    UnPairSecondConfirm* unPairFragment_ = nullptr;
    SettingConnectBlueSuccess *successFragment_ = nullptr;
    SettingConnectBlueFail *failFragment_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif
void ScanTimerCallback(int data);
void SettingCancelBrScanHandle(void);
void SettingEndScanHandle(void);
#ifdef __cplusplus
}
#endif
} // OHOS
#endif