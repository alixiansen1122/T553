#include "gnss_service.h"
#include <time.h>
#include <sys/time.h>
#include "dev_storage.h"

gnss_context_t gnss_cntx = {0};
pthread_mutex_t agps_mutex = PTHREAD_MUTEX_INITIALIZER;
bool gnss_factory_flag = 0;
int gnss_test_mode = 0;
extern uint64_t get_utc_time(void);

static int32_t get_utc_hour_time(void)
{
    struct timeval tv = {0};
    struct tm tm = {0};
    struct timezone tz = {0};

    gettimeofday(&tv, &tz);
    localtime_r(&tv.tv_sec, &tm);

    return tm.tm_hour;
}

static void gnss_agps_timer_handler(unsigned long data)
{
    int32_t ret = 0;
    (void)data;

    int tm_hour = get_utc_hour_time() + 8;
    tm_hour = tm_hour % 24;
    GNSS_SERVICE_PRINT("[agps timer]tm_hour = %d\n", tm_hour);

    if ((tm_hour <= 23) && (tm_hour >= 6))
    {
        char last_tm_hour = 0;
        watch_storage_get(STORAGE_TM_HOUR, &last_tm_hour, 1);
        if (last_tm_hour != (char)tm_hour)
        {
            GNSS_SERVICE_PRINT("[agps timer]start to get agps data, last_tm_hour = %d\n", last_tm_hour);
            watch_storage_set(STORAGE_TM_HOUR, &tm_hour, 1);
            http_send_type_to_server(HTTP_GET_AGPS);
        }
    }

    osal_timer_mod(&gnss_cntx.timer_agps, 2 * 60 * 60 * 1000);
}

