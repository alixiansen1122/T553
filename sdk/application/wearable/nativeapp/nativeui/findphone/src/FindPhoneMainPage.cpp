/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FindPhoneMainPage
 * Create: 2025-03-23
 */

#include "UiConfig.h"
#include "bts_br_gap.h"
#include "bts_spp.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"
#include "msg_center_device.h"
#include "ui_resource_findPhone.h"
#include "uiservice/ui_service.h"
#include "ui_test_group.h"
#include "settings/model/SettingDisplayModel.h"
#include "findphone/FindPhonePresenter.h"
#include "findphone/FindPhoneView.h"
#include "findphone/FindPhoneMainPage.h"

namespace OHOS {
static FindPhoneMainPage *g_pFindPhoneMainPage = nullptr;
static constexpr uint16_t FIND_PHONE_IMAGE_WIDTH = 500;
static constexpr uint16_t FIND_PHONE_IMAGE_HEIGHT = 500;
static constexpr uint16_t FIND_PHONE_BUTTON_POSITION_X = 154;
static constexpr uint16_t FIND_PHONE_BUTTON_POSITION_Y = 154;
static constexpr uint16_t FIND_PHONE_BUTTON_WIDTH = 146;
static constexpr uint16_t FIND_PHONE_BUTTON_HEIGHT = 146;
static constexpr uint16_t FIND_PHONE_RTYBUTTON_POSITION_X = 179;
static constexpr uint16_t FIND_PHONE_RTYBUTTON_POSITION_Y = 289;
static constexpr uint16_t FIND_PHONE_RTYBUTTON = 96;
static constexpr uint16_t FIND_PHONE_SIZE = 466;
static constexpr uint16_t FONT_DEFAULT_SIZE_1 = 36;
static constexpr uint16_t FONT_DEFAULT_SIZE_2 = 38;
static constexpr uint16_t FIND_PHONE_INITIAL = 0;
static constexpr uint16_t FIND_PHONE_TITLE_X = 140;
static constexpr uint16_t FIND_PHONE_TITLE_Y = 345;
static constexpr uint16_t FIND_PHONE_TITLE_WIDTH = 314;
static constexpr uint16_t FIND_PHONE_TITLE_HEIGHT = 100;
static constexpr uint16_t FIND_PHONE_FINDTITLE_X = 150;
static constexpr uint16_t FIND_PHONE_FINDTITLE_Y = 31;
static constexpr uint16_t FIND_PHONE_FINDTITLE_WIDTH = 253;
static constexpr uint16_t FIND_PHONE_FINDTITLE_HEIGHT = 53;
static constexpr uint16_t FIND_PHONE_RINGING_Y = 353;
static constexpr uint16_t FIND_PHONE_RINGING_WIDTH = 454;
static constexpr uint16_t FIND_PHONE_RINGING_HEIGHT = 42;
static constexpr uint16_t FIND_PHONE_SEARCHTIMEOUT_X = 50;
static constexpr uint16_t FIND_PHONE_SEARCHTIMEOUT_Y = 185;
static constexpr uint16_t FIND_PHONE_SEARCHTIMEOUT_WIDTH = 424;
static constexpr uint16_t FIND_PHONE_SEARCHTIMEOUT_HEIGHT = 50;
static constexpr uint16_t IMAGE_SIZE = 500;
static constexpr uint16_t IMAGE_OPA = 100;
static constexpr uint16_t IMAGEANIMATOERVIEW_TIME = 60;
static constexpr uint16_t FIND_PHONE_IMAGE_X = 5;
static constexpr uint16_t FIND_PHONE_IMAGE_Y = 1;
static constexpr uint16_t FINDPHONE_TIMEOUT = 60;
static constexpr uint16_t MS_ONE_SECONDS = 1000;
static constexpr char *FIND_BUTTON = "findButton";
static constexpr char *RETRY_BUTTON = "retryButton";
static constexpr uint32_t g_imageAnimatorInfo[FINDPHONE_IMAGE_COUNT] = {
    IMAGE_FINDPHONE_FRAME01, IMAGE_FINDPHONE_FRAME02, IMAGE_FINDPHONE_FRAME03, IMAGE_FINDPHONE_FRAME04,
    IMAGE_FINDPHONE_FRAME05, IMAGE_FINDPHONE_FRAME06, IMAGE_FINDPHONE_FRAME07, IMAGE_FINDPHONE_FRAME08,
    IMAGE_FINDPHONE_FRAME09, IMAGE_FINDPHONE_FRAME10, IMAGE_FINDPHONE_FRAME11, IMAGE_FINDPHONE_FRAME12,
    IMAGE_FINDPHONE_FRAME13, IMAGE_FINDPHONE_FRAME14, IMAGE_FINDPHONE_FRAME15, IMAGE_FINDPHONE_FRAME16,
    IMAGE_FINDPHONE_FRAME17, IMAGE_FINDPHONE_FRAME18, IMAGE_FINDPHONE_FRAME19, IMAGE_FINDPHONE_FRAME20,
    IMAGE_FINDPHONE_FRAME21, IMAGE_FINDPHONE_FRAME22, IMAGE_FINDPHONE_FRAME23, IMAGE_FINDPHONE_FRAME24,
    IMAGE_FINDPHONE_FRAME25, IMAGE_FINDPHONE_FRAME26, IMAGE_FINDPHONE_FRAME27, IMAGE_FINDPHONE_FRAME28,
};

REGIST_SLICE_PAGE(VIEW_FIND_PHONE, FIND_PHONE_PAGES::FIND_PHONE_MAIN_PAGE, FindPhoneMainPage, true);

FindPhoneMainPage::FindPhoneMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FindPhoneMainPage::FindPhoneMainPage");
    g_pFindPhoneMainPage = this;
}

