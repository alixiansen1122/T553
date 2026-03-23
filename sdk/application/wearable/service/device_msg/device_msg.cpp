/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Device Msg
 * Create: 2025-06-06
 */

#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
#include "msg_center_device.h"
#ifdef PUBLISH_TEST_DATA
#include <cstdlib>
#include <ctime>
#include "graphic_timer.h"
#include "broadcast_feature.h"
#include "ohos_timer.h"
#endif
namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif
#ifdef PUBLISH_TEST_DATA
#define BLOODOXYGEN_LOWER 90
#define BLOODOXYGEN_RANGE 10
#define STEP_RANGE 5
#define DISTANCE_RANGE 10
#define HEARTRATE_LOWER 60
#define HEARTRATE_RANGE 60
static uint8_t g_rotation_publish = 0;
static GraphicTimer *g_publishTestDataHandle = nullptr;
enum publishDataType { PUBLISH_SETP = 0, PUBLISH_DISTANCE, PUBLISH_HEARTRATE, PUBLISH_BLOODOXYGEN, PUBLISH_TYPE_MAX };
static void PublishTopicTestDataEvent(uint16_t topic, const void *data, size_t len)
{
    int32_t ret;
    uint16_t size = len;
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = nullptr;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PublishTopicTestDataEvent topic=%u!!", topic);

    void *publishData = malloc(size);
    if (publishData == nullptr) {
        return;
    }
    ret = memcpy_s(publishData, size, data, size);
    if (ret != EOK) {
        free(publishData);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PublishTopicTestDataEvent memcpy_s fail! ret = %d", ret);
        return;
    }

    Request request = {
        .msgId = topic,
        .len = size,
        .msgValue = 0,
        .data = publishData,
    };

    /* public request */
    api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetFeatureApi failed");
        free(publishData);
        return;
    }
    ret = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (ret != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "QueryInterface failed");
        free(publishData);
        return;
    }
    broadcastApi->PublishTopic((IUnknown *)broadcastApi, &request);
    broadcastApi->Release((IUnknown *)broadcastApi);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PublishTopicTestDataEvent end!!");

    return;
}

static void PublishTestDataCallback(void *)
{
    switch (g_rotation_publish++ % PUBLISH_TYPE_MAX) {
        case PUBLISH_BLOODOXYGEN: {
            uint8_t blood_oxygen = BLOODOXYGEN_LOWER + rand() % BLOODOXYGEN_RANGE;
            PublishTopicTestDataEvent(TOPIC_EVENT_BLOODOXYGEN_UPDATA, &blood_oxygen, sizeof(uint8_t));
            break;
        }
        case PUBLISH_SETP: {
            uint32_t step = rand() % STEP_RANGE;
            PublishTopicTestDataEvent(TOPIC_EVENT_STEPDATA_UPDATE, &step, sizeof(uint32_t));
            break;
        }
        case PUBLISH_HEARTRATE: {
            HeartDatas heart_data = {0};
            heart_data.perSec = HEARTRATE_LOWER + rand() % HEARTRATE_RANGE;
            PublishTopicTestDataEvent(TOPIC_EVENT_HEARTRATE_UPDATA, &heart_data, sizeof(HeartDatas));
            break;
        }
        case PUBLISH_DISTANCE: {
            uint32_t distance = rand() % DISTANCE_RANGE;
            PublishTopicTestDataEvent(TOPIC_EVENT_SPORT_DISTANCE, &distance, sizeof(uint32_t));
            break;
        }
        default:
            break;
    }
}

static void PublishTestData()
{
    uint32 tick = GetOSTick(5000);  // 5秒发布一次测试数据
    if (g_publishTestDataHandle == nullptr) {
        g_publishTestDataHandle = new GraphicTimer(tick, PublishTestDataCallback, nullptr, true);
    }
    if (g_publishTestDataHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PublishTestData new GraphicTimer failed!!");
        return;
    }
    bool retTimer = g_publishTestDataHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PublishTestData Start failed!!");
        if (g_publishTestDataHandle != nullptr) {
            delete g_publishTestDataHandle;
            g_publishTestDataHandle = nullptr;
        }
        return;
    }
}
#endif
static const msg_center_cmd_map_t g_msg_center_device_tbl[] = {
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_GET_BATTERY_LEVEL, msg_center_get_battery_level},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_GET_STORAGE_INFO, msg_center_get_storage_info},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_GET_PRODUCT_INFO, msg_center_get_product_info},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_GET_LANGUAGE_ID, msg_center_get_language_id},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SET_LANGUAGE_ID, msg_center_set_language_id},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_DEVICE_HEARTRATE, msg_center_device_heartrate},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_DEVICE_STEPDATA, msg_center_device_stepdata},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_DEVICE_BLOODOXYGEN, msg_center_device_bloodoxygen},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_OFFLINE_LOG_SWITCH, msg_center_offline_log_switch},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SPORT_ACTION, msg_center_sport_action},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SPORT_GET_RECORD, msg_center_sport_get_record},
#if (CONFIG_ONLINE_LOG == 1)
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SET_ONLINELOG_CHANNEL, msg_center_set_online_log_channel},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SET_ONLINELOG_LEVEL, msg_center_set_online_log_level},
#endif
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_OTA_INIT, msg_center_ota_init},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_OTA_UPDATE, msg_center_ota_update},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_STOP_FIND_PHONE, msg_center_device_findphone},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_WATCH_EXIT_CAMERA, msg_center_camera_exit},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_CAMERA_CONNECTED_PHONE, msg_center_camera_online},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SYNC_WEATHER_REC, msg_center_device_sync_weather_rec},
    {MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SYNC_DATE_REC, msg_center_device_sync_date_rec},
};

errcode_t msg_center_device_type_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_device_tbl) / sizeof(g_msg_center_device_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_device_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void device_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_DEVICE, msg_center_device_type_dispatch);
#ifdef PUBLISH_TEST_DATA
    PublishTestData();
#endif
}

APP_FEATURE_INIT_PRI(device_init, LAYER_INIT_LEVEL_4);
#ifdef __cplusplus
}
#endif
}  // namespace OHOS