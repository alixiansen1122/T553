/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBluetoothPage
 * Created: 2025-06-05
 */

#include "components/ui_simple_list.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "ui_resource_settings.h"
#include "ohos_timer.h"
#include "graphic_service.h"
#include "main/LoadImg.h"
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingBluetoothItemGroup.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/page/SettingBluetoothPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::BLUETOOTH_PAGE, SettingBluetoothPage, false);

static SettingBluetoothPage *g_pSettingBluetoothPage{nullptr};
GraphicTimer* g_settingStopScanHandle = nullptr;
static constexpr uint32_t SCAN_TIME = 25000;
static constexpr int16_t SETTING_ADDR2 = 2;
static constexpr int16_t SETTING_ADDR3 = 3;
static constexpr int16_t SETTING_ADDR4 = 4;
static constexpr int16_t SETTING_ADDR5 = 5;
static constexpr int16_t SETTING_LIST_POSITION_X = 14;
static constexpr int16_t SETTING_LIST_POSITION_Y = 350;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 50;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 LIST_POS = 20;
static constexpr int16 SNUM_2 = 2;
static constexpr int16_t SETTING_CONTENT_HEIGHT = 160;
static constexpr int16_t SETTING_TITLE_Y = 39;
static constexpr int16_t SETTING_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_TITLE_FONT = 38;
static constexpr int16_t SETTING_AVILABLE_Y = 102;
static constexpr int16_t SETTING_AVILABLE_HEIGHT = 39;
static constexpr int16_t SETTING_AVILABLE_FONT = 32;
static constexpr int16_t SETTING_SEATCHBTN_HEIGHT = 36;
static constexpr int16_t SETTING_SEATCHBTN_FONT = 30;
static constexpr int16_t SETTING_ITEM_HEIGHT_ = 130;
static constexpr int16_t SETTING_NODEVICE_Y = 150;
static constexpr int16_t SETTING_NODEVICE_FONT = 25;
static constexpr int16_t SETTING_RESEARCH_X = 111;
static constexpr int16_t SETTING_RESEARCH_Y = 230;
static constexpr int16_t SETTING_CONYENT_HEIGHT = 40;
static constexpr int16_t SETTING_RESEARCH_WIDTH = 232;
static constexpr int16_t SETTING_RESEARCH_HEIGHT = 76;
static constexpr int16_t SETTING_IMGANI_X = 163;
static constexpr int16_t SETTING_IMGANI_HEIGHT = 126;
static constexpr int16_t SETTING_IMGANI_Y = 150;
static constexpr int16_t SETTING_IMGANI_UPDATE = 120;
static constexpr int16_t SETTING_BTLIST_X = 426;
static constexpr int16_t SETTING_BTLIST_Y = 129;
static constexpr int16_t SETTING_BTLIST_ROUND = 30;
static constexpr int16_t CONTAINER_BLANK_SIZE = 100;
static constexpr int16_t LIST_WIDTH = 416;
static constexpr int16_t SEARCH_BTN_Y = 271;
static constexpr int16_t LIST_CONTENT_HEIGHT = 207;
static constexpr int16_t SETTING_RESET_RESEARCH_OFFSE5T = 50;
static constexpr int16_t SETTING_RESETTESEARCH_OFFSE5T = 80;
static constexpr int16_t SETTING_CONTENT_OFFSE5T_SEARCH = 85;
static constexpr int16_t SETTING_CONTENT_OFFSE5T_NOSEARCH = 200;
static constexpr char *RESEARCH_BUTTON = "researchButton";
static constexpr char *OPTION_LIST = "optionList";
static constexpr char *BT_LIST = "btList";

std::vector<IconTextNavigationItemData> g_bluetoothOptionData = {
    {SettingPageId::CONNECT_NEW_PHONE_PAGE, IMAGE_PHONE, "连接新手机",
        nullptr, true, SETTING_LIST_ABOUT_BG_COLOR, SETTING_LIST_ABOUT_BG_COLOR_OPA},
    {SettingPageId::QRCODE_CONN_PAGE, IMAGE_QCODE, "二维码连接",
        nullptr, true, SETTING_LIST_ABOUT_BG_COLOR, SETTING_LIST_ABOUT_BG_COLOR_OPA}};

