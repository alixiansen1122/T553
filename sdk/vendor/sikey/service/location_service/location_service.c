#include "location_service.h"
#include "ril_interface.h"
#include "modem_cmd.h"
#include "gnss_service.h"
#include "cJSON.h"
#include "net_data_process.h"
#include "mqtt_event.h"
location_context_t location_cntx = {0};

char signal_info_buf[64] = {0};

char bts_info_buf[128] = {0};
//char nearbts_info_buf[128] = {0};
bool getting_cell = 0;
pthread_mutex_t bts_buf_mutex;

bool is_report_wifilist = 0;
bool wifi_scan_done = 0;
bool wifi_scaning = 0;
char wifi_info_buf[WIFI_BUF_SIZE] = {0};
char wifi_serach_buf[WIFI_BUF_SIZE] = {0};
char wifi_info_temp[WIFI_BUF_SIZE] = {0};
pthread_mutex_t wifi_info_buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wifi_report_mutex = PTHREAD_MUTEX_INITIALIZER;

char gnss_info_buf[64] = {0};
pthread_mutex_t gnss_info_buf_mutex = PTHREAD_MUTEX_INITIALIZER;
extern bool gnss_is_fixed;
bool gnss_enable = 0;

osal_timer wifi_bt_timer;
static uint32_t event_count = 0;
osal_semaphore search_sem;

bool ConvStrToGPSValue(char *sStr, double *pdfValue, bool nIsLat)
{
    bool nResult = false;
    char sDegree[3 + 2] = {0};
    char sMinute[7 + 2] = {0};
    uint32_t nLen = 0;
    uint32_t nDegreeLen = 0;
    if (!sStr || !sStr[0] || !pdfValue) {
        return nResult;
    }
    nLen = strlen(sStr);
    if (nIsLat) {
        nDegreeLen = 2;
        if (nLen != 9) {
            return nResult;
        }
    } else {
        nDegreeLen = 3;
        if (nLen != 10) {
            return nResult;
        }
    }
    memcpy(sDegree, sStr, nDegreeLen);
    memcpy(sMinute, sStr + nDegreeLen, nLen - nDegreeLen);
    *pdfValue = (double)atoi(sDegree) + (double)atof(sMinute) / 60.0;
    nResult = true;
    return nResult;
}

void get_signal_info(void)
{
    int32_t ret = 0;
    soc_ue_cell_radio_info signal_info;

    ret = ril_get_radio_signal_info(&signal_info);
    if (ret == 0)
    {
        memset(signal_info_buf, 0, 64);
        snprintf(signal_info_buf, 64, "%d,%d,%d,%d",signal_info.rsrp, signal_info.rsrq, signal_info.rssi, signal_info.snr);
        LOCATION_SERVICE_PRINT("signal_info_buf = %s\n", signal_info_buf);
    }
    else
    {
        LOCATION_SERVICE_PRINT("get signal info failed\n");
    }
}

void get_wifilist(void)
{
    LOCATION_SERVICE_PRINT("get_wifilist\n");
    pthread_mutex_lock(&wifi_report_mutex);
    is_report_wifilist = 1;
    if (wifi_scaning == 0)
    {
        LOCATION_SERVICE_PRINT("get_wifilist start\n");
        //wifi_scaning = 1;
        wifi_scan();
    }
    pthread_mutex_unlock(&wifi_report_mutex);
}

/* 定时上报的接口,mode=0为常规定时上报wifi, mode=1为寻宠模式 */
void location_start(uint8_t mode)
{
    /* 开启 wifi scan */
    LOCATION_SERVICE_PRINT("location_start\n");
    if (mode && (gnss_enable == 0))
    {
        LOCATION_SERVICE_PRINT("gnss scan start\n");
        gnss_enable = 1;
        memset(wifi_serach_buf, 0, WIFI_BUF_SIZE);
        custom_update_gnss_nmea_print_flag(0);
        /* 如果是在寻宠模式，同时开启gnss */
        gnss_send_msg(MSG_GNSS_INIT, 0, NULL);
        /* 首次开启定时器用于定时上报定位数据 */
        osal_timer_start(&location_cntx.timer_location);
    }
}

