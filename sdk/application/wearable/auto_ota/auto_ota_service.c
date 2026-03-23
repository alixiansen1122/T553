/**
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Power display service.
 * Author:
 * Create:
 */

#include <soc_osal.h>
#include <osal_list.h>
#include <securec.h>
#include "cmsis_os2.h"
#include "systick.h"
#include "soc_errno.h"
#include "common_def.h"
#include "bts_def.h"
#include "bts_br_gap.h"
#include "oal_interface.h"
#include "wearable_log.h"
#include "local_device_manager.h"
#include "remote_device_manager.h"
#include "spp_service.h"
#include "bts_spp.h"
#include "btsrv_log.h"
#include "bts_feature.h"
#include "auto_ota_service.h"
#include "msg_center_ota.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUTO_OTA_RETRY_TIMES 5
#define AUTO_OTA_SEM_TIMEOUT (60*60*1000) /* 等待信号量超时时间，考虑资源大包传输时间长场景 */
#define AUTO_OTA_DELAY_MS 100
#define AUTO_OTA_SPP_TASK_STACK_SIZE 2048
#define AUTO_OTA_SPP_TASK_PRI 20
#define ADDR_MAC_LEN 6
#define AUTO_OTA_RECORD_DEV_MAX_NUM 200 /* 设计成客户可定制 */
#define AUTO_OTA_SCAN_WAIT_TIME  (25*1000) /* 客户可定制 */
#define AUTO_OTA_UPDATE_RSSI_WAIT_TIME  (20*1000) /* 客户可定制 */
#define AUTO_OTA_INQUERY_DEV_NUM   20 /* 客户可定制 */
#define AUTO_OTA_OPRATE_DELAY_MS (2*1000) /* 客户可定制 */
#define AUTO_OTA_ACL_CONN_DELAY (6*1000) /* 客户可定制 */
#define AUTO_OTA_SPP_RECONN_TIMEOUT 10000 /* 客户可定制 */
#define AUTO_OTA_WAIT_PREPARE_TIMEOUT (2 * 1000) /* 客户可定制, 用于更换升级包场景 */
#define AUTO_OTA_DEV_NAME_PREFIX "Watch" /* 客户基于自己产品定义修改bt name前缀，用于黑名单管理 */
#define AUTO_OTA_DEV_NAME_PREFIX_LEN 5
#define AUTO_OTA_DST_UPG_VER_PATH "/user/dst_ver.txt"

bool g_auto_ota_found_list_init = false;
struct osal_list_head g_auto_ota_found_dev_list_head;
osSemaphoreId_t g_auto_ota_transmit_done_sem = NULL;
uint8_t g_auto_ota_upload_mode = 0; /* 默认是0，不阻塞auto_ota */
uint8_t g_auto_ota_mac_tmp[ADDR_MAC_LEN] = {0};
static bool g_anti_back_link_flag = false;

typedef struct auto_ota_record_dev {
    uint8_t addr[ADDR_MAC_LEN];   /* mac地址 */
    uint8_t has_been_updated; /* 标记是否已经升级 */
    uint8_t reserved;         /* 对齐 */
} auto_ota_record_dev_t;

static auto_ota_record_dev_t g_auto_ota_record_dev[AUTO_OTA_RECORD_DEV_MAX_NUM];

typedef struct {
    struct osal_list_head node;
    bd_addr_t bd_addr;
    uint8_t name[BD_NAME_LEN];
    int32_t cod;
    int32_t rssi;
} auto_ota_found_dev_list_node;

typedef struct {
    uint32_t sample_mode;
    spp_create_socket_para_t socket_params;
    bd_addr_t addr;
} auto_ota_spp_connect_task_params;

/* 数据管理 */
void auto_ota_dev_init(void)
{
    for (uint8_t i = 0; i < AUTO_OTA_RECORD_DEV_MAX_NUM; i++) {
        (void)memset_s(&g_auto_ota_record_dev[i], sizeof(auto_ota_record_dev_t), 0, sizeof(auto_ota_record_dev_t));
    }
    return;
}