const int g_imageAnimator[SEARCH_IMAGE_COUNT] = {
    IMAGE_Y3_1,
    IMAGE_Y3_2,
    IMAGE_Y3_3,
    IMAGE_Y3_4,
    IMAGE_Y3_5,
    IMAGE_Y3_6,
    IMAGE_Y3_7,
    IMAGE_Y3_8,
    IMAGE_Y3_9,
    IMAGE_Y3_10,
    IMAGE_Y3_11,
    IMAGE_Y3_12,
    IMAGE_Y3_13,
    IMAGE_Y3_14,
    IMAGE_Y3_15,
    IMAGE_Y3_16,
    IMAGE_Y3_17,
    IMAGE_Y3_18,
    IMAGE_Y3_19,
    IMAGE_Y3_20,
    IMAGE_Y3_21,
    IMAGE_Y3_22,
    IMAGE_Y3_23,
    IMAGE_Y3_24,
    IMAGE_Y3_25,
    IMAGE_Y3_26,
    IMAGE_Y3_27,
    IMAGE_Y3_28,
    IMAGE_Y3_29,
    IMAGE_Y3_30,
    IMAGE_Y3_31,
};

SettingBluetoothPage::SettingBluetoothPage()
{
    g_pSettingBluetoothPage = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::SettingBluetoothPage");
}

SettingBluetoothPage::~SettingBluetoothPage()
{
    bool ret = is_bt_discovering(BT_TRANSPORT_BR_EDR);
    if (ret) {
        bool results = gap_br_cancel_discovery();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtScan CancelBtDiscovery results %d", results);
    }
    BtAnimatorStop();
    DestoryPageView();
    g_pSettingBluetoothPage = nullptr;
    SettingBluetoothModel::GetInstance()->SetConnectFragmentStatus(false);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::~SettingBluetoothPage");
}

void SettingBluetoothPage::InitContentList()
{
    optionList_->ClearAll();
    for (auto& it : g_bluetoothOptionData) {
        Contents cont;
        cont.createFunc = SettingBluetoothPage::CreateForContent;
        cont.updateFunc = SettingBluetoothPage::UpdateForContent;
        cont.type = SettingUnionItemType::ICON_TEXT_NAVIGATION;
        cont.data = &it;
        optionList_->AddContent(cont);
    }
    optionList_->RefreshList();
}

SettingBluetoothPage* SettingBluetoothPage::GetInstance()
{
    return g_pSettingBluetoothPage;
}

void SettingBluetoothPage::SetScanStatus(bool value)
{
    scanStart_ = value;
}

bool SettingBluetoothPage::GetScanStatus()
{
    return scanStart_;
}

void SettingBluetoothPage::OnStart(void *data)
{
    InitPageView();
    InitImageAnimator();
    if (SettingBluetoothModel::GetInstance()->GetScansDevicesList().size() != 0) {
        SettingBluetoothModel::GetInstance()->MoveMatchingDeviceToFront();
        LoadFindDeviceList();
    }
    BtScan();
    InitContentList();
    AddViewToPageContainer(scroll_);
    AddViewToPageContainer(connectNewPhoneFragment_->GetFragmentView());
    AddViewToPageContainer(connectOpenFragment_->GetFragmentView());
    AddViewToPageContainer(reconnectionFragment_->GetFragmentView());
    AddViewToPageContainer(secondConfirmFragment_->GetFragmentView());
    AddViewToPageContainer(unPairFragment_->GetFragmentView());
    AddViewToPageContainer(successFragment_->GetFragmentView());
    AddViewToPageContainer(failFragment_->GetFragmentView());
}

