/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PullUpDownView
 * Create: 2025-06-29
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "common/screen.h"
#include "main/MainPresenterSample.h"
#include "screensetting/ScreenModels.h"
#include "settings/model/SettingBluetoothModel.h"
#include "ui_resource_image.h"
#include "main/PullUpDownView.h"

namespace OHOS {

static PullUpDownView *g_pPullUpDownView = nullptr;

constexpr int16_t DISTURB_IMG_X = 117;
constexpr int16_t DISTURB_IMG_Y = 50;
constexpr int16_t SCREEN_IMG_X = 251;
constexpr int16_t SCREEN_IMG_Y = 50;
constexpr int16_t PHONE_IMG_X = 49;
constexpr int16_t PHONE_IMG_Y = 184;
constexpr int16_t ALARM_IMG_X = 184;
constexpr int16_t ALARM_IMG_Y = 184;
constexpr int16_t SETTING_IMG_X = 319;
constexpr int16_t SETTING_IMG_Y = 184;

constexpr int16_t BLUETOOTH_IMG_X = 145;
constexpr int16_t BLUETOOTH_IMG_Y = 335;
constexpr int16_t BATTERY_IMG_X = 195;
constexpr int16_t BATTERY_IMG_Y = 340;

constexpr int16_t DISTURB_LABEL_X = 136;
constexpr int16_t DISTURB_LABEL_Y = 145;
constexpr int16_t SCREEN_LABEL_X = 270;
constexpr int16_t SCREEN_LABEL_Y = 145;
constexpr int16_t PHONE_LABEL_X = 68;
constexpr int16_t PHONE_LABEL_Y = 279;
constexpr int16_t ALARM_LABEL_X = 203;
constexpr int16_t ALARM_LABEL_Y = 279;
constexpr int16_t SETTING_LABEL_X = 338;
constexpr int16_t SETTING_LABEL_Y = 279;

constexpr int16_t BATTERY_LABEL_X = 247;
constexpr int16_t BATTERY_LABEL_Y = 335;
constexpr int16_t DATE_LABEL_X = 173;
constexpr int16_t DATE_LABEL_Y = 374;

constexpr int16_t FONT_DEFAULT_SIZE = 24;
constexpr int16_t TEXT_WIDTH = 86;
constexpr int16_t TEXT_HEIGHT = 30;

constexpr int16_t NONOTICE_LABLE_X = 141;
constexpr int16_t NONOTICE_LABLE_Y = 212;

PullUpDownView::PullUpDownView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PullUpDownView::PullUpDownView");
    g_pPullUpDownView = this;
}

PullUpDownView::~PullUpDownView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PullUpDownView::~PullUpDownView");
    RemoveAll();
    delete dnotDisturb;
    dnotDisturb = nullptr;
    delete briScreen;
    briScreen = nullptr;
    delete finPhone;
    finPhone = nullptr;
    delete droAlarm;
    droAlarm = nullptr;
    delete droSetting;
    droSetting = nullptr;
    delete dnotDisturbLable;
    dnotDisturbLable = nullptr;
    delete briScreenLable;
    briScreenLable = nullptr;
    delete finPhoneLable;
    finPhoneLable = nullptr;
    delete droAlarmLable;
    droAlarmLable = nullptr;
    delete droSettingLable;
    droSettingLable = nullptr;
    delete bluetooth;
    bluetooth = nullptr;
    delete battery;
    battery = nullptr;
    delete batteryLable;
    batteryLable = nullptr;
    delete dateLable;
    dateLable = nullptr;
    g_pPullUpDownView = nullptr;
}

PullUpDownView *PullUpDownView::GetInstance(void)
{
    return g_pPullUpDownView;
}

void PullUpDownView::InitSubViewImg(void)
{
    if (HealthModel::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "health model nullptr");
        return;
    }

    dnotDisturb = new UIImageView();
    if (dnotDisturb == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "dnotDisturb new fail");
        return;
    }
    dnotDisturb->SetPosition(DISTURB_IMG_X, DISTURB_IMG_Y);
    if (HealthModel::GetInstance()->GetDntDistStatus() == 0) {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, dnotDisturb, DROPDOWN_DROPDOWN_DN_DISTURB);
    } else {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, dnotDisturb, DROPDOWN_DROPDOWN_DN_DISTURB_B);
    }
    dnotDisturb->SetViewId(DROPDOWN_CLICK_DONOTDISTURB);
    dnotDisturb->SetTouchable(true);
    dnotDisturb->SetOnClickListener(clickListener); // 进入勿扰模式

    briScreen = new UIImageView();
    if (briScreen == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "briScreen new fail");
        return;
    }
    briScreen->SetPosition(SCREEN_IMG_X, SCREEN_IMG_Y);
    if (HealthModel::GetInstance()->GetBriScrStatus() == 0) {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, briScreen, DROPDOWN_DROPDOWN_BRI_SCREEN);
    } else {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, briScreen, DROPDOWN_DROPDOWN_BRI_SCREEN_B);
    }
    briScreen->SetViewId(DROPDOWN_CLICK_BRIGHTENSCREEN);
    briScreen->SetTouchable(true);
    briScreen->SetOnClickListener(clickListener); // 进入亮屏模式
}

