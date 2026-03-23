/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CameraMainPage
 * Created: 2025-06-05
 */

#include <string>
#include "ui_test_group.h"
#include "phoneservice/PhoneService.h"
#include "uiservice/ui_service.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"
#include "msg_center_device.h"
#include "bts_spp.h"
#include "ui_resource_camera.h"
#include "camera/CameraView.h"
#include "camera/CameraPresenter.h"
#include "camera/CameraMainPage.h"

namespace OHOS {
static CameraMainPage *g_pCameraMainPage = nullptr;
static constexpr uint16_t BLUETOOTH_XPOS = 158;
static constexpr uint16_t BLUETOOTH_XPOY = 154;
static constexpr uint16_t TITLE_XPOS = 15;
static constexpr uint16_t TITLE_XPOY = 329;
static constexpr uint16_t TITLE_WIDTH = 424;
static constexpr uint16_t TITLE_HIGHT = 50;
static constexpr uint16_t TITLE_FONT_SIZE = 36;
static constexpr uint16_t CAMERATITLE_FONT_SIZE = 38;
static constexpr uint16_t CAMERATITLE_XPOS = 97;
static constexpr uint16_t CAMERATITLE_XPOY = 37;
static constexpr uint16_t CAMERATITLE_WIDTH = 260;
static constexpr uint16_t CAMERATITLE_HIGHT = 53;
static constexpr uint16_t CAMERAFIND_XPOS = 123;
static constexpr uint16_t CAMERAFIND_XPOY = 192;
static constexpr uint16_t CAMERAFIND_WIDTH = 250;
static constexpr uint16_t CAMERAFIND_HIGHT = 42;
static constexpr uint16_t CAMERATIME_XPOS = 102;
static constexpr uint16_t BUTTON_WIDTH = 171;
static constexpr uint16_t BUTTON_HIGHT = 171;
static constexpr uint16_t COUNTDOWN_TIME = 5;
static constexpr uint16_t CAMERA_SIZE = 466;
static constexpr uint16_t CAMERA_BUTTON_POSITION_X = 144;
static constexpr uint16_t CAMERA_BUTTON_POSITION_Y = 154;
static constexpr uint16_t TIME_FONT_SIZE = 72;
static constexpr uint16_t CAMERATIME_HIGHT = 70;
static constexpr char *TAKEPHOTO_BUTTON = "takePhotoButton";

REGIST_SLICE_PAGE(VIEW_CAMERA, CAMERA_PAGES::CAMERA_MAIN_PAGE, CameraMainPage, true);

CameraMainPage::CameraMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CameraMainPage::CameraMainPage");
    ImageCacheManager::GetInstance().LoadAllInMultiRes(CAMERA_IMAGE);
    g_pCameraMainPage = this;
}