void SettingBluetoothPage::InitPageView()
{
    scroll_ = new UIScrollViewNested();
    if (scroll_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::scroll_ new fail");
        return;
    }
    scroll_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, HORIZONTAL_RESOLUTION);
    scroll_->SetVerticalScrollState(true);
    scroll_->SetYScrollBarVisible(true);
    scroll_->SetOnDragListener(this);
    scroll_->SetScrollBlankSize(CONTAINER_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    connectNewPhoneFragment_ = new SettingConnectNewPhone();
    connectNewPhoneFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    connectNewPhoneFragment_->CreateView();
    if (connectNewPhoneFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::successFragment_ new fail");
        return;
    }
    connectNewPhoneFragment_->GetFragmentView()->SetVisible(false);

    connectOpenFragment_ = new SettingConnectionOpen();
    connectOpenFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    connectOpenFragment_->CreateView();
    if (connectOpenFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::connectOpenFragment_ new fail");
        return;
    }
    connectOpenFragment_->GetFragmentView()->SetVisible(false);

    reconnectionFragment_ = new SettingReconnection();
    reconnectionFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    reconnectionFragment_->CreateView();
    if (reconnectionFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::reconnectionFragment_ new fail");
        return;
    }
    reconnectionFragment_->GetFragmentView()->SetVisible(false);

    secondConfirmFragment_ = new SettingReconnectSecondConfirm();
    secondConfirmFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    secondConfirmFragment_->CreateView();
    if (secondConfirmFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::secondConfirmFragment_ new fail");
        return;
    }
    secondConfirmFragment_->GetFragmentView()->SetVisible(false);

    unPairFragment_ = new UnPairSecondConfirm();
    unPairFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    unPairFragment_->CreateView();
    if (unPairFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::unPairFragment_ new fail");
        return;
    }
    unPairFragment_->GetFragmentView()->SetVisible(false);

    successFragment_ = new SettingConnectBlueSuccess();
    successFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    successFragment_->CreateView();
    if (successFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::successFragment_ new fail");
        return;
    }
    successFragment_->GetFragmentView()->SetVisible(false);

    failFragment_ = new SettingConnectBlueFail();
    failFragment_->GetFragmentView()->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    failFragment_->CreateView();

    if (failFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::failFragment_ new fail");
        return;
    }
    failFragment_->GetFragmentView()->SetVisible(false);

    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(0, SETTING_TITLE_Y, RESOLUTION_WIDTH, SETTING_TITLE_HEIGHT);
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_TITLE_FONT);
    titleLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    titleLabel_->SetText("蓝牙");

    availableDeviceLabel_ = new UILabel();
    if (availableDeviceLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::availableDeviceLabel_ new fail");
        return;
    }
    availableDeviceLabel_->SetPosition(0, SETTING_AVILABLE_Y, RESOLUTION_WIDTH, SETTING_AVILABLE_HEIGHT);
    availableDeviceLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    availableDeviceLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_AVILABLE_FONT);
    availableDeviceLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    availableDeviceLabel_->SetText("可用设备");

    // 根据list数量下移
    searchBtnTextLabel_ = new UILabel();
    if (searchBtnTextLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::searchBtnTextLabel_ new fail");
        return;
    }
    searchBtnTextLabel_->SetPosition(0, SEARCH_BTN_Y, RESOLUTION_WIDTH, SETTING_SEATCHBTN_HEIGHT);
    searchBtnTextLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    searchBtnTextLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SEATCHBTN_FONT);
    searchBtnTextLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    searchBtnTextLabel_->SetText("正在搜索...");
    searchBtnTextLabel_->SetVisible(true);

    noDeviceLabel_ = new UILabel();
    if (noDeviceLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::noDeviceLabel_ new fail");
        return;
    }
    noDeviceLabel_->SetPosition(0, SETTING_NODEVICE_Y, RESOLUTION_WIDTH, SETTING_SEATCHBTN_HEIGHT);
    noDeviceLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    noDeviceLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_NODEVICE_FONT);
    noDeviceLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    noDeviceLabel_->SetText("未发现附近有可用设备");
    noDeviceLabel_->SetVisible(false);

    resetResearchButton_ = new UILabelButton();
    if (resetResearchButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::resetResearchButton_ new fail");
        return;
    }
    resetResearchButton_->SetPosition(SETTING_RESEARCH_X, SETTING_RESEARCH_Y, SETTING_RESEARCH_WIDTH,
                                      SETTING_RESEARCH_HEIGHT);
    resetResearchButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    resetResearchButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_AVILABLE_FONT);
    resetResearchButton_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    resetResearchButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    resetResearchButton_->SetText("重新搜索");
    resetResearchButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::PRESSED);
    resetResearchButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::RELEASED);
    resetResearchButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::INACTIVE);
    resetResearchButton_->SetViewId(RESEARCH_BUTTON);
    resetResearchButton_->SetOnClickListener(this);
    resetResearchButton_->SetVisible(false);

    optionList_ = new UISimpleList();
    if (optionList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::optionList_ new fail");
        return;
    }
    optionList_->SetPosition(SETTING_LIST_POSITION_X, SETTING_LIST_POSITION_Y);
    optionList_->Resize(LIST_WIDTH, LIST_CONTENT_HEIGHT);
    optionList_->SetDraggable(false);
    optionList_->SetScrollBlankSize(0);
    optionList_->SetMaxScrollDistance(0);
    optionList_->SetViewId(OPTION_LIST);

    btEarPhoneList_ = new UISimpleList();
    if (btEarPhoneList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitPageView::btEarPhoneList_ new fail");
        return;
    }
    btEarPhoneList_->SetPosition(SETTING_LIST_POSITION_X, SETTING_NODEVICE_Y);
    btEarPhoneList_->SetScrollBlankSize(0);
    btEarPhoneList_->SetMaxScrollDistance(0);
    btEarPhoneList_->SetVisible(true);
    btEarPhoneList_->SetViewId(BT_LIST);

    scroll_->Add(titleLabel_);
    scroll_->Add(availableDeviceLabel_);
    scroll_->Add(searchBtnTextLabel_);
    scroll_->Add(optionList_);
    scroll_->Add(resetResearchButton_);
    scroll_->Add(noDeviceLabel_);
    scroll_->Add(btEarPhoneList_);
}

