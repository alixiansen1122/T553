/*
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: gnss manage code
 */
#include "stdint.h"
#include "stdlib.h"
#include "securec.h"
#include "osal_semaphore.h"
#include "osal_msgqueue.h"
#include "osal_task.h"
#include "soc_osal.h"
#include "common_def.h"
#include "tiot_service_interface.h"
#include "errcode.h"
#include "gpio.h"
#include "los_base.h"
#include "pinctrl.h"
#include "gnss_device.h"

#define GNSS_MSG_TYPE_LIMIT 2
#define GNSS_THREAD_STACK_SIZE  4096
#define GNSS_THREAD_PRIORITY 8
#define GNSS_DEVICE_MSG_LEN  8
#define GNSS_MSG_QUEUE_LEN 64
#define GNSS_ACK_FLAG 0xff01
#define GNSS_CONFIG_TIMEOUT_MS 500
#define GNSS_OPEN_CALLBACK_BUF_LEN 10240
#define GNSS_MSG_HEAD_LEN 2
#define GNSS_CONFIG_QUERY_TYPE 5

typedef struct {
    uint16_t  data_type;         /*!< @if Eng Data type
                                         @else   消息类型                                               @endif */
    uint16_t  payload_length;       /*!< @if Eng Data length for .payload field, not include header 8-bytes
                                         @else   消息载荷长度                                           @endif */
    uint8_t   *payload;           /*!< @if Eng Data payload
                                         @else   消息载荷                                               @endif */
} gnss_queue_msg_type_t;

typedef struct {
    uint16_t  cmd;         /*!< @if Eng  Cmd type
                                         @else   消息类型                                               @endif */
    uint16_t  status;       /*!< @if Eng Ack status
                                         @else   ack状态                                                @endif */
} gnss_ack_payload_type_t;

static osal_semaphore g_gnss_config_sem;
static gnss_callback_func g_gnss_callback[GNSS_MSG_TYPE_LIMIT] = {0};
static osal_task *g_gnss_thread = NULL;
static unsigned long g_queue_id = 0;
static tiot_handle g_handle = 0;
static tiot_service_open_param g_open_param;
static uint32_t g_ack_status = 0;
static uint16_t g_config_type = 0;
static uint16_t g_gnss_init_status = 0;

errcode_t uapi_gnss_register_callback(uint32_t callback_type, gnss_callback_func callback_func)
{
    if (callback_type != GNSS_CALLBACK_NMEA && callback_type != GNSS_CALLBACK_MSG) {
        return ERRCODE_GNSS_CALLBACK_TYPE_ERROR;
    }
    if (callback_func == NULL) {
        return ERRCODE_GNSS_CALLBACK_FUNC_ERROR;
    }

    if (g_gnss_callback[callback_type] != NULL) {
        return ERRCODE_GNSS_CALLBACK_EXISTED;
    }
    g_gnss_callback[callback_type] = callback_func;
    return ERRCODE_SUCC;
}

static void gnss_write_to_msg_queue(const uint8_t *buff, uint32_t len)
{
    if (buff == NULL || len <= GNSS_MSG_HEAD_LEN) {
        return;
    }
    gnss_message_t *gnss_msg = (gnss_message_t *)(buff + GNSS_MSG_HEAD_LEN);
    uint16_t data_type = *(uint16_t *)buff;
    if (data_type == GNSS_CALLBACK_MSG) {
        if (gnss_msg->message_type == GNSS_ACK_FLAG) {
            gnss_ack_payload_type_t *ack_payload = (gnss_ack_payload_type_t *)gnss_msg->payload;
            if (ack_payload->cmd != g_config_type) {
                printf("wrong ack! g_config_type %u, ack_number %u\r\n", g_config_type, ack_payload->cmd);
                return;
            }
            g_ack_status = ack_payload->status;
            osal_sem_up(&g_gnss_config_sem);
            return;
        }
    }
    uint32_t payload_len = len - GNSS_MSG_HEAD_LEN;
    gnss_queue_msg_type_t msg;
    msg.payload = (uint8_t *)malloc(payload_len);
    if (msg.payload == NULL) {
        printf("allocate memory fail\r\n");
        return;
    }
    if (memcpy_s(msg.payload, payload_len, buff + GNSS_MSG_HEAD_LEN, payload_len) != 0) {
        free(msg.payload);
        msg.payload = NULL;
        return;
    }
    msg.payload_length = payload_len;
    msg.data_type = *(uint16_t *)buff;
    if (osal_msg_queue_write_copy(g_queue_id, (void*)&msg, sizeof(gnss_queue_msg_type_t),
        LOS_NO_WAIT) != OSAL_SUCCESS) {
        free(msg.payload);
        msg.payload = NULL;
        return;
    }
}