void searching_stop(void)
{
    LOCATION_SERVICE_PRINT("location_stop\n");
    if (gnss_enable == 1) {
        event_count = 0;
        osal_timer_stop(&location_cntx.timer_location);
        /* 关闭gnss */
        gnss_send_msg(MSG_GNSS_STOP, 0, NULL);
        /* 卸载gnss */
        //gnss_send_msg(MSG_GNSS_DEINIT, 0, NULL);
        gnss_enable = 0;
    }
}

static void location_timer_handler(unsigned long data)
{
    (void)data;
    LOCATION_SERVICE_PRINT("location_timer_handler start\n");
    osal_sem_up(&search_sem);
}

static void wifi_bt_timer_handler(unsigned long data)
{
    (void)data;
    /* 获取信号强度 */
    get_signal_info();
    LOCATION_SERVICE_PRINT("wifi_bt_timer_handler start\n");
    /* 开启wifi scan */
    if (wifi_scaning == 0)
    {
        LOCATION_SERVICE_PRINT("wifi scan start\n");
        wifi_scan();
    }
}

static void search_msg_task(void *para)
{
    int retry = 0;
    LOCATION_SERVICE_PRINT("search_msg_task start\n");
    while (1)
    {
        osal_sem_down(&search_sem);
        event_count++;

        /* 先做wifi scan */
        if ((event_count & 1) == 0)
        {

            LOCATION_SERVICE_PRINT("wifi scan start\n");
            wifi_scaning = 1;
            wifi_scan_done = 0;
            retry = 5;
            wifi_scan();
            /* 等待扫描结果 */
            while ((wifi_scan_done == 0) && retry > 0)
            {
                osal_msleep(3000);
                retry--;
            }
            LOCATION_SERVICE_PRINT("wifi scan %s, retry count = %d\n", wifi_scan_done ? "pass" : "failed", 5 - retry);
        }
        else
        {
#if 0
            retry = 2;
            getting_cell = 0;
            /* 获取基站信息 */
            LOCATION_SERVICE_PRINT("get cell start\n");
            get_cell_info();
            while ((getting_cell == 0) && retry > 0)
            {
                osal_msleep(3000);
                retry--;
            }
            LOCATION_SERVICE_PRINT("getting cell %s, retry count = %d\n", getting_cell ? "pass" : "failed", 2 - retry);
#else
            /* 获取基站信息 */
            LOCATION_SERVICE_PRINT("get cell start\n");
            get_cell_info();
#endif
            /* 获取信号强度 */
            get_signal_info();
        }
        /* 上报定位信息 */
        ws_report_location();
        /* 开启定时器用于定时上报定位数据 */
        osal_timer_mod(&location_cntx.timer_location, 10 * 1000);
    }
}