void auto_ota_dev_add(uint8_t *addr)
{
    for (uint16_t i = 0; i < AUTO_OTA_RECORD_DEV_MAX_NUM; i++) {
        if (g_auto_ota_record_dev[i].has_been_updated == 0) {
            (void)memcpy_s(g_auto_ota_record_dev[i].addr, ADDR_MAC_LEN, addr, ADDR_MAC_LEN);
            g_auto_ota_record_dev[i].has_been_updated = 1;
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "to add record dev_mac : ****%02x%02x%02x%02x\r\n",
                g_auto_ota_record_dev[i].addr[3], g_auto_ota_record_dev[i].addr[2], g_auto_ota_record_dev[i].addr[1],
                g_auto_ota_record_dev[i].addr[0]);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "alloc index = %d\r\n", i);
            return;
        }
    }
}

bool auto_ota_dev_del(uint8_t *addr)
{
    for (uint16_t i = 0; i < AUTO_OTA_RECORD_DEV_MAX_NUM; i++) {
        if (g_auto_ota_record_dev[i].has_been_updated == 1 &&
            memcmp(g_auto_ota_record_dev[i].addr, addr, ADDR_MAC_LEN) == 0) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "found! to del record dev_mac : ****%02x%02x%02x%02x\r\n",
                g_auto_ota_record_dev[i].addr[3], g_auto_ota_record_dev[i].addr[2], g_auto_ota_record_dev[i].addr[1],
                g_auto_ota_record_dev[i].addr[0]);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "free index = %d\r\n", i);
            (void)memset_s(g_auto_ota_record_dev[i].addr, ADDR_MAC_LEN, 0, ADDR_MAC_LEN);
            g_auto_ota_record_dev[i].has_been_updated = 0;
            return true;
        }
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "dev_del: no found dev_mac.\r\n");
    return false;
}

bool auto_ota_dev_found(uint8_t *addr)
{
    for (uint16_t i = 0; i < AUTO_OTA_RECORD_DEV_MAX_NUM; i++) {
        if (g_auto_ota_record_dev[i].has_been_updated == 1 &&
            memcmp(g_auto_ota_record_dev[i].addr, addr, ADDR_MAC_LEN) == 0) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "no need to record dev_mac: ****%02x%02x%02x%02x\r\n",
                g_auto_ota_record_dev[i].addr[3], g_auto_ota_record_dev[i].addr[2], g_auto_ota_record_dev[i].addr[1],
                g_auto_ota_record_dev[i].addr[0]);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "taken index = %d\r\n", i);
            return true;
        }
    }
    return false;
}

void auto_ota_dev_show(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "auto_ota total dev info :");
    for (uint16_t i = 0; i < AUTO_OTA_RECORD_DEV_MAX_NUM; i++) {
        if (g_auto_ota_record_dev[i].has_been_updated == 1) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "already record dev_mac : ****%02x%02x%02x%02x\r\n",
                g_auto_ota_record_dev[i].addr[3], g_auto_ota_record_dev[i].addr[2], g_auto_ota_record_dev[i].addr[1],
                g_auto_ota_record_dev[i].addr[0]);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "taken index = %d\r\n", i);
        }
    }
}

/* 黑名单处理 */
static bool auto_ota_dev_check_dev_name(unsigned char *dev_name)
{
    if (strncmp((const char *)dev_name, AUTO_OTA_DEV_NAME_PREFIX, AUTO_OTA_DEV_NAME_PREFIX_LEN) == 0) {
        return true;
    } else {
        return false;
    }
}

/* 重置发现设备链表 */
void auto_ota_reset_found_dev_list(void)
{
    if (!g_auto_ota_found_list_init) {
        g_auto_ota_found_list_init = true;
        OSAL_INIT_LIST_HEAD(&g_auto_ota_found_dev_list_head);
        return;
    }

    while (osal_list_empty(&g_auto_ota_found_dev_list_head) == false) {
        struct osal_list_head *node = g_auto_ota_found_dev_list_head.next;
        osal_list_del(node);
        oal_mem_free(node);
    }
}