void SettingBluetoothPage::LoadFindDeviceList()
{
    std::list<BtCaseInfo> &bluetoothCaseList = SettingBluetoothModel::GetInstance()->GetScansDevicesList();
    btEarPhoneList_->ClearAll();
    for (auto& it : bluetoothCaseList) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadFindDeviceList addr: %02x%02x%02x%02x%02x%02x",
            it.addr[SETTING_ADDR5], it.addr[SETTING_ADDR4], it.addr[SETTING_ADDR3],
            it.addr[SETTING_ADDR2], it.addr[1], it.addr[0]);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadFindDeviceList connect_status:%s", it.connect_status.c_str());
        Contents cont;
        cont.createFunc = SettingBluetoothPage::CreateForBtList;
        cont.updateFunc = SettingBluetoothPage::UpdateForBtList;
        cont.type = 1;
        cont.data = &it;
        btEarPhoneList_->AddContent(cont);
    }
    btEarPhoneList_->Resize(SETTING_BTLIST_X, SETTING_BTLIST_Y * bluetoothCaseList.size());
    btEarPhoneList_->SetDraggable(false);
    btEarPhoneList_->EnableAutoAlign(true);
    btEarPhoneList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    btEarPhoneList_->RefreshList();
    UpdataImageAnimatorAndList();
    scroll_->Invalidate();
}

void SettingBluetoothPage::InitImageAnimator()
{
    imageAnimator_ = new UIImageAnimatorView();
    if (imageAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::InitImageAnimator::imageAnimator_ new fail");
        return;
    }

    imageAnimator_->SetPosition(SETTING_IMGANI_X, SETTING_IMGANI_Y, SETTING_IMGANI_HEIGHT, SETTING_IMGANI_HEIGHT);
    for (uint8_t i = 0; i < SEARCH_IMAGE_COUNT; i++) {
        searchImageInfo_[i].imageInfo =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(g_imageAnimator[i], SETTING_IMAGE);
        searchImageInfo_[i].imageType = IMG_SRC_IMAGE_INFO;
        searchImageInfo_[i].pos.x = SETTING_IMGANI_X;
        searchImageInfo_[i].pos.y = SETTING_IMGANI_Y;
        searchImageInfo_[i].width = SETTING_IMGANI_HEIGHT;
        searchImageInfo_[i].height = SETTING_IMGANI_HEIGHT;
    }
    imageAnimator_->SetImageAnimatorSrc(searchImageInfo_, SEARCH_IMAGE_COUNT, SETTING_IMGANI_UPDATE);
    imageAnimator_->SetRepeat(true);
    imageAnimator_->SetAutoEnable(false);
    imageAnimator_->SetResizeMode(UIImageView::FILL);
    imageAnimator_->SetVisible(true);
    imageAnimator_->SetSizeFixed(true);
    scroll_->Add(imageAnimator_);
}