static void location_msg_task(void *para)
{
    location_msg_item_t msg = {0};
    unsigned int size = sizeof(location_msg_item_t);

    while (location_cntx.location_queue_id == 0)
    {
        osDelay(10);
    }
    while (1)
    {
        memset(&msg, 0, sizeof(location_msg_item_t));
        if (osal_msg_queue_read_copy(location_cntx.location_queue_id, (void *)&msg, &size, OSAL_MSGQ_WAIT_FOREVER) != OSAL_SUCCESS)
        {
            continue;
        }
        switch (msg.msg_id)
        {
        case MSG_CELL_GET_DONE:
        {
            WATCH_CELL_INFO *cell = (WATCH_CELL_INFO *)msg.data;
            pthread_mutex_lock(&bts_buf_mutex);
            memset(bts_info_buf, 0, 128);
            //memset(nearbts_info_buf, 0, 128);
            LOCATION_SERVICE_PRINT("get cell done\n");
            snprintf(bts_info_buf, 128, "%u,%u,%lu,%llu,%d,%d", cell->resident_cell_info.mcc, cell->resident_cell_info.mnc,
                    cell->resident_cell_info.tac, cell->resident_cell_info.cellid, cell->resident_cell_info.strength, cell->resident_cell_info.cage);
            LOCATION_SERVICE_PRINT("bts_info_buf = %s\n",bts_info_buf);
            #if 0
            for (int i = 0; i < cell->neighbor_cell_num; i++)
            {
                snprintf(nearbts_info_buf, 128, "%u,%u,%lu,%llu,%d,%d|", cell->neighbor_cell_list[i].mcc, cell->neighbor_cell_list[i].mnc,
                    cell->neighbor_cell_list[i].tac,cell->neighbor_cell_list[i].cellid, cell->neighbor_cell_list[i].strength, cell->neighbor_cell_list[i].cage);
            }
            #endif
            pthread_mutex_unlock(&bts_buf_mutex);
            getting_cell = 1;
            break;
        }
        case MSG_WIFI_SCAN_DONE:
        {
            WifiAPInfo *wifi_info = (WifiAPInfo *)msg.data;
            uint32_t count = msg.length / sizeof(WifiAPInfo);
            uint32_t offset = 0;

            LOCATION_SERVICE_PRINT("wifi scan done, get wifi number %d\n", count);
            //LOCATION_SERVICE_PRINT("[location_service]wifi info mutex start\n");
            pthread_mutex_lock(&wifi_info_buf_mutex);
            memset(wifi_info_buf, 0, WIFI_BUF_SIZE);
            for (int i = 0; i < count; i++, wifi_info++)
            {
                char buf[64] = {0};
                size_t len = 0;
                snprintf(buf, 64, "%d,%d,%s|", wifi_info->channel_num, wifi_info->rssi, wifi_info->bssid);
                len = strlen(buf);
                memcpy(wifi_info_buf + offset, buf, len);
                offset = offset + len;
            }
            //LOCATION_SERVICE_PRINT("[location_service] location_flag = %d\n", location_flag);
            if (location_flag)
            {
                memset(wifi_serach_buf, 0, WIFI_BUF_SIZE);
                memcpy(wifi_serach_buf, wifi_info_buf, WIFI_BUF_SIZE);
            }
            memset(wifi_info_temp, 0, WIFI_BUF_SIZE);
            memcpy(wifi_info_temp, wifi_info_buf, WIFI_BUF_SIZE);
            pthread_mutex_unlock(&wifi_info_buf_mutex);
            //LOCATION_SERVICE_PRINT("[location_service]wifi info mutex end\n");
            pthread_mutex_lock(&wifi_report_mutex);
            LOCATION_SERVICE_PRINT("[location_service]is_report_wifilist = %d\n", is_report_wifilist);
            if (is_report_wifilist)
            {
                ws_report_wifilist();
                is_report_wifilist = 0;
            }
            wifi_scaning = 0;
            wifi_scan_done = 1;
            pthread_mutex_unlock(&wifi_report_mutex);
            break;
        }
        case MSG_GNSS_SCAN_DONE:
        {
            LOCATION_SERVICE_PRINT("gnss scan done\n");
            gnss_location_info_t *loc_info = (gnss_location_info_t *)msg.data;
            double gps_lat = 0;
            double gps_lng = 0;
            int32_t radius = 0;
            if (loc_info)
            {
                //LOCATION_SERVICE_PRINT("latitude = %s\n", loc_info->latitude);
                //LOCATION_SERVICE_PRINT("longitude = %s\n", loc_info->longitude);
                ConvStrToGPSValue(loc_info->latitude, &gps_lat, 1);
                ConvStrToGPSValue(loc_info->longitude, &gps_lng, 0);
                if (strlen(loc_info->pdop) > 0)
                {
                    char *token = strtok(loc_info->pdop, ".");
                    radius = (int)atof(loc_info->pdop);
                    if (radius == 0 && token)
                    {
                        radius = 1;
                    }
                }
                else
                {
                    char *token = strtok(loc_info->hdop, ".");
                    radius = (int)atof(loc_info->hdop);
                    if (radius == 0 && token)
                    {
                        radius = 1;
                    }
                }
                if (radius < 0)
                {
                    radius = 0;
                }
                //LOCATION_SERVICE_PRINT("radius = %d\n", radius);
                //LOCATION_SERVICE_PRINT("latitude = %f\n", gps_lat);
                //LOCATION_SERVICE_PRINT("longitude = %f\n", gps_lng);
                //LOCATION_SERVICE_PRINT("[location_service]gnss info mutex start\n");
                pthread_mutex_lock(&gnss_info_buf_mutex);
                memset(gnss_info_buf, 0, 64);
                gps_lat = (toupper(loc_info->latitude_dir) == 'N') ? gps_lat : -gps_lat;
                gps_lng = (toupper(loc_info->longitude_dir) == 'E') ? gps_lng : -gps_lng;
                snprintf(gnss_info_buf, 64, "%f,%f,%d", gps_lng, gps_lat, radius);
                pthread_mutex_unlock(&gnss_info_buf_mutex);
                gnss_is_fixed = 0;
                mqtt_event_gnss_fix(gps_lat, gps_lng);
                //LOCATION_SERVICE_PRINT("[location_service]gnss info mutex end\n");
            }
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

int location_task_init(void)
{
    osThreadAttr_t threadAttr = {0};

    if (location_cntx.isInit)
    {
        goto EXIT0;
    }
    if (OSAL_FAILURE == osal_msg_queue_create(LOCATION_MSGQ_NAME, LOCATION_MSGQ_SIZE, &location_cntx.location_queue_id, 0, sizeof(location_msg_item_t)))
    {
        goto EXIT0;
    }
    memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = LOCATION_TASK_NAME;
    threadAttr.stack_size = LOCATION_STACK_SIZE;
    threadAttr.priority = LOCATION_TASK_PRIORITY;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);
    location_cntx.location_task_ref = osThreadNew(location_msg_task, NULL, &threadAttr);
    if (NULL == location_cntx.location_task_ref)
    {
        goto EXIT1;
    }
    /* 添加寻宠上报线程 */
    memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = "pet_search_task";
    threadAttr.stack_size = LOCATION_STACK_SIZE;
    threadAttr.priority = LOCATION_TASK_PRIORITY;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);
    location_cntx.search_task_ref = osThreadNew(search_msg_task, NULL, &threadAttr);
    if (NULL == location_cntx.search_task_ref)
    {
        goto EXIT2;
    }    
    /* 定时定位任务 */
    location_cntx.timer_location.handler = location_timer_handler;
    location_cntx.timer_location.interval = 1 * 1000;
    location_cntx.timer_location.data = NULL;
    if (OSAL_SUCCESS != osal_timer_init(&location_cntx.timer_location))
    {
        goto EXIT3;
    }

    /*wifi scan以及基站信息 */
    wifi_bt_timer.handler = wifi_bt_timer_handler;
    wifi_bt_timer.interval = 10;
    wifi_bt_timer.data = NULL;
    if (OSAL_SUCCESS != osal_timer_init(&wifi_bt_timer))
    {
        goto EXIT4;
    }
    osal_sem_init(&search_sem, 0);
    location_cntx.isInit = 1;
    LOCATION_SERVICE_PRINT("location task inited success!\n");

    return 0;
EXIT4:
    osal_timer_destroy(&location_cntx.timer_location);
EXIT3:
    if (location_cntx.search_task_ref != NULL)
    {
        osThreadTerminate(location_cntx.search_task_ref);
    }
EXIT2:
    if (location_cntx.location_task_ref != NULL)
    {
        osThreadTerminate(location_cntx.location_task_ref);
    }
EXIT1:
    osal_msg_queue_delete(location_cntx.location_queue_id);
EXIT0:
    return -1;
}

int32_t location_send_msg(uint32_t msg_id, uint32_t length, void *data)
{
    int32_t ret = OSAL_FAILURE;
    location_msg_item_t msg = {0};

    if (!location_cntx.isInit)
    {
        goto EXIT;
    }
    if (0 == location_cntx.location_queue_id)
    {
        goto EXIT;
    }
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
    ret = osal_msg_queue_write_copy(location_cntx.location_queue_id, (void *)&msg, sizeof(location_msg_item_t), OSAL_MSGQ_WAIT_FOREVER);
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