bool auto_ota_bt_start_discovery(void)
{
    auto_ota_reset_found_dev_list();
    (void)gap_br_set_inquiry_paramters(0, AUTO_OTA_INQUERY_DEV_NUM);
    for (uint8_t i = 0; i < AUTO_OTA_RETRY_TIMES; i++) {
        bool res = gap_br_start_discovery();
        if (res != true) {
            osDelay(AUTO_OTA_DELAY_MS);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "gap_br_start_discovery fail.\r\n");
            continue;
        } else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "gap_br_start_discovery succ.\r\n");
            return true;
        }
    }
    return false;
}

/* 新增节点 */
static void auto_ota_add_node_to_found_dev_list(const bd_addr_t *bd_addr, const uint8_t *name, int32_t cod, int32_t rssi)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "auto_ota_add_node, addr: ****%02x%02x%02x%02x, name: %s",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], name); /* addr下标 0 1 2 3 */

    auto_ota_found_dev_list_node *node =
        (auto_ota_found_dev_list_node *)(void *)oal_mem_alloc(sizeof(auto_ota_found_dev_list_node));
    if (node == NULL) {
        return;
    }
    (void)memset_s(node, sizeof(auto_ota_found_dev_list_node), 0, sizeof(auto_ota_found_dev_list_node));

    if (memcpy_s(&node->bd_addr, sizeof(node->bd_addr), bd_addr, sizeof(bd_addr_t)) != EOK) {
        oal_mem_free(node);
        return;
    }

    if (memcpy_s(node->name, sizeof(node->name), name, BD_NAME_LEN) != EOK) {
        oal_mem_free(node);
        return;
    }

    node->cod = cod;
    node->rssi = rssi;
    osal_list_add_tail((struct osal_list_head *)node, &g_auto_ota_found_dev_list_head);
}

static void auto_ota_sort_found_dev_list(void)
{
    struct osal_list_head *list_head = &g_auto_ota_found_dev_list_head;

    for (struct osal_list_head *node1 = list_head->next; node1 != NULL && node1 != list_head; node1 = node1->next) {
        auto_ota_found_dev_list_node *dev_info1 = (auto_ota_found_dev_list_node *)node1;
        for (struct osal_list_head *node2 = node1->next; node2 != NULL && node2 != list_head; node2 = node2->next) {
            auto_ota_found_dev_list_node *dev_info2 = (auto_ota_found_dev_list_node *)node2;

            /* rssi大的在前 */
            if (dev_info1->rssi >= dev_info2->rssi) {
                continue;
            }

            /* 交换两个节点的信息 */
            auto_ota_found_dev_list_node tmp_node;
            uint32_t ret =
                (uint32_t)memcpy_s(&tmp_node, sizeof(tmp_node), dev_info1, sizeof(auto_ota_found_dev_list_node));
            ret |= (uint32_t)memcpy_s(&dev_info1->bd_addr,
                sizeof(auto_ota_found_dev_list_node) - sizeof(struct osal_list_head),
                &dev_info2->bd_addr,
                sizeof(auto_ota_found_dev_list_node) - sizeof(struct osal_list_head));
            ret |= (uint32_t)memcpy_s(&dev_info2->bd_addr,
                sizeof(auto_ota_found_dev_list_node) - sizeof(struct osal_list_head),
                &tmp_node.bd_addr,
                sizeof(auto_ota_found_dev_list_node) - sizeof(struct osal_list_head));
            if (ret != EOK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "swap fail %x\n", ret);
            }
        }
    }
}

/* 异步返回，更新RSSI */
static void auto_ota_update_found_dev_node_rssi(const bd_addr_t *bd_addr, int32_t rssi)
{
    struct osal_list_head *list_head = &g_auto_ota_found_dev_list_head;
    struct osal_list_head *node = list_head->next;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER,"will update rssi, addr: ****%02x%02x%02x%02x, rssi = %d\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], rssi); /* addr下标 0 1 2 3 */

    while (node != NULL && node != list_head) {
        auto_ota_found_dev_list_node *dev_info = (auto_ota_found_dev_list_node *)node;
        if (memcmp(dev_info->bd_addr.addr, bd_addr->addr, BD_ADDR_LEN) == 0) {
            dev_info->rssi = rssi;
            return;
        }

        node = node->next;
    }
}