void SettingBluetoothPage::UpdataImageAnimatorAndList()
{
    int16_t imageAnimatorPosY = btEarPhoneList_->GetY() +
            ((SettingBluetoothModel::GetInstance()->GetScansDevicesList().size()) * SETTING_ITEM_HEIGHT_);
    imageAnimator_->SetPosition(SETTING_IMGANI_X, imageAnimatorPosY, SETTING_IMGANI_HEIGHT, SETTING_IMGANI_HEIGHT);
    for (uint8_t i = 0; i < SEARCH_IMAGE_COUNT; i++) {
        searchImageInfo_[i].imageInfo =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(g_imageAnimator[i], SETTING_IMAGE);
        searchImageInfo_[i].imageType = IMG_SRC_IMAGE_INFO;
        searchImageInfo_[i].pos.x = SETTING_IMGANI_X;
        searchImageInfo_[i].pos.y = imageAnimatorPosY;
        searchImageInfo_[i].width = SETTING_IMGANI_HEIGHT;
        searchImageInfo_[i].height = SETTING_IMGANI_HEIGHT;
    }
    imageAnimator_->SetImageAnimatorSrc(searchImageInfo_, SEARCH_IMAGE_COUNT, SETTING_IMGANI_UPDATE);
    imageAnimator_->SetRepeat(true);
    imageAnimator_->SetAutoEnable(false);
    imageAnimator_->SetResizeMode(UIImageView::FILL);
    imageAnimator_->SetSizeFixed(true);
    imageAnimator_->Invalidate();
    imageAnimator_->Start();
    searchBtnTextLabel_->SetPosition(0, imageAnimator_->GetY() + SETTING_IMGANI_HEIGHT);
    resetResearchButton_->SetPosition(SETTING_RESEARCH_X, imageAnimator_->GetY() + SETTING_RESETTESEARCH_OFFSE5T);
    optionList_->SetPosition(SETTING_LIST_POSITION_X, imageAnimator_->GetY() + SETTING_CONTENT_OFFSE5T_NOSEARCH);
    scroll_->Invalidate();
}

void SettingBluetoothPage::BtAnimatorStart(void)
{
    imageAnimator_->Start();
}

void SettingBluetoothPage::BtAnimatorStop(void)
{
    imageAnimator_->Stop();
}

void SettingBluetoothPage::NotFindDevice()
{
    BtAnimatorStop();
    searchBtnTextLabel_->SetVisible(false);
    imageAnimator_->SetVisible(false);
    noDeviceLabel_->SetVisible(true);
    resetResearchButton_->SetVisible(true);
    scroll_->Invalidate();
}

void SettingBluetoothPage::TimerFindDevice()
{
    BtAnimatorStop();
    searchBtnTextLabel_->SetVisible(false);
    imageAnimator_->SetVisible(false);
    noDeviceLabel_->SetVisible(false);
    resetResearchButton_->SetVisible(true);
    scroll_->Invalidate();
}

void SettingCancelBrScanHandle(void)
{
    if (g_settingStopScanHandle != nullptr) {
        g_settingStopScanHandle->Stop();
        delete g_settingStopScanHandle;
        g_settingStopScanHandle = nullptr;
    }
    bool results = gap_br_cancel_discovery();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingCancelBrScanHandle CancelBtDiscovery results %d", results);
}

void SettingEndScanHandle(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
        "SettingBluetoothModel::GetInstance()->GetScansDevicesList().size() results %d",
        SettingBluetoothModel::GetInstance()->GetScansDevicesList().size());
    SettingBluetoothPage* bluetoothPage = SettingBluetoothPage::GetInstance();
    if (bluetoothPage == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bluetoothPage nullptr");
        return;
    }
    if (SettingBluetoothModel::GetInstance()->GetScansDevicesList().size() > 0) {
        bluetoothPage->TimerFindDevice();
    } else {
        // 未发现蓝牙耳机
        bluetoothPage->NotFindDevice();
    }
}

