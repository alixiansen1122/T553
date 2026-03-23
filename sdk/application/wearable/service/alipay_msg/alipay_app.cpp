/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature implement
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include "common_def.h"
#include "msg_center.h"
#include <stdio.h>
#include "adapter.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"
#include "alipay_app.h"
#include "vendor_net.h"
#include "cmsis_os2.h"
#include "sys/time.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HOSTENT_TIMEOUT 2000
#define TCP_CONNECT_TIMEOUT 2000
#define IPV4_LEN 4

osSemaphoreId_t g_alipay_hostent_sem = NULL;
osSemaphoreId_t g_alipay_tcp_sem = NULL;
unsigned char g_alipay_ip[IPV4_LEN] = { 0 };

uint8_t *g_tcp_data = NULL;
uint16_t g_tcp_data_len = 0;
uint16_t g_tcp_data_readed = 0;
uint32_t g_tcp_fd = 0;
uint8_t g_netstatus = 0;

void msg_center_alipay_reply_ack(uint8_t cmd_id, uint8_t type)
{
    uint8_t ack = 1; /* 与手机APK侧约定 */
    int ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "send replay ret = 0x%x\r\n", ret);
}

errcode_t msg_center_alipay_network_status(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);

    g_netstatus = *((uint8_t *)tlv_payload);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_alipay netstatus = %d\r\n", g_netstatus);
    msg_center_alipay_reply_ack(cmd_id, type);
    return 0;
}

uint8_t msg_center_alipay_get_network_status(void)
{
    return g_netstatus;
}

int msg_center_alipay_hostent_request(void *usr_data, uint16_t usr_len)
{
    int ret;
    if (g_alipay_hostent_sem == NULL) {
        g_alipay_hostent_sem = osSemaphoreNew(1, 0, NULL);
    }

    ret = msg_center_send_data(MSGCENTER_CMD_ALIPAY, MSGCENTER_TYPE_ID_ALIPAY_HOST_INFO, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return -1;
    } else {
        return 0;
    }
}

errcode_t msg_center_alipay_hostent_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);

    (void)memcpy_s(g_alipay_ip, IPV4_LEN, tlv_payload, payload_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv ip info--> %d.%d.%d.%d\r\n",
                  g_alipay_ip[0], g_alipay_ip[1], g_alipay_ip[2], g_alipay_ip[3]);
    osSemaphoreRelease(g_alipay_hostent_sem);
    return ERRCODE_SUCC;
}

int msg_center_alipay_hostent_reponse(unsigned char *ip)
{
    int ret;
    osStatus_t status = osSemaphoreAcquire(g_alipay_hostent_sem, HOSTENT_TIMEOUT);
    if (status == osOK) {
        (void)memcpy_s(ip, IPV4_LEN, g_alipay_ip, IPV4_LEN);
        ret = 0;
    } else {
        ret = -1;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "hostent wait status = 0x%x\r\n", status);
    osSemaphoreDelete(g_alipay_hostent_sem);
    g_alipay_hostent_sem = NULL;
    return ret;
}

int msg_center_alipay_connect_request(void *usr_data, uint16_t usr_len)
{
    int ret;
    if (g_alipay_tcp_sem == NULL) {
        g_alipay_tcp_sem = osSemaphoreNew(1, 0, NULL);
    }
    ret = msg_center_send_data(MSGCENTER_CMD_ALIPAY, MSGCENTER_TYPE_ID_ALIPAY_CONNECT, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return -1;
    } else {
        return 0;
    }
}

errcode_t msg_center_alipay_connect_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);

    alipay_connect_reponse rep;
    (void)memcpy_s(&rep, sizeof(alipay_connect_reponse), tlv_payload, payload_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv alipay_connect_reponse info--> 0x%x 0x%x\r\n",
                  rep.ret, rep.fd);
    g_tcp_fd = rep.fd;
    osSemaphoreRelease(g_alipay_tcp_sem);
    return ERRCODE_SUCC;
}