static void auto_ota_gap_read_remote_rssi_event_callback(const bd_addr_t *bd_addr, int rssi, int status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER,
        " addr: ****%02x%02x%02x%02x rssi: %d status: %d\n",
        bd_addr->addr[3], bd_addr->addr[2], /* addr下标 2 3 */
        bd_addr->addr[1], bd_addr->addr[0], rssi, status); /* addr下标 0 1 */

    if (status == ERRCODE_BT_SUCCESS) {
        auto_ota_update_found_dev_node_rssi(bd_addr, rssi);
    }
}

static bool auto_ota_get_top_dev_to_connect(void)
{
    struct osal_list_head *list_head = &g_auto_ota_found_dev_list_head;
    uint8_t blank_addr[ADDR_MAC_LEN] = {0};

    for (struct osal_list_head *node = list_head->next; node != NULL && node != list_head; node = node->next) {
        auto_ota_found_dev_list_node *dev_info = (auto_ota_found_dev_list_node *)node;
        if (memcmp(dev_info->bd_addr.addr, blank_addr, ADDR_MAC_LEN) == 0) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "ignore zero addr, name: %s\n", dev_info->name);
            continue;
        }
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER,
                "plan to connect addr: ****%02x%02x%02x%02x name: \"%s\" cod: %x rssi: %d\n",
                dev_info->bd_addr.addr[3], dev_info->bd_addr.addr[2], /* addr下标 3 2 */
                dev_info->bd_addr.addr[1], dev_info->bd_addr.addr[0], /* addr下标 1 0 */
                dev_info->name,
                dev_info->cod,
                dev_info->rssi);

        for (uint8_t i = 0; i < AUTO_OTA_RETRY_TIMES; i++) {
            if (gap_connect_remote_device(&(dev_info->bd_addr)) != true) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "to connect acl fail\r\n");
                osDelay(100);
            } else {
                osDelay(AUTO_OTA_ACL_CONN_DELAY);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "to connect acl succ\r\n");
                return true;
            }
        }
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "no found_device\n");
    return false;
}

static void auto_ota_discovery_result_callback(const bd_addr_t *bd_addr)
{
    unsigned char length = BD_NAME_LEN;
    unsigned char *device_name = (unsigned char *)(void *)oal_mem_alloc(length);
    if (device_name == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "gap_discovery_result_callback malloc error!\n");
        return;
    }
    (void)memset_s(device_name, length, 0, length);

    bool ret1 = gap_get_device_name(bd_addr, device_name, &length);
    int cod = gap_get_device_class(bd_addr);
    bool ret2 = gap_read_remote_rssi_value(bd_addr);

    if (auto_ota_dev_found(bd_addr->addr) != true && auto_ota_dev_check_dev_name(device_name) == true) {
        auto_ota_add_node_to_found_dev_list(bd_addr, device_name, cod, 0);
    }

    return;
}