// 定时器回调函数
static void SettingBluetoothScanTimerCallbackProc(void* data)
{
    (void)data;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Bt scan end jumpView");
    SettingCancelBrScanHandle();
    SettingEndScanHandle();
}

void SettingBluetoothScanTimerCallback(void* data)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(SettingBluetoothScanTimerCallbackProc, data));
}

void SettingBluetoothPage::BtScan()
{
    bool ret;
    bool results;
    SetScanStatus(true);
    BtAnimatorStart();
    ret = is_bt_discovering(BT_TRANSPORT_BR_EDR);
    if (ret) {
        results = gap_br_cancel_discovery();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtScan CancelBtDiscovery results %d", results);
    }

    results = gap_br_set_inquiry_paramters(DEVICE_MAJOR_CLASS_AUDIO_VIDEO, 0);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
        "SettingBluetoothPage gap_br_set_inquiry_paramters results %d", results);
    results = gap_br_start_discovery();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
        "SettingBluetoothPage gap_br_start_discovery results %d", results);
    if (g_settingStopScanHandle != nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BluetoothEarPhoneView::BtScan g_settingStopScanHandle != nullptr");
        return;
    }
    uint32_t tick = GetOSTick(SCAN_TIME);
    g_settingStopScanHandle = new GraphicTimer(tick, SettingBluetoothScanTimerCallback, nullptr, true);
    if (g_settingStopScanHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BluetoothEarPhoneView::BtScan new GraphicTimer failed!!");
        return;
    }
    bool retTimer = g_settingStopScanHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothEarPhoneView::BtScan Start failed");
        if (g_settingStopScanHandle != nullptr) {
            delete g_settingStopScanHandle;
            g_settingStopScanHandle = nullptr;
        }
        return;
    }
    return;
}

void SettingBluetoothPage::StopBtScan()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothEarPhoneView::StopBtScan");
    if (imageAnimator_->GetState() == true) {
        SettingCancelBrScanHandle();
        SettingEndScanHandle();
    }
}

