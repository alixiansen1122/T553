/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MsgCenter Device
 * Create: 2025-06-06
 */

#include "msg_center_device.h"
#include "msg_center_protocol.h"
#include "device_proxy.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "sport/SportView.h"
#include "sport/SportModel.h"
#include "findphone/FindPhoneMainPage.h"
#include "camera/CameraMainPage.h"
#include "weather/WeatherMainPage.h"
#include "date/DateMainPage.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "diag_msg.h"
#include "diag_ind_src.h"

uint8_t g_bloodoxygen = 0;
uint8_t g_heartrate = 0;
uint32_t g_stepdata = 0;
uint32_t g_distance = 0;
// 获取电量
errcode_t msg_center_get_battery_level(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get_battery_level");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;

    uint8_t battery_level = OHOS::battery_power::get_battery_percentage();

    errcode_t ret = msg_center_send_data(cmd_id, type, &battery_level, sizeof(battery_level));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send get_battery_level_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
// 获取存储空间
errcode_t msg_center_get_storage_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get_storage_info");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;
    uint8_t battery_level = 0;

    OHOS::storage_info info;
    OHOS::device_proxy::get_storage_info(info);

    errcode_t ret = msg_center_send_data(cmd_id, type, &info, sizeof(OHOS::storage_info));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send get_storage_info_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
// 获取手表类型
errcode_t msg_center_get_product_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get_product_info");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;
    uint8_t battery_level = 0;

    std::string product_info;
    OHOS::device_proxy::get_product_info(product_info);

    errcode_t ret = msg_center_send_data(cmd_id, type, (void *)product_info.c_str(), product_info.length());
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send get_product_info_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
// 获取语言
errcode_t msg_center_get_language_id(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get_language_id");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;
    uint8_t battery_level = 0;

    uint8_t language_id = 0;
    OHOS::device_proxy::get_language_id(language_id);

    errcode_t ret = msg_center_send_data(cmd_id, type, &language_id, sizeof(uint8_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send get_language_id_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
// 设置语言
errcode_t msg_center_set_language_id(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "set_language_id");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;

    uint8_t language_id = 0;
    if (v_len != sizeof(uint8_t)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "v_len check error, v_len=%u, need=%u", v_len, sizeof(uint8_t));
        return ERRCODE_FAIL;
    }

    language_id = *(uint8_t *)v_data;
    OHOS::device_proxy::set_language_id(language_id);

    errcode_t ret = msg_center_send_data(cmd_id, type, &language_id, sizeof(uint8_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send set_language_id_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
// ota初始化
errcode_t msg_center_ota_init(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ota_init");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;

    OHOS::ota_info info;
    if (v_len != sizeof(OHOS::ota_info)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "v_len check error, v_len=%u, need=%u", v_len, sizeof(uint8_t));
        return ERRCODE_FAIL;
    }

    memcpy_s(&info, sizeof(OHOS::ota_info), v_data, v_len);
    OHOS::OTA_ERR err = OHOS::device_proxy::ota_init(info);

    errcode_t ret = msg_center_send_data(cmd_id, type, &err, sizeof(OHOS::OTA_ERR));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send set_volume_info_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
// ota检测和本地升级
errcode_t msg_center_ota_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ota_update");
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    void *v_data = msg_center_get_tlv_payload(usr_data);
    uint8_t v_len = usr_len - tl_len;

    OHOS::ota_info info;
    if (v_len != sizeof(OHOS::ota_info)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "v_len check error, v_len=%u, need=%u", v_len, sizeof(uint8_t));
        return ERRCODE_FAIL;
    }

    memcpy_s(&info, sizeof(OHOS::ota_info), v_data, v_len);
    OHOS::OTA_ERR err = OHOS::device_proxy::ota_update(info);

    errcode_t ret = msg_center_send_data(cmd_id, type, &err, sizeof(OHOS::OTA_ERR));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send set_volume_info_ack fail, ret = %d", ret);
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send ret = %d", ret);
    OHOS::device_proxy::call_ui_ota_update();

    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "send ui over");
    return ERRCODE_SUCC;
}

errcode_t msg_center_device_heartrate(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;

    ret = msg_center_send_data(cmd_id, type, &g_heartrate, sizeof(g_heartrate));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_device_heartrate:%d\r\n", g_heartrate);
    /* notify UI or call UI load */

    return ERRCODE_SUCC;
}

errcode_t msg_center_device_stepdata(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;

    ret = msg_center_send_data(cmd_id, type, &g_stepdata, sizeof(g_stepdata));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_device_stepdata:%d\r\n", g_stepdata);
    /* notify UI or call UI load */

    return ERRCODE_SUCC;
}

errcode_t msg_center_device_bloodoxygen(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;

    ret = msg_center_send_data(cmd_id, type, &g_bloodoxygen, sizeof(g_bloodoxygen));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_device_bloodoxygen:%d\r\n", g_bloodoxygen);
    /* notify UI or call UI load */

    return ERRCODE_SUCC;
}
/* SPORT */
errcode_t msg_center_sport_action(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t ack = 1;
    OHOS::sport_control_t *sport = (OHOS::sport_control_t *)tlv_payload;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_sport_action=%d, type=%d", sport->action, sport->type);

    switch (sport->action) {
        case OHOS::SPORT_ACTION_START:
            OHOS::SportModel::GetInstance().SetSportType(static_cast<OHOS::SportType>(sport->type));
            OHOS::SportModel::GetInstance().StartSport(false);
            OHOS::NativeAbility::GetInstance().SwitchSlice(VIEW_SPORT, OHOS::SportPages::SPORT_PAGE_DATA);
            break;
        case OHOS::SPORT_ACTION_PAUSE:
            OHOS::SportModel::GetInstance().PauseAndReport(false);
            OHOS::NativeAbility::GetInstance().SwitchSlice(VIEW_SPORT, OHOS::SportPages::SPORT_PAGE_PAUSE);
            break;
        case OHOS::SPORT_ACTION_RESUME:
            OHOS::SportModel::GetInstance().ResumeAndReport(false);
            OHOS::NativeAbility::GetInstance().SwitchSlice(VIEW_SPORT, OHOS::SportPages::SPORT_PAGE_DATA);
            break;
        case OHOS::SPORT_ACTION_STOP:
            OHOS::SportModel::GetInstance().StopAndReport(false);
            OHOS::NativeAbility::GetInstance().SwitchSlice(VIEW_SPORT, OHOS::SportPages::SPORT_PAGE_MENU);
            break;
        default:
            ret = ERRCODE_FAIL;
            break;
    }

    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_sport_action send data failed\r\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_sport_get_record(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;

    auto record = OHOS::SportModel::GetInstance().GetRecord();
    uint16_t num = record.size();
    uint16_t size = sizeof(uint16_t) + num * sizeof(OHOS::sport_data_t);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_sport_get_record record num=%d, malloc szie=%d\r\n", num,
                  size);
    uint8_t *data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_sport_get_record malloc data failed\r\n");
        return ERRCODE_FAIL;
    }
    uint8_t *data_addr = data;

    memcpy_s(data, sizeof(uint16_t), &num, sizeof(uint16_t));
    data += sizeof(uint16_t);

    for (int i = 0; i < num; i++) {
        memcpy_s(data, sizeof(OHOS::sport_data_t), &record[i], sizeof(OHOS::sport_data_t));
        data += sizeof(OHOS::sport_data_t);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "========record[%d]=============", i);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "type:%d", record[i].type);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "heartRate:%d", record[i].heartRate);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "calorie:%d", record[i].calorie);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "altitude:%d", record[i].altitude);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "ascent:%d", record[i].ascent);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "descent:%d", record[i].descent);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "distance:%d", record[i].distance);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "lap:%d", record[i].lap);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "startTime:%d", record[i].startTime);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "endTime:%d", record[i].endTime);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "elapsedTime:%d", record[i].elapsedTime);
    }

    ret = msg_center_send_data(cmd_id, type, data_addr, size);
    free(data_addr);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_sport_get_record send data failed\r\n");
        return ERRCODE_FAIL;
    }
    OHOS::SportModel::GetInstance().ClearRecord();
    return ERRCODE_SUCC;
}

errcode_t msg_center_camera_exit(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "recv exit camera from phone");
    OHOS::NativeAbility::GetInstance().ChangeSliceToApplist();
    return ERRCODE_SUCC;
}

errcode_t msg_center_camera_online(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "recv connect success from phone");
    if (OHOS::CameraMainPage::GetInstance() != nullptr) {
        OHOS::CameraMainPage::GetInstance()->Refresh_page();
        return ERRCODE_SUCC;
    } else {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "CameraMainPage not exist\r\n");
        return ERRCODE_FAIL;
    }
}

errcode_t msg_center_offline_log_switch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t data = *tlv_payload;
    errcode_t ret = ERRCODE_SUCC;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_offline_log_switch, data = %d\r\n", data);
    if (data) {
        uapi_zdiag_set_offline_log_enable(true); /*open offline log*/
    } else {
        uapi_zdiag_set_offline_log_enable(false); /*close offline log*/
    }
    ret = msg_center_send_data(cmd_id, type, &data, sizeof(data));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

#if (CONFIG_ONLINE_LOG == 1)
errcode_t msg_center_set_online_log_channel(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t data = *tlv_payload;
    errcode_t ret = ERRCODE_SUCC;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_set_online_log_channel, data = %d\r\n", data);
    change_log_diag_channel(data);
    ret = msg_center_send_data(cmd_id, type, &data, sizeof(data));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_set_online_log_level(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t data = *tlv_payload;
    errcode_t ret = ERRCODE_SUCC;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_set_online_log_level, data = %d\r\n", data);
    set_online_log_level(data);
    ret = msg_center_send_data(cmd_id, type, &data, sizeof(data));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

errcode_t msg_center_device_findphone(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    bool ret = true;
    if (OHOS::FindPhoneMainPage::GetInstance() != nullptr) {
        ret = OHOS::FindPhoneMainPage::GetInstance()->StopFindPhone();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "FindphonePage not exist\r\n");
    }
    if (!ret) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_device_sync_weather_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    if (OHOS::WeatherMainPage::GetInstance() != nullptr) {
        OHOS::WeatherMainPage::GetInstance()->ReloadPage();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "WeatherMainPage not exist\r\n");
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_device_sync_date_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    if (OHOS::DateMainPage::GetInstance() != nullptr) {
        OHOS::DateMainPage::GetInstance()->ReloadPage();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "DateMainPage not exist\r\n");
    }
    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif