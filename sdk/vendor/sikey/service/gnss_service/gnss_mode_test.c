#include "gnss_service.h"

osThreadId_t gnss_test_task_ref;
unsigned long gnss_test_queue_id;
osal_timer timer_at_test;

unsigned long timestamp_start = 0;
unsigned long timestamp_end = 0;
extern uint64_t get_utc_time(void);

static void gnss_test_timer_handler(unsigned long data)
{
    GNSS_SERVICE_PRINT("gnss_test_timer_handler, gnss_test_mode = %d\n", gnss_test_mode);
    if (gnss_test_mode == 1)
    {
        gnss_test_send_msg(MSG_GNSS_COLD_TEST, 0, NULL);
    }
    else if (gnss_test_mode == 2)
    {
        gnss_test_send_msg(MSG_GNSS_HOT_TEST, 0, NULL);
    }
}

static void gnss_test_msg_task(void *para)
{
    errcode_t ret = 0;
    gnss_msg_item_t msg = {0};
    unsigned int size = sizeof(gnss_msg_item_t);
    char buf[128] = {0};

    while (1)
    {
        memset(buf, 0, 128);
        memset(&msg, 0, sizeof(gnss_msg_item_t));
        if (osal_msg_queue_read_copy(gnss_test_queue_id, (void *)&msg, &size, OSAL_MSGQ_WAIT_FOREVER) != OSAL_SUCCESS)
        {
            continue;
        }
        switch (msg.msg_id)
        {
        case MSG_GNSS_COLD_TEST:
        {
            custom_update_nmea_save_flag(1);
            timestamp_start = get_utc_time() + 28800;
            snprintf(buf, 128, "[gnss_test]cold_test_start=%lu", timestamp_start);
            custom_gnss_save_data(1, buf, 128);
            GNSS_SERVICE_PRINT("[gnss_test]cold_test_start = %lu\n", timestamp_start);
            gnss_test_mode = 1;
            gnss_send_msg(MSG_GNSS_INIT, 0, NULL);
            break;
        }
        case MSG_GNSS_HOT_TEST:
        {
            custom_update_nmea_save_flag(1);
            timestamp_start = get_utc_time() + 28800;
            snprintf(buf, 128, "[gnss_test]hot_test_start=%lu", timestamp_start);
            custom_gnss_save_data(1, buf, 128);
            GNSS_SERVICE_PRINT("[gnss_test]hot_test_start = %lu\n", timestamp_start);
            gnss_test_mode = 2;
            gnss_send_msg(MSG_GNSS_INIT, 0, NULL);
            break;
        }
        case MSG_GNSS_STOP_TEST:
        {
            timestamp_end = get_utc_time() + 28800;
            snprintf(buf, 128, "[gnss_test]end test=%lu,finish time=%lu", timestamp_end, timestamp_end - timestamp_start);
            custom_gnss_save_data(1, buf, 128);
            GNSS_SERVICE_PRINT("[gnss_test]end test=%lu,finish time=%lu\n", timestamp_end, timestamp_end - timestamp_start);
            if (gnss_test_mode != 0)
            {
                gnss_send_msg(MSG_GNSS_STOP, 0, NULL);
                osal_timer_mod(&timer_at_test, 5 * 1000);
            }
            break;
        }
        case MSG_GNSS_END_TEST:
        {
            snprintf(buf, 128, "[gnss_test]close gnss");
            custom_gnss_save_data(1, buf, 128);
            GNSS_SERVICE_PRINT("[gnss_test]close gnss\n");
            gnss_test_mode = 0;
            osal_timer_stop(&timer_at_test);
            gnss_send_msg(MSG_GNSS_STOP, 0, NULL);
            gnss_send_msg(MSG_GNSS_DEINIT, 0, NULL);
            break;
        }
        case MSG_GNSS_CNR_TEST_START:
        {
            GNSS_SERVICE_PRINT("+GPS:FACTORY start\n");
            gnss_test_mode = 3;
            gnss_send_msg(MSG_GNSS_INIT, 0, NULL);
            break;
        }
        case MSG_GNSS_CNR_TEST_END:
        {
            GNSS_SERVICE_PRINT("+GPS:FACTORY end\n");
            gnss_test_mode = 0;
            gnss_send_msg(MSG_GNSS_STOP, 0, NULL);
            break;
        }
        default:
            break;
        }

        if (msg.data)
        {
            free(msg.data);
            msg.data = NULL;
        }
    }
}

void gnss_mode_test_init(void)
{
    osThreadAttr_t threadAttr = {0};

    if (OSAL_FAILURE == osal_msg_queue_create(GNSS_TEST_MSGQ_NAME, GNSS_TEST_MSGQ_SIZE, &gnss_test_queue_id, 0, sizeof(gnss_msg_item_t)))
    {
        goto EXIT0;
    }
    memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = GNSS_TASK_NAME;
    threadAttr.stack_size = GNSS_STACK_SIZE;
    threadAttr.priority = GNSS_TASK_PRIORITY;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);
    gnss_test_task_ref = osThreadNew(gnss_test_msg_task, NULL, &threadAttr);
    if (NULL == gnss_test_task_ref)
    {
        goto EXIT1;
    }
    timer_at_test.handler = gnss_test_timer_handler;
    timer_at_test.interval = 3 * 60 * 1000;
    timer_at_test.data = 0;
    if (OSAL_SUCCESS != osal_timer_init(&timer_at_test))
    {
        goto EXIT2;
    }
    return;
EXIT2:
    if (gnss_test_task_ref != NULL)
    {
        osThreadTerminate(gnss_test_task_ref);
    }
EXIT1:
    osal_msg_queue_delete(gnss_test_queue_id);
EXIT0:
    return;
}

int32_t gnss_test_send_msg(uint32_t msg_id, uint32_t length, void *data)
{
    int32_t ret = OSAL_FAILURE;
    gnss_msg_item_t msg = {0};

    if (0 == gnss_test_queue_id)
    {
        goto EXIT;
    }
    memset(&msg, 0x00, sizeof(msg));
    msg.msg_id = msg_id;
    if (length > 0 && data != NULL)
    {
        msg.length = length;
        msg.data = (uint8_t *)malloc(sizeof(uint8_t) * length);
        if (!msg.data)
        {
            goto EXIT;
        }
        memset(msg.data, 0, length);
        memcpy(msg.data, data, length);
    }
    ret = osal_msg_queue_write_copy(gnss_test_queue_id, (void *)&msg, sizeof(gnss_msg_item_t), OSAL_MSGQ_WAIT_FOREVER);
EXIT:
    if (ret == OSAL_FAILURE)
    {
        if (msg.data)
        {
            free(msg.data);
        }
    }
    return ret;
}

void gnss_start_test(int test_mode)
{
    GNSS_SERVICE_PRINT("gnss_code_start_test\n");
    gnss_test_mode = test_mode;
    gnss_factory_flag = 1;
    gnss_send_msg(MSG_GNSS_INIT, 0, NULL);
}