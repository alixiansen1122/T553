/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */
#include <stdio.h>
#include "securec.h"
#include "msg_center.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"
#include "common_def.h"
#include "dial_market.h"
#include "parameter.h"
#include "main/dial/WatchInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

errcode_t msg_center_dial_market_get_sys_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    watch_sys_info sys;
    bool op_ret = false;

    /* 组装信息 */
    strcpy_s(sys.chip_platform, MAX_CHIP_PLAT_LEN, "hisi");
    strcpy_s(sys.chip_model, MAX_CHIP_MODEL_LEN, "3321");
    char *os_version = GetOsName();
    if (os_version == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "os_version is NULL\r\n");
        return ERRCODE_FAIL;
    }
    strcpy_s(sys.os_version, MAX_OS_LEN, os_version); // "OpenHarmony-5.0.1.115"

    char *api_level = GetSdkApiLevel();
    if (api_level == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "api_level is NULL\r\n");
        free(os_version);
        return ERRCODE_FAIL;
    }
    strcpy_s(sys.api_level, MAX_API_LEVEL_LEN, api_level); // "13"

    op_ret = GetWatchSystemInfo(&sys);
    if (op_ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "GetWatchSystemInfo fail\r\n");
        free(os_version);
        free(api_level);
        return ERRCODE_FAIL;
    }

    ret = msg_center_send_data(cmd_id, type, &sys, sizeof(watch_sys_info));
    if (ret != ERRCODE_SUCC) {
        free(os_version);
        free(api_level);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    free(os_version);
    free(api_level);
    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_get_dev_cap(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    watch_dev_cap_info cap;
    bool op_ret = false;

    op_ret = GetWatchCapability(&cap);
    if (op_ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "GetWatchCapability fail\r\n");
        return ERRCODE_FAIL;
    }

    ret = msg_center_send_data(cmd_id, type, &cap, sizeof(watch_dev_cap_info));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_get_one_watch_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint32_t uuid = *(uint32_t *)tlv_payload;
    bool op_ret = false;
    watch_dial_info info;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "uuid = %d\r\n", uuid);
    op_ret = GetWatchFaceInfo(uuid, &info);
    if (op_ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "GetWatchFaceInfo fail\r\n");
        (void)memset_s(&info, sizeof(watch_dial_info), 0, sizeof(watch_dial_info));
        info.uuid = uuid;
        (void)msg_center_send_data(cmd_id, type, &info, sizeof(watch_dial_info));
        return ERRCODE_FAIL;
    }

    ret = msg_center_send_data(cmd_id, type, &info, sizeof(watch_dial_info));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_install_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t uuid = 0;
    bool op_ret = false;
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);

    char *path = (char *)malloc(payload_len + 1);
    if (path == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(path, payload_len, tlv_payload, payload_len);
    path[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "path = %s\r\n", path);

    op_ret = GetUuidByPath(path, &uuid);
    if (op_ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "install fail.\r\n");
        free(path);
        return ERRCODE_FAIL;
    }

    ret = msg_center_send_data(cmd_id, type, &uuid, sizeof(uint32_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        free(path);
        return ERRCODE_FAIL;
    }

    free(path);
    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_uninstall_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    bool op_ret = false;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint32_t uuid = *(uint32_t *)tlv_payload;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "uuid = %d\r\n", uuid);
    op_ret = UninstallWatch(uuid);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "uninstall ret : %d\r\n", op_ret);

    ret = msg_center_send_data(cmd_id, type, &op_ret, sizeof(uint8_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_set_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    bool op_ret = false;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint32_t uuid = *(uint32_t *)tlv_payload;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "uuid = %d\r\n", uuid);
    op_ret = SetCurrentWatchFace(uuid);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "set_current_watch_face ret : %d\r\n", op_ret);

    ret = msg_center_send_data(cmd_id, type, &op_ret, sizeof(uint8_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_get_all_watch_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    watch_dial_info *info = nullptr;
    uint8_t watch_num = 0;
    bool op_ret = false;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "get_all_watch_info\r\n");
    op_ret = GetAllWatchFaceInfo(&info, &watch_num);
    if (op_ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "GetAllWatchFaceInfo fail\r\n");
        watch_num = 0;
        (void)msg_center_send_data(cmd_id, type, info, sizeof(watch_dial_info) * watch_num);
        return ERRCODE_FAIL;
    }

    ret = msg_center_send_data(cmd_id, type, info, sizeof(watch_dial_info) * watch_num);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        free(info);
        return ERRCODE_FAIL;
    }

    free(info);
    return ERRCODE_SUCC;
}

errcode_t msg_center_dial_market_get_cur_watch_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    bool op_ret = false;
    watch_dial_info info;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "get_cur_watch_info\r\n");
    op_ret = GetCurrentWatchFaceInfo(&info);
    if (op_ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "GetCurrentWatchFaceInfo fail\r\n");
        (void)memset_s(&info, sizeof(watch_dial_info), 0, sizeof(watch_dial_info));
        info.uuid = 0xFFFFFFFF; // 异常时用全F的uuid表示
        (void)msg_center_send_data(cmd_id, type, &info, sizeof(watch_dial_info));
        return ERRCODE_FAIL;
    }

    ret = msg_center_send_data(cmd_id, type, &info, sizeof(watch_dial_info));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send data fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
#ifdef __cplusplus
}
#endif