static void gnss_process_msg(uint8_t *buff, uint32_t len)
{
    unused(len);
    if (buff == NULL || len == 0) {
        return;
    }
    gnss_queue_msg_type_t *msg = (gnss_queue_msg_type_t *)buff;
    uint16_t data_type = msg->data_type;
    if ((data_type != 0) && (data_type != 1)) {
        free(msg->payload);
        msg->payload = NULL;
        return;
    }
    if (g_gnss_callback[data_type] != NULL) {
        g_gnss_callback[data_type](msg->payload, msg->payload_length);
    }
    if (msg->payload != NULL) {
        free(msg->payload);
        msg->payload = NULL;
    }
}

static int gnss_thread(void *param)
{
    unused(param);
    gnss_queue_msg_type_t recv_buffer = {0};
    uint32_t recv_buffer_len = sizeof(gnss_queue_msg_type_t);
    while (1) {
        memset_s((void *)(&recv_buffer), recv_buffer_len, 0, recv_buffer_len);
        osal_msg_queue_read_copy(g_queue_id, (void *)(&recv_buffer), &recv_buffer_len, OSAL_MSGQ_WAIT_FOREVER);
        gnss_process_msg((uint8_t *)(&recv_buffer), recv_buffer_len);
    }
    return 0;
}

errcode_t uapi_gnss_init(void)
{
    uapi_pin_set_mode(S_AGPIO_L17, PIN_MODE_0);
    uapi_gpio_set_dir(S_AGPIO_L17, HAL_GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO_L17, HAL_GPIO_LEVEL_HIGH);
    if (g_gnss_init_status == 1) {
        return ERRCODE_SUCC;
    }
    if (osal_msg_queue_create(NULL, GNSS_MSG_QUEUE_LEN, &g_queue_id, 0, GNSS_DEVICE_MSG_LEN) != 0) {
        return ERRCODE_FAIL;
    }
    osal_kthread_lock();
    g_gnss_thread = osal_kthread_create(gnss_thread, NULL, "gnss_thread", GNSS_THREAD_STACK_SIZE);
    if (g_gnss_thread == NULL) {
        osal_msg_queue_delete(g_queue_id);
        osal_kthread_unlock();
        return ERRCODE_GNSS_CREATE_THREAD_FAILED;
    }
    osal_kthread_set_priority(g_gnss_thread, GNSS_THREAD_PRIORITY);
    osal_kthread_unlock();
    osal_sem_binary_sem_init(&g_gnss_config_sem, 0);
#ifndef CONFIG_PRODUCT_EVB_DITING
    int32_t ret = tiot_service_init();
    if (ret != 0) {
        osal_kthread_destroy(g_gnss_thread, 1);
        osal_msg_queue_delete(g_queue_id);
        osal_sem_destroy(&g_gnss_config_sem);
        return ERRCODE_FAIL;
    }
#endif
    g_gnss_init_status = 1;
    return ERRCODE_SUCC;
}

errcode_t uapi_gnss_open(void)
{
    if (g_handle != 0) {
        return ERRCODE_FAIL;
    }
    g_open_param.buff = (uint8_t *)malloc(GNSS_OPEN_CALLBACK_BUF_LEN);
    if (g_open_param.buff == NULL) {
        return ERRCODE_FAIL;
    }
    g_open_param.buff_len = GNSS_OPEN_CALLBACK_BUF_LEN;
    g_open_param.rx_callback = gnss_write_to_msg_queue;
    g_open_param.flags = 0;
    g_handle = tiot_service_open("gn71", &g_open_param);
    if (g_handle == 0) {
        free(g_open_param.buff);
        g_open_param.buff = NULL;
        return ERRCODE_GNSS_OPEN_GNSS_FAILED;
    }
#if defined(CONFIG_GNSS_HSUART)
    uapi_pin_set_pull(S_MGPIO33, HAL_PIO_PULL_UP);  // 解决主控侧uart tx电平在睡眠后无法保持高电平，导致误唤醒GS21的问题
#else
    uapi_pin_set_pull(S_MGPIO19, HAL_PIO_PULL_UP);  // 解决主控侧uart tx电平在睡眠后无法保持高电平，导致误唤醒GS21的问题
#endif
    return ERRCODE_SUCC;
}