static bool auto_ota_upg_check(void)
{
    errcode_t ret = ERRCODE_SUCC;

    /* 目标版本从文件读取，方便客户每次推送不同版本 */
    FILE *fp = NULL;

    fp = fopen(AUTO_OTA_DST_UPG_VER_PATH, "rb");
    if (fp == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "open %s fail.\r\n", AUTO_OTA_DST_UPG_VER_PATH);
        return false;
    }

    fseek(fp, 0L, SEEK_END);
    long file_len = ftell(fp);
    char *dst_ver = (char *)malloc(sizeof(char) * (file_len + 1));
    if (dst_ver == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "malloc fail, file_len = %d.\r\n", file_len);
        fclose(fp);
        return false;
    }

    fseek(fp, 0L, SEEK_SET);
    if (fread(dst_ver, file_len, 1, fp) != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "fread fail, file_len = %d.\r\n", file_len);
        fclose(fp);
        free(dst_ver);
        dst_ver = NULL;
        return false;
    }
    fclose(fp);
    dst_ver[file_len] = '\0';

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "dst_ver = %s, file_len = %d.\r\n", dst_ver, file_len);

    ret = msg_center_send_data(MSGCENTER_CMD_OTA, MSGCENTER_TYPE_ID_OTA_UPG_CHECK, dst_ver, strlen(dst_ver));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send upgrade version check fail, ret = %d\r\n", ret);
        free(dst_ver);
        dst_ver = NULL;
        return false;
    }

    free(dst_ver);
    dst_ver = NULL;
    return true;
}

static int auto_ota_spp_client_connect_task_body(void *data)
{
    auto_ota_spp_connect_task_params *task_params = (auto_ota_spp_connect_task_params *)data;
    spp_create_socket_para_t *socket_param = &task_params->socket_params;

    int ret = spp_connect(socket_param, &task_params->addr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "spp_connect ret: %x\n", ret);

    oal_mem_free(data);
    return OSAL_SUCCESS;
}

/* client连接server */
bool auto_ota_spp_connect(const char *spp_uuid, const bd_addr_t *addr)
{
    uint8_t uuid_len = (uint8_t)strlen(spp_uuid);
    auto_ota_spp_connect_task_params *task_params =
        (auto_ota_spp_connect_task_params *)oal_mem_alloc(sizeof(auto_ota_spp_connect_task_params));

    if (generate_uuid_from_string(spp_uuid, uuid_len + 1, &task_params->socket_params.uuid) != EXT_ERR_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "generate uuid fail\n");
        return false;
    }

    task_params->sample_mode = 0;
    task_params->socket_params.socket_type = SPP_SOCKET_RFCOMM;
    task_params->socket_params.is_encrypt = true;
    (void)memcpy_s(&(task_params->addr.addr), BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);

    osal_task *task = osal_kthread_create(auto_ota_spp_client_connect_task_body, task_params,
        "auto_ota_spp_client_connect_task", AUTO_OTA_SPP_TASK_STACK_SIZE);
    if (task == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "create task fail\n");
        return false;
    }
    int ret = osal_kthread_set_priority(task, AUTO_OTA_SPP_TASK_PRI);
    if (ret != OSAL_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "set pri error: %08x\n", ret);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "create task success.\n");

    bool is_connected = false;
    uint64_t start_time = uapi_tcxo_get_ms();
    uint64_t current_time;
    while (is_connected == false) {
        is_connected = is_spp_connected(0);
        osDelay(AUTO_OTA_OPRATE_DELAY_MS);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "spp reconnecting.\r\n");
        current_time = uapi_tcxo_get_ms();
        if (current_time - start_time > AUTO_OTA_SPP_RECONN_TIMEOUT || is_connected == true) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "spp connect %s.\r\n", is_connected == true ? "Succ" : "Fail");
            break;
        }
    }

    /* 升级版本号校验 */
    bool res = auto_ota_upg_check();
    if (res != true) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "auto_ota_upg_check fail.\n");
        return false;
    }

    return true;
}

static void auto_ota_acl_state_changed_callback(const bd_addr_t *bd_addr, gap_acl_state_t state, unsigned int reason)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "acl cb, state %d, reason %x", state, reason);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "addr: ****%02x%02x%02x%02x",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* mac下标 0 1 2 3 */
    char uuid[] = {"1101"};
    if (state == GAP_ACL_STATE_CONNECTED) {
        if (g_anti_back_link_flag == true) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "anti back link addr: ****%02x%02x%02x%02x",
                bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* mac下标 0 1 2 3 */
            return;
        }
        bool res = auto_ota_spp_connect(uuid, bd_addr);
        if (res != true) {
            (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
            return;
        }
        /* spp trans回调中释放信号量最准确，故此处临时记录mac地址 */
        (void)memcpy_s(g_auto_ota_mac_tmp, ADDR_MAC_LEN, (const char *)bd_addr->addr, ADDR_MAC_LEN);
        g_anti_back_link_flag = true;
    }
}