FindPhoneMainPage::~FindPhoneMainPage()
{
    g_pFindPhoneMainPage = nullptr;
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }

    if (buttonFind_ != nullptr) {
        delete buttonFind_;
        buttonFind_ = nullptr;
    }

    if (labelDisconnect_ != nullptr) {
        delete labelDisconnect_;
        labelDisconnect_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelRing_ != nullptr) {
        delete labelRing_;
        labelRing_ = nullptr;
    }

    if (buttonRetry_ != nullptr) {
        delete buttonRetry_;
        buttonRetry_ = nullptr;
    }

    if (startCount_ != nullptr) {
        delete startCount_;
        startCount_ = nullptr;
    }

    if (imageAnimator_ != nullptr) {
        delete imageAnimator_;
        imageAnimator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(FINDPHONE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FindPhoneMainPage::~FindPhoneMainPage");
}

FindPhoneMainPage *FindPhoneMainPage::GetInstance()
{
    return g_pFindPhoneMainPage;
}

void FindPhoneMainPage::OnStop()
{
    if (imageAnimator_->GetState() == true) {
        DisableFindPhoneTimer();
        uint8_t value = 1;
        errcode_t result;
#ifdef _WIN32
        result = 0;
#else
        result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_STOP_FIND_PHONE, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnStop stop find phone result:%d ", result);
#endif
    }
}

void FindPhoneMainPage::OnStart(void *data)
{
    container_ = new UIViewGroup();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new container_ fail");
        return;
    }
    container_->SetPosition(FIND_PHONE_INITIAL, FIND_PHONE_INITIAL, FIND_PHONE_SIZE, FIND_PHONE_SIZE);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    container_->SetTouchable(true);
    container_->SetDraggable(true);
    container_->SetOnDragListener(this);

    InitImageAnimator();
    InitButton();

    labelDisconnect_ = new UILabel();
    if (labelDisconnect_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelDisconnect_ fail");
        return;
    }
    labelDisconnect_->SetText("蓝牙已断开");
    labelDisconnect_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE_1);
    labelDisconnect_->SetPosition(FIND_PHONE_TITLE_X, FIND_PHONE_TITLE_Y, FIND_PHONE_TITLE_WIDTH,
                                  FIND_PHONE_TITLE_HEIGHT);
    container_->Add(labelDisconnect_);
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetText("查找手机");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE_2);
    labelTitle_->SetPosition(FIND_PHONE_FINDTITLE_X, FIND_PHONE_FINDTITLE_Y, FIND_PHONE_FINDTITLE_WIDTH,
                             FIND_PHONE_FINDTITLE_HEIGHT);
    container_->Add(labelTitle_);

    labelRing_ = new UILabel();
    if (labelRing_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelRing_ fail");
        return;
    }
    labelRing_->SetText("响铃");
    labelRing_->SetPosition(0, FIND_PHONE_RINGING_Y, FIND_PHONE_RINGING_WIDTH, FIND_PHONE_RINGING_HEIGHT);
    labelRing_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE_1);
    labelRing_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelRing_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    container_->Add(labelRing_);
    if (GetConnectStatus() == 1) {
        labelRing_->SetVisible(true);
        labelDisconnect_->SetVisible(false);
        LOADIMG::LoadBtnImage(buttonFind_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_CONNECT, IMAGE_FINDPHONE_CONNECT);
        buttonFind_->Enable();
    } else {
        LOADIMG::LoadBtnImage(buttonFind_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_DISCONNECT, IMAGE_FINDPHONE_DISCONNECT);
        buttonFind_->Disable();
        labelDisconnect_->SetVisible(true);
        labelRing_->SetVisible(false);
    }
    imageAnimator_->Stop();
    imageAnimator_->SetVisible(false);
    AddViewToPageContainer(container_);
}

