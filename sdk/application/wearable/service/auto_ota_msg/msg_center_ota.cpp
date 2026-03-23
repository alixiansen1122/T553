/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */
#include "build_version.h"
#include "securec.h"
#include "soc_osal.h"
#include "diag_service.h"
#include "upg.h"
#include "upg_common_porting.h"
#include "upg_definitions.h"
#include "diag_common.h"
#include "transmit_st.h"
#include "transmit_item.h"
#include "transmit_write_read.h"
#include "dfx_resource_id.h"
#include "dfx_file_operation.h"
#include "msg_center_ota.h"
#include "msg_center_protocol.h"
#include "wearable_log.h"
#include "upg_porting.h"
#ifdef SUPPORT_AUTO_OTA
#include "auto_ota_service.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SUPPORT_AUTO_OTA
extern osSemaphoreId_t g_auto_ota_transmit_done_sem;
extern uint8_t g_auto_ota_mac_tmp[];
#endif
#ifndef unused
#define unused(var)     (void)(var)
#endif

/* 固定升级包文件长度, "/user/update.fwpkg"*/
#define UPGRADE_FILE_LEN 19

static errcode_t msg_center_ota_upgrade_request_call_back(bool is_finished, uintptr_t usr_data)
{
    unused(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    /* 目的端升级路径，建议固定 */
    uint32_t package_len = 0;
    const char *file_name = "/user/update.fwpkg";
    FILE *fp = fopen(file_name, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "open %s fail.\r\n", file_name);
        return ERRCODE_FAIL;
    }
    fseek(fp, 0L, SEEK_END);
    package_len = ftell(fp);
    fclose(fp);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send file_len = %d\n", package_len);

    // 文件传输结果判断
    if (is_finished) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "transmit success\n");
        // 文件传输成功，向Watch发送请求升级指令
        ret = msg_center_send_data(MSGCENTER_CMD_OTA, MSGCENTER_TYPE_ID_OTA_UPG_START, &package_len, sizeof(package_len));
        if (ret != ERRCODE_SUCC) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send upgrade command fail, ret = %d\r\n", ret);
            return ERRCODE_FAIL;
        }
#ifdef SUPPORT_AUTO_OTA
        auto_ota_dev_add(g_auto_ota_mac_tmp);
        (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
#endif
        return ret;
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "transmit fail\n");
#ifdef SUPPORT_AUTO_OTA
        (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
#endif
        return ret;
    }
}

static errcode_t msg_center_src_send_file_start(transmit_type_t transmit_type, diag_addr dst, uint32_t total_size,
    const char *file_name, bool re_transmit, transmit_result_hook handler)
{
    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = dst;

    switch (transmit_type) {
        case TRANSMIT_TYPE_SAVE_FILE:
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
        case TRANSMIT_TYPE_WRITE_MEMORY:
        case TRANSMIT_TYPE_WRITE_FLASH:
            break;
        default:
            return ERRCODE_INVALID_PARAM;
    }

    transmit_item_t *item = transmit_item_init(0);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    transmit_item_init_file_name(item, file_name, strlen(file_name));
    transmit_item_init_permanent(item, false);
    transmit_item_init_local_start(item, true);
    transmit_item_init_local_src(item, true);
    transmit_item_init_remote_type(item, transmit_type);
    transmit_item_init_local_type(item, TRANSMIT_LOCAL_TYPE_READ_FILE);
    transmit_item_init_read_handler(item, file_read_data, (uintptr_t)item);
    transmit_item_init_option(item, &option);
    transmit_item_init_down_machine(item, false);
    transmit_item_init_total_size(item, total_size);
    transmit_item_init_result_handler(item, handler, (uintptr_t)item);
    transmit_item_init_re_trans(item, re_transmit);

    if (transmit_item_init_is_success(item) == false) {
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    item->file_fd = dfx_file_open_for_read(item->file_name);
    if (item->file_fd < 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "[ERR][transmit src]file : %s open failed, fd = %d\r\n",
            item->file_name, item->file_fd);
        item->file_fd = 0;
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }
#endif

    transmit_item_enable(item);
    return ERRCODE_SUCC;
}

