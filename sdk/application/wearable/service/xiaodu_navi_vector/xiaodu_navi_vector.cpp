#include <stdio.h>
#include "msg_center.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"
#include "xiaodu_navi_vector.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif
#include "common_def.h"
#include "baidumap_msg_center.h"
#include "baidumap.h"
#include "watchdog.h"
#include "js_ability.h"

#ifdef __cplusplus
extern "C" {
#endif

static void msg_center_transfer_longbuf(char *data, uint8_t type)
{
    FILE *fp = nullptr;

    fp = fopen(data, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "open %s fail.\r\n", data);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    long file_len = ftell(fp);
    char *file_content = (char *)malloc(sizeof(char) * (file_len + 1));
    if (file_content == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, " malloc fail, file_len = %d.\r\n", file_len);
        fclose(fp);
        return;
    }

    fseek(fp, 0L, SEEK_SET);
    if (fread(file_content, file_len, 1, fp) != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "fread fail, file_len = %d.\r\n", file_len);
        free(file_content);
        file_content = nullptr;
        fclose(fp);
        return;
    }
    fclose(fp);
    file_content[file_len] = '\0';

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "file_len = %d.\r\n", file_len);

    /* send to JSI */
    BaidumapSendMsgToJS(file_content, type);
    free(file_content);
    file_content = nullptr;
    return;
}


errcode_t msg_center_vector_navi_pull_up(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    unused(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(60000); // 60000ms
#endif

#ifdef JS_ENABLE
    StartJsApp("com.baidu.BaiduMap");
#endif

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_recv_svg(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1; /* recv流程的协议ack做保活判断，1 -- JS onLine， 0 -- JS offLine */
    errcode_t ret = ERRCODE_SUCC;
    uint8_t res;

    /* ack */
    const char *bundleName = OHOS::ACELite::JSAbility::GetPackageName();
    if (bundleName != nullptr && strcmp(bundleName, "com.baidu.BaiduMap") == 0) {
        ack = 1;
    } else {
        ack = 0;
    }

    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (ack == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "JS offline\r\n");
        return ERRCODE_SUCC;
    }

    /* svg json */
    char *svg_json = (char *)malloc(payload_len + 1);
    if (svg_json == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_svg_json: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(svg_json, payload_len, tlv_payload, payload_len);
    svg_json[payload_len] = '\0';
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_svg_json file: %s\r\n", svg_json);

#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(600000); // 600000ms
#endif

    /* send svg_json to baiduSDK */
    struct Content content = {
        .x = 0,
        .y = 0,
        .z = 0,
        .data = NULL,
        .json = svg_json
    };

    DuerMapInfo(cmd_id, type, &content);

    free(svg_json);
    svg_json = NULL;

    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_req_svg(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "req svg succ\r\n");
    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_req_dst_by_text(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

/* 百度APP需要适配流式pcm，保存pcm文件，识别导航目的地，优先级低 */
errcode_t msg_center_vector_navi_req_dst_by_pcm(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_recv_dst_list(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

    char *data = (char *)malloc(payload_len + 1);
    if (data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "recv dst list: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv vector navi info--> %s\r\n", data);
    /* send to baiduSDK */
    free(data);
    data = nullptr;
    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_start_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_recv_navi_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

    /* send to BaiduSDK */
    char *data = (char *)malloc(payload_len + 1);
    if (data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "recv dst list: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv vector navi info--> %s\r\n", data);
    /* send data to js */
    free(data);
    data = nullptr;
    return ERRCODE_SUCC;
}

/* recv start navi form phone */
errcode_t msg_center_vector_navi_start_from_phone(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

    /* send to baiduSDK */

    return ERRCODE_SUCC;
}

/* send req poi to app */
errcode_t msg_center_vector_navi_req_poi(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        BaidumapFailSendMsgToJS("send data fail", type + 1); // JSI提供的fail回调只挂接了rec，因此req流程中加一
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "req poi ok.\r\n");
    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_recv_poi(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

    /* ack */
    const char *bundleName = OHOS::ACELite::JSAbility::GetPackageName();
    if (bundleName != nullptr && strcmp(bundleName, "com.baidu.BaiduMap") == 0) {
        ack = 1;
    } else {
        ack = 0;
    }

    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (ack == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "JS offline\r\n");
        return ERRCODE_SUCC;
    }

    char *data = (char *)malloc(payload_len + 1);
    if (data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "recv poi: malloc failed.\r\n");
        BaidumapFailSendMsgToJS("rec malloc fail", type);
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv poi info -- > %s, usr_len = %d\r\n", data, usr_len);

    msg_center_transfer_longbuf(data, type);
    free(data);
    data = nullptr;

    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_finish_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    /* send to baiduSDK */
    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_finish_from_phone(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

    /* send to baiduSDK */

    return ERRCODE_SUCC;
}

static errcode_t msg_center_vector_navi_req_common_proc(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        BaidumapFailSendMsgToJS("send data fail", type + 1);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static bool is_vector_navi_longbuf(uint8_t type)
{
    if (type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_POI_SUG_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REGEOCODE_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_RIDING_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_WALKING_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_TRANSIT_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_ROUTE_UPDATE ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_RE_ROUTE_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

bool is_vector_navi_long_distance(uint8_t type)
{
    if (type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_RIDING_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_WALKING_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_TRANSIT_SEARCH) {
        return true;
    } else {
        return false;
    }
}

static bool has_two_callback(uint8_t type)
{
    if (type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEOCODE_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REGEOCODE_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_POI ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_RIDING_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_WALKING_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_TRANSIT_SEARCH ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_AUTH_LICENSE ||
        type == MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_INIT) {
        return true;
    } else {
        return false;
    }
}

static errcode_t msg_center_vector_navi_rec_common_proc(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t ack = 1;

    /* ack */
    const char *bundleName = OHOS::ACELite::JSAbility::GetPackageName();
    if (bundleName != nullptr && strcmp(bundleName, "com.baidu.BaiduMap") == 0) {
        ack = 1;
    } else {
        ack = 0;
    }

    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (ack == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "JS offline\r\n");
        return ERRCODE_SUCC;
    }

    #if defined(SUPPORT_POWER_MANAGER)
        const power_display_svr_api_t *display_api = power_display_svr_get_api();
        if (display_api->get_screen_state() != SCREEN_ON) {
            display_api->turn_on_screen();
        }
        display_api->set_screen_set_keepon_timeout(600000); // 600000ms
    #endif
    uapi_watchdog_kick();

    char *data = (char *)malloc(payload_len + 1);
    if (data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "%s: malloc failed.\r\n", vector_navi_get_str(type));
        BaidumapFailSendMsgToJS("rec malloc fail", type);
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "%s -- > %s\r\n", vector_navi_get_str(type), data);

    if (is_vector_navi_longbuf(type) && payload_len != 1) {
        if (payload_len == 2) { // 2，长距离的协议优化，限制为 ROUTE_TRANS_MAX_SIZE, app先读文件过滤，减少无效带宽损耗
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "type(%d) exceeds long distance, send fail to JS.\r\n", type);
            BaidumapFailSendMsgToJS(data, type);
        } else {
            msg_center_transfer_longbuf(data, type);
        }
    } else if (has_two_callback(type) && payload_len == 1) {
        if (strcmp(data, "1") == 0) {
            BaidumapSendMsgToJS(data, type);
        } else if ((strcmp(data, "0") == 0)) {
            BaidumapFailSendMsgToJS(data, type);
        } else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "%s, app should trans 0 or 1\r\n", vector_navi_get_str(type));
        }
    } else {
        BaidumapSendMsgToJS(data, type);
    }
    free(data);
    data = nullptr;

    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_req_auth_license(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_auth_license(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_geo_location(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_geo_location(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_navi_init(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_navi_init(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_plan_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_plan_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_plan_succ(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_plan_succ(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_plan_fail(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_plan_fail(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_navi_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_navi_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_navi_destroy(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_navi_destroy(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_road_guide_text_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_road_guide_text_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_road_guide_icon_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_road_guide_icon_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_remain_time_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_remain_time_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_remain_distance_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_remain_distance_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_remain_route_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_remain_route_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_gps_status_change(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_gps_status_change(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_faraway(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_faraway(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_plan_yawing(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_plan_yawing(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_re_route_complete(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_re_route_complete(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_arrive_destination(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_arrive_destination(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vertor_navi_req_poi_sug_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vertor_navi_rec_poi_sug_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_riding_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_riding_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_walking_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_walking_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_route_transit_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_route_transit_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_geocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_geocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_regeocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_regeocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_req_destroy_page(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_first_batch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;
    uint8_t res;

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    /* BD09坐标系json */
    char *json = (char *)malloc(payload_len + 1);
    if (json == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_first_batch_json: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(json, payload_len, tlv_payload, payload_len);
    json[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_first_batch_json: %s\r\n", json);

    duer_msg_center_map_to_title(json, payload_len);
    free(json);
    json = NULL;

    return ERRCODE_SUCC;
}

errcode_t msg_center_vector_navi_req_destroy_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_req_common_proc(cmd_id, type, usr_data, usr_len);
}

errcode_t msg_center_vector_navi_rec_destroy_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    return msg_center_vector_navi_rec_common_proc(cmd_id, type, usr_data, usr_len);
}

__attribute__((weak)) uint8_t duer_msg_center_recv_map_info(uint8_t command_id, uint8_t type, struct Content *content)
{
    return 0;
}

__attribute__((weak)) uint8_t duer_msg_center_send_map_info(uint8_t duer_type, void *usr_data, uint16_t usr_len)
{
    return 0;
}

__attribute__((weak)) uint8_t duer_msg_center_map_to_title(void *data, uint16_t len)
{
    return 0;
}

#ifdef __cplusplus
}
#endif