void FindPhoneMainPage::InitImageAnimator()
{
    imageAnimator_ = new UIImageAnimatorView();
    if (imageAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "imageAnimator_ new fail");
        return;
    }
    for (int i = 0; i < FINDPHONE_IMAGE_COUNT; i++) {
        searchImageInfo_[i].imageInfo =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(g_imageAnimatorInfo[i], FINDPHONE_IMAGE);
        searchImageInfo_[i].imageType = IMG_SRC_IMAGE_INFO;
        searchImageInfo_[i].pos.x = FIND_PHONE_IMAGE_X;
        searchImageInfo_[i].pos.y = FIND_PHONE_IMAGE_Y;
        searchImageInfo_[i].width = IMAGE_SIZE;
        searchImageInfo_[i].height = IMAGE_SIZE;
    }
    imageAnimator_->SetPosition(FIND_PHONE_IMAGE_X, FIND_PHONE_IMAGE_Y, FIND_PHONE_IMAGE_WIDTH,
                                FIND_PHONE_IMAGE_HEIGHT);
    imageAnimator_->SetImageAnimatorSrc(searchImageInfo_, FINDPHONE_IMAGE_COUNT, IMAGEANIMATOERVIEW_TIME);
    imageAnimator_->Stop();
    imageAnimator_->LayoutCenterOfParent();
    imageAnimator_->SetDraggable(true);
    imageAnimator_->SetOnDragListener(this);
    container_->Add(imageAnimator_);
}

void FindPhoneMainPage::InitButton()
{
    buttonFind_ = new UIButton();
    if (buttonFind_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonFind_ fail");
        return;
    }
    buttonFind_->SetPosition(FIND_PHONE_BUTTON_POSITION_X, FIND_PHONE_BUTTON_POSITION_Y);
    buttonFind_->Resize(FIND_PHONE_BUTTON_WIDTH, FIND_PHONE_BUTTON_HEIGHT);
    buttonFind_->SetViewId(FIND_BUTTON);
    buttonFind_->SetOnClickListener(this);
    buttonFind_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    buttonFind_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonFind_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    buttonFind_->SetVisible(true);
    LOADIMG::LoadBtnImage(buttonFind_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_CONNECT, IMAGE_FINDPHONE_CONNECT);
    buttonRetry_ = new UIButton();
    if (buttonRetry_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonRetry_ fail");
        return;
    }
    buttonRetry_->SetPosition(FIND_PHONE_RTYBUTTON_POSITION_X, FIND_PHONE_RTYBUTTON_POSITION_Y);
    buttonRetry_->Resize(FIND_PHONE_RTYBUTTON, FIND_PHONE_RTYBUTTON);
    buttonRetry_->SetViewId(RETRY_BUTTON);
    buttonRetry_->SetOnClickListener(this);
    buttonRetry_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    buttonRetry_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadBtnImage(buttonRetry_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_RESET, IMAGE_FINDPHONE_RESET);
    buttonRetry_->SetVisible(false);
    container_->Add(buttonRetry_);
    container_->Add(buttonFind_);
}

bool FindPhoneMainPage::ClickButtonChange()
{
    errcode_t result;
    uint8_t value = 1;
    if (imageAnimator_->GetState() == true) {
#ifdef _WIN32
        result = 0;
#else
        result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_STOP_FIND_PHONE, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ClickButtonChange stop find phone result:%d ", result);
#endif
        if (result == 0) {
            DisableFindPhoneTimer();
            LOADIMG::LoadBtnImage(buttonFind_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_CONNECT, IMAGE_FINDPHONE_CONNECT);
            imageAnimator_->Stop();
            imageAnimator_->SetVisible(false);
            buttonFind_->SetOpaScale(OPA_OPAQUE);
            labelRing_->SetText("响铃");
            return true;
        }
    } else {
#ifdef _WIN32
        result = 0;
#else
        result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_START_FIND_PHONE, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ClickButtonChange start find phone result:%d ", result);
#endif
        if (result == 0) {
            InitFindPhoneTimer();
            imageAnimator_->Start();
            imageAnimator_->SetVisible(true);
            buttonFind_->SetOpaScale(OPA_TRANSPARENT);
            labelRing_->SetText("停止响铃");
            return true;
        }
    }
    return true;
}