void SettingBluetoothPage::DestoryPageView()
{
    if (scroll_ != nullptr) {
        scroll_->RemoveAll();
        delete scroll_;
        scroll_ = nullptr;
    }
    if (connectNewPhoneFragment_ != nullptr) {
        connectNewPhoneFragment_->DestroyView();
        delete connectNewPhoneFragment_;
        connectNewPhoneFragment_ = nullptr;
    }
    if (connectOpenFragment_ != nullptr) {
        connectOpenFragment_->DestroyView();
        delete connectOpenFragment_;
        connectOpenFragment_ = nullptr;
    }
    if (reconnectionFragment_ != nullptr) {
        reconnectionFragment_->DestroyView();
        delete reconnectionFragment_;
        reconnectionFragment_ = nullptr;
    }
    if (secondConfirmFragment_ != nullptr) {
        secondConfirmFragment_->DestroyView();
        delete secondConfirmFragment_;
        secondConfirmFragment_ = nullptr;
    }
    if (unPairFragment_ != nullptr) {
        unPairFragment_->DestroyView();
        delete unPairFragment_;
        unPairFragment_ = nullptr;
    }
    if (successFragment_ != nullptr) {
        successFragment_->DestroyView();
        delete successFragment_;
        successFragment_ = nullptr;
    }
    if (failFragment_ != nullptr) {
        failFragment_->DestroyView();
        delete failFragment_;
        failFragment_ = nullptr;
    }
    if (optionList_ != nullptr) {
        delete optionList_;
        optionList_ = nullptr;
    }
    if (btEarPhoneList_ != nullptr) {
        delete btEarPhoneList_;
        btEarPhoneList_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (noDeviceLabel_ != nullptr) {
        delete noDeviceLabel_;
        noDeviceLabel_ = nullptr;
    }
    if (searchBtnTextLabel_ != nullptr) {
        delete searchBtnTextLabel_;
        searchBtnTextLabel_ = nullptr;
    }
    if (resetResearchButton_ != nullptr) {
        delete resetResearchButton_;
        resetResearchButton_ = nullptr;
    }
    if (availableDeviceLabel_ != nullptr) {
        delete availableDeviceLabel_;
        availableDeviceLabel_ = nullptr;
    }
    if (imageAnimator_ != nullptr) {
        delete imageAnimator_;
        imageAnimator_ = nullptr;
    }
}

UIView* SettingBluetoothPage::CreateForContent(uint8_t type)
{
    if (type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        IconTextNavigationItemView* item = new IconTextNavigationItemView();
        if (item == nullptr) {
            return nullptr;
        }
        item->SetOnClickListener(g_pSettingBluetoothPage);
        return item;
    }
    return nullptr;
}

void SettingBluetoothPage::UpdateForContent(UIView* view, void* data, uint8_t type)
{
    UNUSED(type);
    IconTextNavigationItemView* itemGroup = dynamic_cast<IconTextNavigationItemView*>(view);
    if (itemGroup == nullptr || data == nullptr) {
        return;
    }
    IconTextNavigationItemData* itemData = static_cast<IconTextNavigationItemData*>(data);
    itemGroup->UpdateViewData(itemData);
}

UIView* SettingBluetoothPage::CreateForBtList(uint8_t type)
{
    UNUSED(type);
    return new SettingBluetoothItemGroup();
}

void SettingBluetoothPage::UpdateForBtList(UIView* view, void* data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SettingBluetoothItemGroup* itemGroup =  dynamic_cast<SettingBluetoothItemGroup*>(view);
    if (itemGroup == nullptr) {
        return;
    }
    BtCaseInfo* caseInfo = static_cast<BtCaseInfo*>(data);
    itemGroup->SetItemInfo(caseInfo);
    itemGroup->SetOnClickListener(g_pSettingBluetoothPage);
}

void SettingBluetoothPage::UpdateBtListContent(int16_t viewIndex)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdateBtListContent");
    std::string deviceName = SettingBluetoothModel::GetInstance()->GetDeviceName();
    SettingBluetoothModel::GetInstance()->UpdateConnectStatusForDevice();
    std::list<BtCaseInfo> bluetoothCaseList = SettingBluetoothModel::GetInstance()->GetScansDevicesList();
    for (auto& it : bluetoothCaseList) {
        if (std::strcmp(reinterpret_cast<const char*>(it.deviceName), deviceName.c_str()) == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdateBtListContent addr: %02x%02x%02x%02x%02x%02x",
                it.addr[SETTING_ADDR5], it.addr[SETTING_ADDR4], it.addr[SETTING_ADDR3],
                it.addr[SETTING_ADDR2], it.addr[1], it.addr[0]);
            Contents cont;
            cont.createFunc = SettingBluetoothPage::CreateForBtList;
            cont.updateFunc = SettingBluetoothPage::UpdateForBtList;
            cont.type = 1;
            cont.data = &it;
            btEarPhoneList_->UpdateContent(viewIndex, cont);
            break;
        }
    }
    btEarPhoneList_->RefreshList();
}

bool SettingBluetoothPage::IsDeviceConnect()
{
    std::list<BtCaseInfo> bluetoothCaseList = SettingBluetoothModel::GetInstance()->GetScansDevicesList();
    for (auto& it : bluetoothCaseList) {
        if (it.connect_status == "已连接") {
            return true;
        }
    }
    return false;
}

bool SettingBluetoothPage::GetImageAnimatorState()
{
    return imageAnimator_->GetState();
}

void SettingBluetoothPage::ChangeView(int16_t value)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ChangeView value: %d", value);
    if (value == BT_MAIN_PAGE) {
        scroll_->SetVisible(true);
    } else if (value == CONNECT_OPEN_FRAGMENT) {
        connectOpenFragment_->RefreshFragment();
        SettingBluetoothModel::GetInstance()->SetConnectFragmentStatus(true);
        connectOpenFragment_->GetFragmentView()->SetVisible(true);
    } else if (value == RECONNECT_SECOND_CONFIRM_FRAGMENT) {
        secondConfirmFragment_->RefreshFragment();
        secondConfirmFragment_->GetFragmentView()->SetVisible(true);
    } else if (value == UNPAIR_CONFIRM_FRAGMENT) {
        unPairFragment_->RefreshFragment();
        unPairFragment_->GetFragmentView()->SetVisible(true);
    }
    if (imageAnimator_->GetState() == false) {
        StartBtScan();
    }
}