void PullUpDownView::InitViewImg(void)
{
    InitSubViewImg();

    finPhone = new UIImageView();
    if (finPhone == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "finPhone new fail");
        return;
    }
    finPhone->SetPosition(PHONE_IMG_X, PHONE_IMG_Y);
    ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, finPhone, DROPDOWN_DROPDOWN_FIND_PHONE);
    finPhone->SetViewId(DROPDOWN_CLICK_FINDMYPHONE); // 找手机界面 DROP_DOWN_FIND_PHONE
    finPhone->SetTouchable(true);
    finPhone->SetOnClickListener(clickListener);

    droAlarm = new UIImageView();
    if (droAlarm == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "droAlarm new fail");
        return;
    }
    droAlarm->SetPosition(ALARM_IMG_X, ALARM_IMG_Y);
    ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, droAlarm, DROPDOWN_DROPDOWN_ALARM);
    droAlarm->SetViewId(DROPDOWN_CLICK_ALARM); // 进入闹钟App DROP_DOWN_ALARM
    droAlarm->SetTouchable(true);
    droAlarm->SetOnClickListener(clickListener);

    droSetting = new UIImageView();
    if (droSetting == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "droSetting new fail");
        return;
    }
    droSetting->SetPosition(SETTING_IMG_X, SETTING_IMG_Y);
    ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, droSetting, DROPDOWN_DROPDOWN_SETTING);
    droSetting->SetViewId(DROPDOWN_CLICK_SETTING); // 进入设置界面 DROP_DOWN_SETTING
    droSetting->SetTouchable(true);
    droSetting->SetOnClickListener(clickListener);
}

void PullUpDownView::InitViewLable(void)
{
    dnotDisturbLable = new UILabel();
    if (dnotDisturbLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "dnotDisturbLable new fail");
        return;
    }
    dnotDisturbLable->SetPosition(DISTURB_LABEL_X, DISTURB_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    dnotDisturbLable->SetText("勿扰");
    dnotDisturbLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    dnotDisturbLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    briScreenLable = new UILabel();
    if (briScreenLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "briScreenLable new fail");
        return;
    }
    briScreenLable->SetPosition(SCREEN_LABEL_X, SCREEN_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    briScreenLable->SetText("亮屏");
    briScreenLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    briScreenLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    finPhoneLable = new UILabel();
    if (finPhoneLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "finPhoneLable new fail");
        return;
    }
    finPhoneLable->SetPosition(PHONE_LABEL_X, PHONE_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    finPhoneLable->SetText("找手机");
    finPhoneLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    finPhoneLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    droAlarmLable = new UILabel();
    if (finPhoneLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "finPhoneLable new fail");
        return;
    }
    droAlarmLable->SetPosition(ALARM_LABEL_X, ALARM_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    droAlarmLable->SetText("闹钟");
    droAlarmLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    droAlarmLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    droSettingLable = new UILabel();
    if (droSettingLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "droSettingLable new fail");
        return;
    }
    droSettingLable->SetPosition(SETTING_LABEL_X, SETTING_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    droSettingLable->SetText("设置");
    droSettingLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    droSettingLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
}

void PullUpDownView::InitViewInfo(void)
{
    bluetooth = new UIImageView(); // 蓝牙状态显示
    if (bluetooth == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetooth new fail");
        return;
    }
    bluetooth->SetPosition(BLUETOOTH_IMG_X, BLUETOOTH_IMG_Y);
    if (SettingBluetoothModel::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetooth model nullptr");
        return;
    }

    if (SettingBluetoothModel::GetInstance()->GetAclConnected()) {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, bluetooth, DROPDOWN_DROPDOWN_BLUETOOTH_B);
    } else {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, bluetooth, DROPDOWN_DROPDOWN_BLUETOOTH);
    }

    battery = new UIImageView(); // 电量状态显示
    if (battery == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "battery new fail");
        return;
    }
    battery->SetPosition(BATTERY_IMG_X, BATTERY_IMG_Y);
    ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, battery, DROPDOWN_DROPDOWN_BATTERY);
    batteryLable = new UILabel();
    if (batteryLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "batteryLable new fail");
        return;
    }
    batteryLable->SetPosition(BATTERY_LABEL_X, BATTERY_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    batteryLable->SetText("90%");
    batteryLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    batteryLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    dateLable = new UILabel();
    if (dateLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "dateLable new fail");
        return;
    }
    dateLable->SetPosition(DATE_LABEL_X, DATE_LABEL_Y, 192, TEXT_HEIGHT); // TEXT_WIDTH = 192
    dateLable->SetText("10/23 星期五");
    dateLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    dateLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
}

bool PullUpDownView::InitView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PullUpDownView::InitDropDownView");
    clickListener = (UIView::OnClickListener *)OHOS::MainPresenterSample::GetInstance();

    InitViewImg();
    InitViewLable();
    InitViewInfo();

    Add(dnotDisturb);
    Add(dnotDisturbLable);
    Add(briScreen);
    Add(briScreenLable);
    Add(finPhone);
    Add(finPhoneLable);
    Add(droAlarm);
    Add(droAlarmLable);
    Add(droSetting);
    Add(droSettingLable);
    Add(bluetooth);
    Add(battery);
    Add(batteryLable);
    Add(dateLable);

    SetTouchable(true);
    SetIntercept(true);

    return true;
}

void PullUpDownView::PreLoad(void)
{
    if (!viewiInitStatus) {
        InitView();
        viewiInitStatus = true;
    }
}

void PullUpDownView::UpdateBlueToothStatus(int status)
{
    if (bluetooth == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetooth nullptr");
        return;
    }
    if (status == 1) {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, bluetooth, DROPDOWN_DROPDOWN_BLUETOOTH_B);
    } else {
        ScreenModels::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, bluetooth, DROPDOWN_DROPDOWN_BLUETOOTH);
    }
    bluetooth->Invalidate();
    return;
}

}