void FindPhoneMainPage::ShowReset()
{
    // rtybutton、title显示，findTitle、button隐藏，imageAnimator停止
#ifdef _WIN32
#else
    uint8_t value = 1;
    uint8_t result = 0;
    result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_STOP_FIND_PHONE, &value, sizeof(value));
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ShowReset stop find phone result:%d ", result);
#endif
    labelDisconnect_->SetText("查找手机超时，请重试");
    labelDisconnect_->SetPosition(FIND_PHONE_SEARCHTIMEOUT_X, FIND_PHONE_SEARCHTIMEOUT_Y,
                                  FIND_PHONE_SEARCHTIMEOUT_WIDTH, FIND_PHONE_SEARCHTIMEOUT_HEIGHT);
    labelDisconnect_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE_1);
    labelDisconnect_->SetVisible(true);
    buttonRetry_->SetVisible(true);
    labelTitle_->SetVisible(false);
    labelRing_->SetVisible(false);
    buttonFind_->SetVisible(false);
    imageAnimator_->Stop();
    imageAnimator_->SetVisible(false);
    DisableFindPhoneTimer();
}

bool FindPhoneMainPage::StopFindPhone()
{
    if (imageAnimator_->GetState() == true) {
        DisableFindPhoneTimer();
        LOADIMG::LoadBtnImage(buttonFind_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_CONNECT, IMAGE_FINDPHONE_CONNECT);
        imageAnimator_->Stop();
        imageAnimator_->SetVisible(false);
        buttonFind_->SetOpaScale(OPA_OPAQUE);
        labelRing_->SetText("响铃");
        return true;
    }
    return true;
}

bool FindPhoneMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), RETRY_BUTTON) == 0) {
        // rtybutton、title隐藏，findTitle、button显示，imageAnimator播放
        uint8_t value = 1;
        errcode_t result;
#ifdef _WIN32
        result = 0;
#else
        result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_START_FIND_PHONE, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnClick start find phone result:%d ", result);
#endif
        if (result == 0) {
            buttonRetry_->SetVisible(false);
            labelDisconnect_->SetVisible(false);
            buttonFind_->SetVisible(true);
            labelTitle_->SetVisible(true);
            InitFindPhoneTimer();
            imageAnimator_->Start();
            imageAnimator_->SetVisible(true);
            buttonFind_->SetOpaScale(OPA_TRANSPARENT);
            labelRing_->SetVisible(true);
            labelRing_->SetText("停止响铃");
            return true;
        }
    } else if (strcmp(view.GetViewId(), FIND_BUTTON) == 0) {
        ClickButtonChange();
    }
    return true;
}

bool FindPhoneMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

uint8_t FindPhoneMainPage::GetConnectStatus()
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

void FindPhoneMainPage::ChangeToDisConnect()
{
    LOADIMG::LoadBtnImage(buttonFind_, FINDPHONE_IMAGE, IMAGE_FINDPHONE_DISCONNECT, IMAGE_FINDPHONE_DISCONNECT);
    DisableFindPhoneTimer();
    imageAnimator_->Stop();
    imageAnimator_->SetVisible(false);
    buttonFind_->SetOpaScale(OPA_OPAQUE);
    buttonFind_->Disable();
    labelDisconnect_->SetVisible(true);
    labelRing_->SetVisible(false);
}

/* 定时器回调函数 */
void FindPhoneTimerCallback(void *data)
{
    (void)data;
    FindPhoneMainPage::GetInstance()->ShowReset();
}

/* 开启定时器 */
void FindPhoneMainPage::InitFindPhoneTimer()
{
    if (startCount_ != nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "already exists!!");
        return;
    }
    uint32 tick = GetOSTick(FINDPHONE_TIMEOUT * MS_ONE_SECONDS);
    startCount_ = new GraphicTimer(tick, FindPhoneTimerCallback, nullptr, true);
    if (startCount_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new GraphicTimer failed!!");
        return;
    }
    SettingDisplayModel::GetInstance().EnableSteadyOn();
    bool retTimer = startCount_->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "osTimerStart failed!!");
        if (startCount_ != nullptr) {
            delete startCount_;
            startCount_ = nullptr;
        }
        return;
    }
}

/* 关闭定时器 */
void FindPhoneMainPage::DisableFindPhoneTimer()
{
    SettingDisplayModel::GetInstance().DisableSteadyOn();
    if (startCount_ != nullptr) {
        startCount_->Stop();
        delete startCount_;
        startCount_ = nullptr;
    }
}
}