void SettingBluetoothPage::ShowResultGroup(int16_t value)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ShowResultGroup value: %d", value);
    StopBtScan();
    if (value == CONNECT_BLUETOOTH_FAIL_FRAGMENT) {
        failFragment_->RefreshFragment();
        failFragment_->GetFragmentView()->SetVisible(true);
    } else if (value == CONNECT_BLUETOOTH_SUCCESS_FRAGMENT) {
        successFragment_->RefreshFragment();
        successFragment_->GetFragmentView()->SetVisible(true);
    }
}

void SettingBluetoothPage::StartBtScan()
{
    imageAnimator_->SetVisible(true);
    searchBtnTextLabel_->SetVisible(true);
    noDeviceLabel_->SetVisible(false);
    resetResearchButton_->SetVisible(false);
    optionList_->SetPosition(SETTING_LIST_POSITION_X,
        searchBtnTextLabel_->GetY() + SETTING_CONTENT_OFFSE5T_SEARCH);
    BtScan();
}

bool SettingBluetoothPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), RESEARCH_BUTTON) == 0) {
        StartBtScan();
    } else if (strcmp(view.GetParent()->GetViewId(), OPTION_LIST) == 0) {
        int16_t viewIndex = view.GetViewIndex();
        if (viewIndex < 0 || viewIndex >= g_bluetoothOptionData.size()) {
            return true;
        }
        IconTextNavigationItemData itemData = g_bluetoothOptionData[viewIndex];
        if (itemData.pageId == SettingPageId::CONNECT_NEW_PHONE_PAGE) {
            StopBtScan();
            scroll_->SetVisible(false);
            connectNewPhoneFragment_->GetFragmentView()->SetVisible(true);
        } else {
            NativeAbility::GetInstance().SwitchPageInSlice(itemData.pageId,
                TransitionType::TRANSITION_INVALID, true);
        }
    } else if (strcmp(view.GetParent()->GetViewId(), BT_LIST) == 0) {
        // 点击搜索到的设备进行连接
        SettingBluetoothItemGroup *listItemViewID = dynamic_cast<SettingBluetoothItemGroup *>(&view);
        BtCaseInfo item = listItemViewID->GetItem();
        SettingBluetoothModel::GetInstance()->SetSelectedDeviceInfo(&item);
        std::string connState = item.connect_status;
        bd_addr_t addr;
        bool flag_connect = IsDeviceConnect();
        // 当前设备为已连接 或没有设备被连接时 才会处理点击事件
        if ((connState == "已连接") || (!flag_connect)) {
            SettingBluetoothModel::GetInstance()->SetDeviceName(reinterpret_cast<const char*>(item.deviceName));
            (void)memcpy_s(addr.addr, BD_ADDR_LEN, item.addr, BD_ADDR_LEN);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBluetoothPage::OnClick addr: %02x%02x%02x%02x%02x%02x",
                addr.addr[SETTING_ADDR5], addr.addr[SETTING_ADDR4], addr.addr[SETTING_ADDR3],
                addr.addr[SETTING_ADDR2], addr.addr[1], addr.addr[0]);
            if (connState == "") {
                errcode_t retVal = SettingBluetoothModel::GetInstance()->ConnectRemoteDevice(&addr);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ConnectRemoteDevice ret =  %d", retVal);
                if (retVal != 0) {
                    UpdateBtListContent(view.GetViewIndex());
                    SettingBluetoothModel::GetInstance()->SetConnectResultPage(true);
                }
            } else if (connState == "已连接" || connState == "连接已经断开") {
                StopBtScan();
                scroll_->SetVisible(false);
                reconnectionFragment_->RefreshFragment();
                reconnectionFragment_->GetFragmentView()->SetVisible(true);
            }
        }
    }
    scroll_->Invalidate();
    return true;
}
}