errcode_t uapi_gnss_config(gnss_message_t *msg)
{
    if (msg == NULL) {
        return ERRCODE_FAIL;
    }
    g_config_type = msg->message_type;
    int32_t ret = tiot_service_write(g_handle, (uint8_t *)msg, sizeof(gnss_message_t)+ msg->payload_length);
    if (ret < 0) {
        g_config_type = 0;
        return ERRCODE_FAIL;
    }
    if (g_config_type == GNSS_CONFIG_QUERY_TYPE) {
        return ERRCODE_SUCC;
    }
    ret = osal_sem_down_timeout(&g_gnss_config_sem, GNSS_CONFIG_TIMEOUT_MS);
    if (ret < 0) {
        return ERRCODE_FAIL;
    }
    switch (g_ack_status) {
        case 0x0000:
            return ERRCODE_SUCC;
        case 0x0001:
            return ERRCODE_GNSS_CONFIG_ITEM_LEN_ERROR;
        case 0x0002:
            return ERRCODE_GNSS_CONFIG_ITEM_PARA_ERROR;
        case 0x0003:
            return ERRCODE_GNSS_CONFIG_ITEM_TYPE_ERROR;
        case 0xFFFF:
            return ERRCODE_GNSS_CONFIG_ITEM_PROC_ERROR;
        default:
            return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_gnss_start(void)
{
    const uint8_t qryVersion[] = {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

    errcode_t ret = uapi_gnss_config(qryVersion);
    if (ret != ERRCODE_SUCC) {
        printf("[GNSS ERROR]request version fail, error code: %#x\n", ret);
    }

    const uint8_t startCmd[] = {0x01, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    return uapi_gnss_config((gnss_message_t *)startCmd);
}

errcode_t uapi_gnss_stop(void)
{
    const uint8_t stopCmd[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    return uapi_gnss_config((gnss_message_t *)stopCmd);
}

errcode_t uapi_gnss_close(void)
{
    if (g_handle == 0) {
        return ERRCODE_FAIL;
    }
    tiot_service_close(g_handle);
    g_handle = 0;
    if (g_open_param.buff != NULL) {
        free(g_open_param.buff);
        g_open_param.buff = NULL;
    }
    memset_s(g_gnss_callback, sizeof(g_gnss_callback), 0, sizeof(g_gnss_callback));
    return ERRCODE_SUCC;
}

errcode_t uapi_gnss_deinit(void)
{
    uapi_pin_set_mode(S_AGPIO_L17, PIN_MODE_0);
    uapi_gpio_set_dir(S_AGPIO_L17, HAL_GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO_L17, HAL_GPIO_LEVEL_LOW);
    osal_kthread_destroy(g_gnss_thread, 1);
    osal_msg_queue_delete(g_queue_id);
    osal_sem_destroy(&g_gnss_config_sem);
#ifndef CONFIG_PRODUCT_EVB_DITING
    tiot_service_deinit();
#endif
    g_gnss_init_status = 0;
    return ERRCODE_SUCC;
}

errcode_t uapi_gnss_power_on(void)
{
#ifdef CONFIG_PRODUCT_EVB_DITING
    uapi_pin_set_mode(S_AGPIO_L11, PIN_MODE_0);
    uapi_gpio_set_dir(S_AGPIO_L11, HAL_GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO_L11, HAL_GPIO_LEVEL_HIGH);

    uapi_pin_set_mode(S_AGPIO_L30, PIN_MODE_0);
    uapi_gpio_set_dir(S_AGPIO_L30, HAL_GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO_L30, HAL_GPIO_LEVEL_HIGH);
#endif
    return ERRCODE_SUCC;
}

errcode_t uapi_gnss_power_off(void)
{
#ifdef CONFIG_PRODUCT_EVB_DITING
    uapi_pin_set_mode(S_AGPIO_L11, PIN_MODE_0);
    uapi_gpio_set_dir(S_AGPIO_L11, HAL_GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO_L11, HAL_GPIO_LEVEL_LOW);

    uapi_pin_set_mode(S_AGPIO_L30, PIN_MODE_0);
    uapi_gpio_set_dir(S_AGPIO_L30, HAL_GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO_L30, HAL_GPIO_LEVEL_LOW);
#endif
    return ERRCODE_SUCC;
}