int msg_center_alipay_connect_reponse(void)
{
    int ret;
    osStatus_t status = osSemaphoreAcquire(g_alipay_tcp_sem, TCP_CONNECT_TIMEOUT);
    if (status == osOK) {
        ret = (g_tcp_fd > 0) ? 0 : -1;
    } else {
        ret = -1;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "connect wait status = 0x%x\r\n", status);
    return ret;
}

int msg_center_alipay_tcpdata_request(void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "tcpdata send len = 0x%x\r\n", usr_len);
    int ret;
    ret = msg_center_send_data(MSGCENTER_CMD_ALIPAY, MSGCENTER_TYPE_ID_ALIPAY_TCPDATA_RECV, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return -1;
    } else {
        return (int)usr_len;
    }
}

int msg_center_alipay_tcpdata_wait(uint32_t timeout)
{
    int ret;
    osStatus_t status = osSemaphoreAcquire(g_alipay_tcp_sem, timeout);
    if (status == osOK) {
        ret = 1;
    } else {
        ret = 0;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "tcpdata wait status = 0x%x\r\n", status);
    return ret;
}

errcode_t msg_center_alipay_tcpdata_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    if (g_tcp_data != NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER,
            "The last buff hasn't been read yet, current payload len is %u.\r\n", payload_len);
        return ERRCODE_FAIL;
    }
    g_tcp_data = (uint8_t *)malloc(payload_len);
    if (g_tcp_data != NULL) {
        g_tcp_data_len = payload_len;
        (void)memcpy_s(g_tcp_data, g_tcp_data_len, tlv_payload, payload_len);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv tcpdata len = %d\r\n",
                  g_tcp_data_len);
    osSemaphoreRelease(g_alipay_tcp_sem);
    return ERRCODE_SUCC;
}

int msg_center_alipay_tcpdata_read(void *mem, int len)
{
    int ret = 0;
    if (g_tcp_data != NULL) {
        int read_len = min(len, g_tcp_data_len);
        (void)memcpy_s(mem, len, g_tcp_data + g_tcp_data_readed, read_len);
        g_tcp_data_readed += read_len;
        g_tcp_data_len -= read_len;
        ret = read_len;
        if (g_tcp_data_len == 0) {
            free(g_tcp_data);
            g_tcp_data = NULL;
            g_tcp_data_readed = 0;
        }
    } else {
        ret = -2; // no data for read
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "read tcpdata len = (%d, %d)\r\n", len, ret);
    return ret;
}

int msg_center_alipay_close_request(void)
{
    if (g_alipay_tcp_sem != NULL) {
        osSemaphoreDelete(g_alipay_tcp_sem);
        g_alipay_tcp_sem = NULL;
    }
    if (g_tcp_data != NULL) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "need free data buff.\r\n");
        free(g_tcp_data);
        g_tcp_data = NULL;
        g_tcp_data_readed = 0;
    }
    int ret;
    int send_ret =
        msg_center_send_data(MSGCENTER_CMD_ALIPAY, MSGCENTER_TYPE_ID_ALIPAY_CLOSE, &g_tcp_fd, sizeof(g_tcp_fd));
    if (send_ret != ERRCODE_SUCC) {
        ret = -1;
    } else {
        ret = 0;
    }
    g_tcp_fd = 0;
    return ret;
}

errcode_t msg_center_alipay_close(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);

    return ERRCODE_SUCC;
}

errcode_t msg_center_alipay_time_sync(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);

    uint32_t sec = *((uint32_t *)tlv_payload);
    struct timeval tv = { 0 };
    struct timezone tz = { 0 };

    tv.tv_sec = sec;
    tv.tv_usec = 0;
    tz.tz_dsttime = 0;
    tz.tz_minuteswest = (8 * 60);

    settimeofday(&tv, &tz);
    msg_center_alipay_reply_ack(cmd_id, type);
    return ERRCODE_SUCC;
}


#ifdef __cplusplus
}
#endif