void auto_ota_gap_register_callbacks(void)
{
    gap_call_backs_t cb = {
        NULL,
        auto_ota_acl_state_changed_callback,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        auto_ota_discovery_result_callback,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        auto_ota_gap_read_remote_rssi_event_callback,
        NULL
    };
    (void)gap_register_callbacks(&cb);
}

errcode_t auto_ota_enable_bt(void)
{
#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
    g_bth_log_type_level = 0xFF2; /* BTSRV_LOG_UART_INFO */
#endif
    bts_feature_t feature = {0};
    feature.bt_auto_reconnect_closed = 1;

    auto_ota_gap_register_callbacks();
    bts_set_features(&feature); /* 关闭client回链 */
    errcode_t res = enable_bt_stack();
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "bt enable fail, ret = %d\r\n", res);
        return ERRCODE_FAIL;
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "bt enable ok\r\n");
        return ERRCODE_SUCC;
    }
}

void auto_ota_task_init(void)
{
    g_auto_ota_transmit_done_sem = osSemaphoreNew(1, 0, NULL);
    if (!g_auto_ota_transmit_done_sem) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "sem creat fail!\r\n");
        return;
    }
}

void auto_ota_task_main(void)
{
    errcode_t ret;

    auto_ota_dev_init();
    auto_ota_task_init();

    if (auto_ota_enable_bt() != ERRCODE_SUCC) {
        return;
    }

    while (1) {
        if (g_auto_ota_upload_mode == 1) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "enter upload fwpkg mode, auto_ota not run!\r\n");
            osDelay(AUTO_OTA_WAIT_PREPARE_TIMEOUT);
            continue;
        }

        /* 保证最开始不会有配对信息回连，可能导致B核wait等回连 */
        if (gap_remove_all_pairs() != true) {
            osDelay(AUTO_OTA_DELAY_MS);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "remove all pairs fail, auto retry...\r\n");
            continue;
        }
        osDelay(AUTO_OTA_OPRATE_DELAY_MS);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "gap_remove_all_pairs ok\r\n");

        /* scan */
        if (auto_ota_bt_start_discovery() != true) {
            osDelay(AUTO_OTA_DELAY_MS);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "start discovery fail, auto retry...\r\n");
            continue;
        }

        osDelay(AUTO_OTA_SCAN_WAIT_TIME); /* 保证能扫到设备 */

        (void)gap_br_cancel_discovery();
        osDelay(AUTO_OTA_UPDATE_RSSI_WAIT_TIME);
        auto_ota_sort_found_dev_list(); /* 根据rssi降序排序 */
        /* connect */
        if (auto_ota_get_top_dev_to_connect() != true) {
            osDelay(AUTO_OTA_DELAY_MS);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "connect fail, auto retry...\r\n");
            continue;
        }

        osSemaphoreAcquire(g_auto_ota_transmit_done_sem, AUTO_OTA_SEM_TIMEOUT);

        /* clean, spp资源内部释放 */
        spp_disconnect(0);
        /* 删除配对时会disconnect ACL */
        if (gap_remove_all_pairs() != true) {
            osDelay(AUTO_OTA_DELAY_MS);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "remove all pairs fail, auto retry...\r\n");
            continue;
        }
        osDelay(AUTO_OTA_OPRATE_DELAY_MS);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "gap_remove_all_pairs ok\r\n");

        osDelay(AUTO_OTA_DELAY_MS);
        auto_ota_dev_show();
        memset_s(g_auto_ota_mac_tmp, sizeof(g_auto_ota_mac_tmp), 0, sizeof(g_auto_ota_mac_tmp));
        g_anti_back_link_flag = false;
        osDelay(AUTO_OTA_DELAY_MS);
    }

    return;
}

#ifdef __cplusplus
}
#endif