CameraMainPage::~CameraMainPage()
{
    g_pCameraMainPage = nullptr;
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }

    if (disconnectTitle_ != nullptr) {
        delete disconnectTitle_;
        disconnectTitle_ = nullptr;
    }

    if (btDisconnected_ != nullptr) {
        delete btDisconnected_;
        btDisconnected_ = nullptr;
    }

    if (cameraFind_ != nullptr) {
        delete cameraFind_;
        cameraFind_ = nullptr;
    }

    if (cameraTitle_ != nullptr) {
        delete cameraTitle_;
        cameraTitle_ = nullptr;
    }

    if (cameraTime_ != nullptr) {
        delete cameraTime_;
        cameraTime_ = nullptr;
    }

    if (takePhotoButton_ != nullptr) {
        delete takePhotoButton_;
        takePhotoButton_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(CAMERA_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CameraMainPage::~CameraMainPage");
}

CameraMainPage *CameraMainPage::GetInstance()
{
    return g_pCameraMainPage;
}

void CameraMainPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "container_ is nullptr");
        return;
    }
    container_->SetPosition(0, 0, CAMERA_SIZE, CAMERA_SIZE);
    container_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    container_->SetTouchable(true);
    container_->SetDraggable(true);
    container_->SetOnDragListener(this);

    btDisconnected_ = new UIImageView();
    if (btDisconnected_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "btDisconnected_ is nullptr");
        return;
    }
    LOADIMG::LoadImageViewImg(btDisconnected_, CAMERA_IMAGE, IMAGE_CAMERA_DISCONNECT);
    btDisconnected_->SetPosition(BLUETOOTH_XPOS, BLUETOOTH_XPOY);
    btDisconnected_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    btDisconnected_->SetVisible(false);
    container_->Add(btDisconnected_);

    disconnectTitle_ = new UILabel();
    if (disconnectTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "disconnectTitle_ is nullptr");
        return;
    }
    disconnectTitle_->SetText("蓝牙已断开");
    disconnectTitle_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_FONT_SIZE);
    disconnectTitle_->SetPosition(TITLE_XPOS, TITLE_XPOY, TITLE_WIDTH, TITLE_HIGHT);
    disconnectTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    disconnectTitle_->SetVisible(false);
    container_->Add(disconnectTitle_);

    cameraTitle_ = new UILabel();
    if (cameraTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cameraTitle_ is nullptr");
        return;
    }
    cameraTitle_->SetText("相机");
    cameraTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, CAMERATITLE_FONT_SIZE);
    cameraTitle_->SetPosition(CAMERATITLE_XPOS, CAMERATITLE_XPOY, CAMERATITLE_WIDTH, CAMERATITLE_HIGHT);
    cameraTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    cameraTitle_->SetVisible(false);
    container_->Add(cameraTitle_);

    cameraFind_ = new UILabel();
    if (cameraFind_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cameraFind_ is nullptr");
        return;
    }
    cameraFind_->SetText("正在连接手机...");
    cameraFind_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_FONT_SIZE);
    cameraFind_->SetPosition(CAMERAFIND_XPOS, CAMERAFIND_XPOY, CAMERAFIND_WIDTH, CAMERAFIND_HIGHT);
    cameraFind_->SetVisible(false);
    container_->Add(cameraFind_);

    cameraTime_ = new UILabel();
    if (cameraTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cameraTime_ is nullptr");
        return;
    }
    cameraTime_->SetText("5s");
    cameraTime_->SetFont(BOLD_VECTOR_FONT_FILENAME, TIME_FONT_SIZE);
    cameraTime_->SetPosition(CAMERATIME_XPOS, CAMERAFIND_XPOY, CAMERAFIND_WIDTH, CAMERATIME_HIGHT);
    cameraTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    cameraTime_->SetVisible(false);
    container_->Add(cameraTime_);
    SetUpButton();
    callBack_ = new CameraAnimatorCallback(this);
    if (callBack_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "callBack_ is nullptr");
        return;
    }
    animator_ = new Animator(callBack_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "animator_ is nullptr");
        return;
    }
    AddViewToPageContainer(container_);
    InitCameraView();
}

uint8_t CameraMainPage::GetConnectStatus()
{
    uint8_t conn_status = 0;

#ifdef _WIN32
    conn_status = 1;
#else
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_PHONE) {
        conn_status = is_spp_connected(0) == true ? 1 : 0;
    } else if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT) {
        conn_status = msg_center_get_ble_conn_state();
    }
#endif
    return conn_status;
}

void CameraMainPage::InitCameraView()
{
    if (GetConnectStatus() == 1) {
        cameraTitle_->SetVisible(true);
        cameraFind_->SetVisible(true);
        disconnectTitle_->SetVisible(false);
        btDisconnected_->SetVisible(false);
        cameraTime_->SetVisible(false);
        takePhotoButton_->SetVisible(false);
        takePhotoButton_->Disable();
        takePhotoButton_->SetTouchable(false);
        container_->Invalidate();
        uint8_t value = 1;
        errcode_t ret;
#ifdef _WIN32
        ret = 0;
#else
        ret = msg_center_send_data(MSGCENTER_CMD_DEVICE,
                                   MSGCENTER_TYPE_ID_CAMERA_CONNECTING_PHONE, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "connecting phoneCamera ret = %d", ret);
#endif
    } else {
        disconnectTitle_->SetVisible(true);
        btDisconnected_->SetVisible(true);
        cameraTitle_->SetVisible(false);
        cameraFind_->SetVisible(false);
        cameraTime_->SetVisible(false);
        takePhotoButton_->SetVisible(false);
        takePhotoButton_->Disable();
        takePhotoButton_->SetTouchable(false);
        container_->Invalidate();
    }
}