static void gnss_msg_task(void *para)
{
    errcode_t ret = 0;
    gnss_msg_item_t msg = {0};
    unsigned int size = sizeof(gnss_msg_item_t);

    while (gnss_cntx.gnss_queue_id == 0)
    {
        osDelay(10);
    }
    while (1)
    {
        memset(&msg, 0, sizeof(gnss_msg_item_t));
        if (osal_msg_queue_read_copy(gnss_cntx.gnss_queue_id, (void *)&msg, &size, OSAL_MSGQ_WAIT_FOREVER) != OSAL_SUCCESS)
        {
            continue;
        }
        switch (msg.msg_id)
        {
        case MSG_GNSS_INIT:
        case MSG_GNSS_START:
        {
            if (gnss_cntx.gnss_inited == 0)
            {
                ret = custom_gnss_init();
                if (ret != 0)
                {
                    GNSS_SERVICE_PRINT("custom_gnss_init failed, ret=0x%x\n", ret);
                    continue;
                }
                gnss_cntx.gnss_inited = 1;
                GNSS_SERVICE_PRINT("custom_gnss_init success\n");
            }
            gnss_info_init();
            int32_t err = 0;
            err = access("/user/xgnss/AGNSS.dat", F_OK);
            if (err < 0)
            {
                GNSS_SERVICE_PRINT("not a file AGNSS.dat\n");
            }
            else
            {
                /* 处在工厂测试模式，不做agps数据注入 */
                if (!gnss_factory_flag && gnss_test_mode == 0)
                {
                    GNSS_SERVICE_PRINT("gnss agps mutex start\n");
                    pthread_mutex_lock(&agps_mutex);
                    ret = gnss_agnss();
                    if (ret != 0)
                    {
                        GNSS_SERVICE_PRINT("gnss_agnss failed, ret=0x%x", ret);
                    }
                    pthread_mutex_unlock(&agps_mutex);
                    GNSS_SERVICE_PRINT("gnss agps mutex end\n");
                }
            }
            if (gnss_test_mode == 1)
            {
                //GNSS_SERVICE_PRINT("start config cold %lu\n", get_utc_time());
                custom_gnss_cold_start();
            }
            else if (gnss_test_mode == 2)
            {
                //GNSS_SERVICE_PRINT("start config hot %lu\n", get_utc_time());
                custom_gnss_hot_start();
            }
            else if (gnss_test_mode == 3)
            {
                custom_gnss_factory_test();
            }
            ret = custom_gnss_start();
            if (ret != 0)
            {
                GNSS_SERVICE_PRINT("custom_gnss_start failed, ret=0x%x", ret);
                continue;
            }
            GNSS_SERVICE_PRINT("custom_gnss_start success\n");
        }
        break;
        case MSG_GNSS_STOP:
        {
            ret = custom_gnss_stop();
            if (ret != 0)
            {
                GNSS_SERVICE_PRINT("custom_gnss_stop failed, ret=0x%x", ret);
                continue;
            }
            GNSS_SERVICE_PRINT("custom_gnss_stop success\n");
        }
        break;
        case MSG_GNSS_DEINIT:
        {
            ret = custom_gnss_deinit();
            if (ret != 0)
            {
                GNSS_SERVICE_PRINT("custom_gnss_deinit failed, ret=0x%x", ret);
                continue;
            }
            gnss_cntx.gnss_inited = 0;
            GNSS_SERVICE_PRINT("custom_gnss_deinit success\n");
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


int gnss_task_init(void)
{
    osThreadAttr_t threadAttr = {0};

    if (gnss_cntx.isInit)
    {
        goto EXIT0;
    }
    if (OSAL_FAILURE == osal_msg_queue_create(GNSS_MSGQ_NAME, GNSS_MSGQ_SIZE, &gnss_cntx.gnss_queue_id, 0, sizeof(gnss_msg_item_t)))
    {
        goto EXIT0;
    }
    memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = GNSS_TASK_NAME;
    threadAttr.stack_size = GNSS_STACK_SIZE;
    threadAttr.priority = GNSS_TASK_PRIORITY;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);
    gnss_cntx.gnss_task_ref = osThreadNew(gnss_msg_task, NULL, &threadAttr);
    if (NULL == gnss_cntx.gnss_task_ref)
    {
        goto EXIT1;
    }

    gnss_cntx.timer_agps.handler = gnss_agps_timer_handler;
    gnss_cntx.timer_agps.interval = 2*60*1000;
    gnss_cntx.timer_agps.data = 0;
    if (OSAL_SUCCESS != osal_timer_init(&gnss_cntx.timer_agps))
    {
        goto EXIT2;
    }

    /* 开启agps的定时器 */
    osal_timer_start(&gnss_cntx.timer_agps);

    gnss_mode_test_init();
    gnss_cntx.isInit = 1;
    GNSS_SERVICE_PRINT("gnss task inited success!\n");

    return 0;
EXIT3:
    osal_timer_destroy(&gnss_cntx.timer_agps);
EXIT2:
    if (gnss_cntx.gnss_task_ref != NULL)
    {
        osThreadTerminate(gnss_cntx.gnss_task_ref);
    }
EXIT1:
    osal_msg_queue_delete(gnss_cntx.gnss_queue_id);
EXIT0:
    return -1;
}

int32_t gnss_send_msg(uint32_t msg_id, uint32_t length, void *data)
{
    int32_t ret = OSAL_FAILURE;
    gnss_msg_item_t msg = {0};

    if (!gnss_cntx.isInit)
    {
        goto EXIT;
    }
    if (0 == gnss_cntx.gnss_queue_id)
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
    ret = osal_msg_queue_write_copy(gnss_cntx.gnss_queue_id, (void *)&msg, sizeof(gnss_msg_item_t), OSAL_MSGQ_WAIT_FOREVER);
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

/*  工厂测试接口 */
void gnss_device_check_start(void)
{
    gnss_factory_flag = 1;
    gnss_send_msg(MSG_GNSS_INIT, 0, NULL);
    /* 延时1s,用于gnss前期固件烧录 */
    osal_msleep(1000*5);
}

void gnss_device_check_end(void)
{
    /* 关闭gnss */
    gnss_send_msg(MSG_GNSS_STOP, 0, NULL);
    /* 卸载gnss */
    //gnss_send_msg(MSG_GNSS_DEINIT, 0, NULL);

    gnss_factory_flag = 0;
}

void gnss_check_init(void)
{
    /* 关闭串口打印nmea数据 */
    custom_update_gnss_nmea_print_flag(0);
    gnss_device_check_start();
    gnss_device_check_end();
}