void msg_center_ota_upgrade_start(void)
{
    const char *file_name = "/user/update.fwpkg";
    uint32_t total_size;
    errcode_t ret;
    FILE *fp = fopen(file_name, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "open %s fail.\r\n", file_name);
        return;
    }
    fseek(fp, 0L, SEEK_END);
    total_size = ftell(fp);
    fclose(fp);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "read file size = %d\r\n", total_size);

    ret = msg_center_src_send_file_start(TRANSMIT_TYPE_SAVE_FILE, DIAG_FRAME_FID_PHONE, total_size, file_name,
                                        false, msg_center_ota_upgrade_request_call_back);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "transmit_src_send_file_start fail, ret = %d\r\n", ret);
        return;
    }

    return;
}

errcode_t msg_center_ota_rec_upg_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    /* 收到升级指令 */
    upg_prepare_info_t info;
    info.package_len = *(uint32_t *)tlv_payload;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_prepare running......, rec file_len = %d\r\n", info.package_len);
    ret = uapi_upg_prepare(&info);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_prepare err, ret = 0x%x\r\n", ret);
        return ret;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_prepare success!!!!\r\n");

    ret = uapi_upg_request_upgrade(false);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_request err, ret = 0x%x\r\n", ret);
        return ret;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg request success!!!!\r\n");

    ret = uapi_upg_start();
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_request start, ret = 0x%x\r\n", ret);
        return ret;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg start success!!!!\r\n");

    upg_reboot();
    return ERRCODE_SUCC;
}

/* 客户逻辑实现 */
__attribute__((weak)) errcode_t msg_center_ota_upg_version_check(char *dst_ver, char *src_ver)
{
    unused(dst_ver);
    unused(src_ver);
    /* 客户版本号校验逻辑 */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg version check default pass!\r\n");
    /* 客户检查本地剩余空间是否足够容纳升级包大小 */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg space check default pass!\r\n");
    return ERRCODE_SUCC;
}

__attribute__((weak)) char * msg_center_ota_get_src_ver(void)
{
    char *src_ver = "XXX_YYY";
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "get src version : %s\r\n", src_ver);
    return src_ver;
}

errcode_t msg_center_ota_rec_upg_check(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    errcode_t res = ERRCODE_SUCC;
    uint8_t check_succ = 1;
    uint8_t check_fail = 0;

    char *dst_ver = (char *)malloc(payload_len + 1);
    if (dst_ver == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_dst_version: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(dst_ver, payload_len, tlv_payload, payload_len);
    dst_ver[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_dst_version: %s\r\n", dst_ver);

    ret = msg_center_ota_upg_version_check(dst_ver, msg_center_ota_get_src_ver());
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg version check fail, no need upg!\r\n");
        res = msg_center_send_data(cmd_id, MSGCENTER_TYPE_ID_OTA_UPG_CHECK_RESULT, &check_fail, sizeof(check_fail));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send upg check fail, result = %d\r\n", res);
        free(dst_ver);
        dst_ver = NULL;
        return ret;
    }

    free(dst_ver);
    dst_ver = NULL;
    res = msg_center_send_data(cmd_id, MSGCENTER_TYPE_ID_OTA_UPG_CHECK_RESULT, &check_succ, sizeof(check_succ));
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send upg check succ, result = %d\r\n", res);

    return ERRCODE_SUCC;
}

errcode_t msg_center_ota_rec_upg_check_result(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t upg_check_result = *tlv_payload;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_check_result = %d\r\n", upg_check_result);
    if (upg_check_result) {
        msg_center_ota_upgrade_start();
    } else {
#ifdef SUPPORT_AUTO_OTA
        (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
#endif
    }

    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif