/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MsgCenter Device
 * Create: 2025-06-06
 */

#ifndef MSG_CENTER_DEVICE_H
#define MSG_CENTER_DEVICE_H

#include "msg_center_protocol.h"
#include "wearable_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum msg_center_device_type_id {
    MSGCENTER_TYPE_ID_GET_BATTERY_LEVEL = 0x01,
    MSGCENTER_TYPE_ID_GET_STORAGE_INFO = 0x02,
    MSGCENTER_TYPE_ID_GET_PRODUCT_INFO = 0x03,
    MSGCENTER_TYPE_ID_GET_LANGUAGE_ID = 0x04,
    MSGCENTER_TYPE_ID_SET_LANGUAGE_ID = 0x05,
    MSGCENTER_TYPE_ID_DEVICE_HEARTRATE = 0x06,
    MSGCENTER_TYPE_ID_DEVICE_STEPDATA = 0x07,
    MSGCENTER_TYPE_ID_DEVICE_BLOODOXYGEN = 0x08,
    MSGCENTER_TYPE_ID_SPORT_ACTION = 0x09,
    MSGCENTER_TYPE_ID_SPORT_REPORT_ACTION = 0x0a,
    MSGCENTER_TYPE_ID_SPORT_REPORT_DATA = 0x0b,
    MSGCENTER_TYPE_ID_SPORT_GET_RECORD = 0x0c,
    MSGCENTER_TYPE_ID_SET_ONLINELOG_CHANNEL = 0x0d,
    MSGCENTER_TYPE_ID_SET_ONLINELOG_LEVEL = 0x0e,
    MSGCENTER_TYPE_ID_OFFLINE_LOG_SWITCH = 0x0f,
    MSGCENTER_TYPE_ID_ONLINELOG_REPORT = 0x10,
    MSGCENTER_TYPE_ID_START_FIND_PHONE = 0x11,
    MSGCENTER_TYPE_ID_STOP_FIND_PHONE = 0x12,
    MSGCENTER_TYPE_ID_SLEEP_DATA = 0x13,
    MSGCENTER_TYPE_ID_ON_CAMERA = 0x14,
    MSGCENTER_TYPE_ID_WATCH_EXIT_CAMERA = 0x15,
    MSGCENTER_TYPE_ID_PHONE_EXIT_CAMERA = 0x16,
    MSGCENTER_TYPE_ID_CAMERA_CONNECTING_PHONE = 0x17,
    MSGCENTER_TYPE_ID_CAMERA_CONNECTED_PHONE = 0x18,
    MSGCENTER_TYPE_ID_OTA_INIT = 0x19,
    MSGCENTER_TYPE_ID_OTA_UPDATE = 0x1a,
    MSGCENTER_TYPE_ID_SYNC_WEATHER_REQ = 0x1b,
    MSGCENTER_TYPE_ID_SYNC_WEATHER_REC = 0x1c,
    MSGCENTER_TYPE_ID_SYNC_DATE_REQ = 0x1d,
    MSGCENTER_TYPE_ID_SYNC_DATE_REC = 0x1e,
} msg_center_device_type_id_t;

// 获取电量
errcode_t msg_center_get_battery_level(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
// 获取存储空间
errcode_t msg_center_get_storage_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
// 获取手表类型
errcode_t msg_center_get_product_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
// 获取语言
errcode_t msg_center_get_language_id(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
// 设置语言
errcode_t msg_center_set_language_id(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
// ota初始化
errcode_t msg_center_ota_init(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
// ota检测和本地升级
errcode_t msg_center_ota_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

errcode_t msg_center_device_heartrate(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_device_stepdata(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_device_bloodoxygen(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

/* SPORT */
errcode_t msg_center_sport_action(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_sport_get_record(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

/* 离线日志开关 */
errcode_t msg_center_offline_log_switch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

/* 查找手机 */
errcode_t msg_center_device_findphone(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

/* 天气 */
errcode_t msg_center_device_sync_weather_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

/* 日历 */
errcode_t msg_center_device_sync_date_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

/* CAMERA */
errcode_t msg_center_camera_exit(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_camera_online(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#if (CONFIG_ONLINE_LOG == 1)
/* 在线日志通道 */
errcode_t msg_center_set_online_log_channel(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
/* 在线日志级数 */
errcode_t msg_center_set_online_log_level(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
#endif

#ifdef PUBLISH_TEST_DATA
void PublishTestData();
#endif
#ifdef __cplusplus
}
#endif
#endif /* MSG_CENTER_DEVICE_H */
