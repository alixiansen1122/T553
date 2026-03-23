/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "PullUpDownView.h"
#include "UiConfig.h"
#include "common/screen.h"
#include "ScreenModel.h"
#include "ui_resource_image.h"
#include "common/image_cache_manager.h"

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
constexpr int16_t BATTERY_IMG_X = 180;
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

constexpr int16_t BATTERY_LABEL_X = 232;
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
    GRAPHIC_LOGD("PullUpDownView::PullUpDownView\n");
    g_pPullUpDownView = this;
}

PullUpDownView::~PullUpDownView()
{
    GRAPHIC_LOGD("PullUpDownView::~PullUpDownView\n");
    RemoveAll();
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
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(DROPDOWN_IAMGES);
}

PullUpDownView *PullUpDownView::GetInstance(void)
{
    return g_pPullUpDownView;
}

void PullUpDownView::InitViewImg(void)
{
    finPhone = new UIImageView();
    if (finPhone == nullptr) {
        GRAPHIC_LOGE("finPhone new fail\n");
        return;
    }
    finPhone->SetPosition(PHONE_IMG_X, PHONE_IMG_Y);
    ScreenModel::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, finPhone, DROPDOWN_DROPDOWN_FIND_PHONE);
    finPhone->SetTouchable(true);

    droAlarm = new UIImageView();
    if (droAlarm == nullptr) {
        GRAPHIC_LOGE("droAlarm new fail\n");
        return;
    }
    droAlarm->SetPosition(ALARM_IMG_X, ALARM_IMG_Y);
    ScreenModel::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, droAlarm, DROPDOWN_DROPDOWN_ALARM);
    droAlarm->SetTouchable(true);

    droSetting = new UIImageView();
    if (droSetting == nullptr) {
        GRAPHIC_LOGE("droSetting new fail\n");
        return;
    }
    droSetting->SetPosition(SETTING_IMG_X, SETTING_IMG_Y);
    ScreenModel::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, droSetting, DROPDOWN_DROPDOWN_SETTING);
    droSetting->SetTouchable(true);
}

void PullUpDownView::InitViewLable(void)
{
    dnotDisturbLable = new UILabel();
    if (dnotDisturbLable == nullptr) {
        GRAPHIC_LOGE("dnotDisturbLable new fail\n");
        return;
    }
    dnotDisturbLable->SetPosition(DISTURB_LABEL_X, DISTURB_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    dnotDisturbLable->SetText("勿扰");
    dnotDisturbLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    dnotDisturbLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    briScreenLable = new UILabel();
    if (briScreenLable == nullptr) {
        GRAPHIC_LOGE("briScreenLable new fail\n");
        return;
    }
    briScreenLable->SetPosition(SCREEN_LABEL_X, SCREEN_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    briScreenLable->SetText("亮屏");
    briScreenLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    briScreenLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    finPhoneLable = new UILabel();
    if (finPhoneLable == nullptr) {
        GRAPHIC_LOGE("finPhoneLable new fail\n");
        return;
    }
    finPhoneLable->SetPosition(PHONE_LABEL_X, PHONE_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    finPhoneLable->SetText("找手机");
    finPhoneLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    finPhoneLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    droAlarmLable = new UILabel();
    if (finPhoneLable == nullptr) {
        GRAPHIC_LOGE("finPhoneLable new fail\n");
        return;
    }
    droAlarmLable->SetPosition(ALARM_LABEL_X, ALARM_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    droAlarmLable->SetText("闹钟");
    droAlarmLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    droAlarmLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    droSettingLable = new UILabel();
    if (droSettingLable == nullptr) {
        GRAPHIC_LOGE("droSettingLable new fail\n");
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
        GRAPHIC_LOGE("bluetooth new fail\n");
        return;
    }
    bluetooth->SetPosition(BLUETOOTH_IMG_X, BLUETOOTH_IMG_Y);
    ScreenModel::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, bluetooth, DROPDOWN_DROPDOWN_BLUETOOTH);

    battery = new UIImageView(); // 电量状态显示
    if (battery == nullptr) {
        GRAPHIC_LOGE("battery new fail\n");
        return;
    }
    battery->SetPosition(BATTERY_IMG_X, BATTERY_IMG_Y);
    ScreenModel::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, battery, DROPDOWN_DROPDOWN_BATTERY);
    batteryLable = new UILabel();
    if (batteryLable == nullptr) {
        GRAPHIC_LOGE("batteryLable new fail\n");
        return;
    }
    batteryLable->SetPosition(BATTERY_LABEL_X, BATTERY_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    batteryLable->SetText("90%");
    batteryLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    batteryLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    dateLable = new UILabel();
    if (dateLable == nullptr) {
        GRAPHIC_LOGE("dateLable new fail\n");
        return;
    }
    dateLable->SetPosition(DATE_LABEL_X, DATE_LABEL_Y, 192, TEXT_HEIGHT); // TEXT_WIDTH = 192
    dateLable->SetText("10/23 星期五");
    dateLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    dateLable->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
}

bool PullUpDownView::InitView()
{
    InitViewImg();
    InitViewLable();
    InitViewInfo();

    Add(dnotDisturbLable);
    Add(briScreenLable);
    Add(finPhone);
    Add(finPhoneLable);
    Add(droAlarm);
    Add(droAlarmLable);
    Add(droSetting);
    Add(droSettingLable);
    Add(battery);
    Add(batteryLable);
    Add(dateLable);
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    return true;
}

void PullUpDownView::UpdateBlueToothStatus(int status)
{
    if (bluetooth == nullptr) {
        GRAPHIC_LOGE("bluetooth nullptr\n");
        return;
    }

    ScreenModel::GetInstance()->LoadAppImages(DROPDOWN_IAMGES, bluetooth, DROPDOWN_DROPDOWN_BLUETOOTH);
    return;
}

}