void CameraMainPage::Refresh_page()
{
    cameraTitle_->SetVisible(true);
    takePhotoButton_->SetVisible(true);
    takePhotoButton_->Enable();
    takePhotoButton_->SetTouchable(true);
    cameraFind_->SetVisible(false);
    disconnectTitle_->SetVisible(false);
    btDisconnected_->SetVisible(false);
    cameraTime_->SetVisible(false);
    container_->Invalidate();
}

void CameraMainPage::SetUpButton()
{
    takePhotoButton_ = new UILabelButton();
    if (takePhotoButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "takePhotoButton_ is nullptr");
        return;
    }

    takePhotoButton_->SetPosition(CAMERA_BUTTON_POSITION_X, CAMERA_BUTTON_POSITION_Y);
    takePhotoButton_->Resize(BUTTON_WIDTH, BUTTON_HIGHT);
    takePhotoButton_->SetViewId(TAKEPHOTO_BUTTON);
    takePhotoButton_->SetOnClickListener(this);
    takePhotoButton_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    takePhotoButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    takePhotoButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    takePhotoButton_->SetVisible(false);
    takePhotoButton_->Disable();
    takePhotoButton_->SetTouchable(false);
    LOADIMG::LoadBtnImage(takePhotoButton_, CAMERA_IMAGE, IMAGE_CAMERA_TAKEPHOTO, IMAGE_CAMERA_TAKEPHOTO);
    container_->Add(takePhotoButton_);
}

void CameraMainPage::OnPause()
{
    animator_->Stop();
}

bool CameraMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), TAKEPHOTO_BUTTON) == 0) {
        if (GetConnectStatus() == 1) {
            takePhotoButton_->SetVisible(false);
            takePhotoButton_->Disable();
            cameraTime_->SetVisible(true);
            SetFirstClickFlag(true);
            animator_->Start();
        } else {
            disconnectTitle_->SetVisible(true);
            btDisconnected_->SetVisible(true);
            cameraTitle_->SetVisible(false);
            cameraFind_->SetVisible(false);
            cameraTime_->SetVisible(false);
            takePhotoButton_->SetVisible(false);
            takePhotoButton_->Disable();
            takePhotoButton_->SetTouchable(false);
            container_->Invalidate();
        }
    }
    return true;
}

bool CameraMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        uint8_t value = 1;
        errcode_t ret;
#ifdef _WIN32
        ret = 0;
#else
        ret = msg_center_send_data(MSGCENTER_CMD_DEVICE,
                                   MSGCENTER_TYPE_ID_PHONE_EXIT_CAMERA, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "notify phone exit camera, ret = %d", ret);
#endif
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

void CameraMainPage::CameraAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    static int count = 0;
    static struct timeval start_time;
    struct timeval current_time;
    if (page_->GetFirstClickFlag()) {
        gettimeofday(&start_time, nullptr);
        page_->SetFirstClickFlag(false);
    }
    gettimeofday(&current_time, nullptr);
    int time = current_time.tv_sec - start_time.tv_sec;
    page_->SetCameraTime(time);
    if (time == COUNTDOWN_TIME) {
        page_->StopAnimator();
    }
}

void CameraMainPage::StopAnimator()
{
    animator_->Stop();
    takePhotoButton_->SetVisible(true);
    takePhotoButton_->Enable();
    cameraTime_->SetVisible(false);
    cameraTime_->SetText("5s");
    SetFirstClickFlag(false);
    uint8_t value = 1;
    errcode_t ret;
#ifdef _WIN32
    ret = 0;
#else
    ret = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_ON_CAMERA, &value, sizeof(value));
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "notify phone take photo ret = %d", ret);
#endif
}

void CameraMainPage::SetCameraTime(int time)
{
    int progressTime = COUNTDOWN_TIME - time;
    int progressValue = 0;
    std::string strTime = std::to_string(COUNTDOWN_TIME - time) + "s";
    cameraTime_->SetText(strTime.c_str());
}

bool CameraMainPage::GetFirstClickFlag()
{
    return isFirstClick_;
}

void CameraMainPage::SetFirstClickFlag(bool value)
{
    isFirstClick_ = value;
}
}