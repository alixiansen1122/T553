/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBluetoothItemView
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "ui_resource_settings.h"
#include "common/image_cache_manager.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/common/SettingBluetoothItemGroup.h"

namespace OHOS {
static constexpr int16_t BLUETOOTH_ITEM_X = 14;
static constexpr int16_t BLUETOOTH_ITEM_WIDTH = 426;
static constexpr int16_t BLUETOOTH_ITEM_HEIGHT = 129;
static constexpr int16_t BLUETOOTH_ITEM_ICON1_X = 30;
static constexpr int16_t BLUETOOTH_ITEM_ICON1_Y = 35;
static constexpr int16_t BLUETOOTH_ITEM_ICON2_X = 378;
static constexpr int16_t BLUETOOTH_ITEM_ICON2_Y = 46;
static constexpr int16_t BLUETOOTH_ITEM_LABEL1_X = 110;
static constexpr int16_t BLUETOOTH_ITEM_LABEL1_Y = 15;
static constexpr int16_t BLUETOOTH_ITEM_LABEL1_STATUS_EXIST_Y = 33;
static constexpr int16_t BLUETOOTH_ITEM_LABEL2_Y = 57;
static constexpr int16_t BLUETOOTH_ITEM_CHARTLINE_X = 20;
static constexpr int16_t BLUETOOTH_ITEM_CHARTLINE_Y = 78;
static constexpr int16_t BLUETOOTH_ITEM_CHARTLINE_WIDTH = 450;
static constexpr int16_t BLUETOOTH_ITEM_CHARTLINE_HEIGHT = 2;
static constexpr int16_t BLUETOOTH_ITEM_ITEM_BUTTON_WIDTH = 390;
static constexpr int16_t BLUETOOTH_ITEM_ITEM_BUTTON_HEIGHT = 83;
static constexpr int16_t BLUETOOTH_ITEM_ITEM_LABEL_WIDTH = 240;
static constexpr int16_t BLUETOOTH_ITEM_ITEM_LABEL_HEIGHT = 40;
static constexpr int16_t BLUETOOTH_ITEM_ITEM_FONT_SIZE = 32;
static constexpr int16_t BLUETOOTH_ITEM_IMAGE_SIZE = 300;
static constexpr int16_t BLUETOOTH_ITEM_CONNSTATE_SIZE = 30;

SettingBluetoothItemGroup::SettingBluetoothItemGroup()
{
    SetPosition(BLUETOOTH_ITEM_X, 0, BLUETOOTH_ITEM_WIDTH, BLUETOOTH_ITEM_HEIGHT);

    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    SetTouchable(true);
    SetUpChild();
    ImageInfo* image =
        ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_BG, SETTING_IMAGE);
    background_.SetSrc(image);
    background_.SetPosition(0, 0);

    Add(&background_);
    Add(&icon_);
    Add(&deviceName_);
    Add(&connectStatus_);
    Add(&conn_);
}

SettingBluetoothItemGroup::~SettingBluetoothItemGroup()
{
    Remove(&background_);
    Remove(&icon_);
    Remove(&deviceName_);
    Remove(&connectStatus_);
    Remove(&conn_);
}

void SettingBluetoothItemGroup::SetItemInfo(const BtCaseInfo* itemInfo)
{
    if (memcpy_s(itemInfo_.deviceName, BD_NAME_LEN, itemInfo->deviceName, BD_NAME_LEN) != 0) {
        return;
    }
    if (memcpy_s(itemInfo_.addr, BD_ADDR_LEN, itemInfo->addr, BD_ADDR_LEN) != 0) {
        return;
    }
    itemInfo_.connect_status = itemInfo->connect_status;
    itemInfo_.rssi = itemInfo->rssi;
    deviceName_.SetText((const char*)itemInfo_.deviceName);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBluetoothItemGroup::SetItemInfo %s", itemInfo_.deviceName);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBluetoothItemGroup::connect_status %s", itemInfo_.connect_status.c_str());
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_BLUETOOTH, SETTING_IMAGE);
    icon_.SetSrc(image);
    connectStatus_.SetText(itemInfo_.connect_status.c_str());
    if (itemInfo_.connect_status == "已连接") {
        image = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_CONNECT, SETTING_IMAGE);
        conn_.SetSrc(image);
        conn_.SetVisible(true);
    } else {
        conn_.SetVisible(false);
    }
    if (itemInfo_.connect_status == "") {
        deviceName_.SetPosition(BLUETOOTH_ITEM_LABEL1_X, BLUETOOTH_ITEM_LABEL1_STATUS_EXIST_Y);
        deviceName_.SetVisible(true);
        connectStatus_.SetVisible(false);
    } else {
        deviceName_.SetPosition(BLUETOOTH_ITEM_LABEL1_X, BLUETOOTH_ITEM_LABEL1_Y);
        deviceName_.SetVisible(true);
        connectStatus_.SetVisible(true);
    }
}

BtCaseInfo& SettingBluetoothItemGroup::GetItem()
{
    return itemInfo_;
}

void SettingBluetoothItemGroup::SetUpChild()
{
    icon_.SetPosition(BLUETOOTH_ITEM_ICON1_X, BLUETOOTH_ITEM_ICON1_Y);
    icon_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    conn_.SetPosition(BLUETOOTH_ITEM_ICON2_X, BLUETOOTH_ITEM_ICON2_Y);
    conn_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    deviceName_.Resize(BLUETOOTH_ITEM_ITEM_LABEL_WIDTH, BLUETOOTH_ITEM_ITEM_LABEL_HEIGHT);
    deviceName_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLUETOOTH_ITEM_ITEM_FONT_SIZE);
    deviceName_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    deviceName_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    deviceName_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    deviceName_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    deviceName_.SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);

    connectStatus_.SetPosition(BLUETOOTH_ITEM_LABEL1_X, BLUETOOTH_ITEM_LABEL2_Y);
    connectStatus_.Resize(BLUETOOTH_ITEM_ITEM_LABEL_WIDTH, BLUETOOTH_ITEM_ITEM_LABEL_HEIGHT);
    connectStatus_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLUETOOTH_ITEM_CONNSTATE_SIZE);
    connectStatus_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    connectStatus_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    connectStatus_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    connectStatus_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
}
}