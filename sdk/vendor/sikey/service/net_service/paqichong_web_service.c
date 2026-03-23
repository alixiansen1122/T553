#include "net_websocket.h"
#include "cmsis_os2.h"
#include "paqichong_web_service.h"
#include "osal_msgqueue.h"
#include "soc_osal.h"
#include "net_data_process.h"
#include "cJSON.h"
#include "osal_timer.h"
#include "osal_semaphore.h"
#include "dev_storage.h"
#include "dfx_reboot.h"
#include "location_service.h"
#include "modem_cmd.h"
#include "sh366102.h"
#include "ril_interface.h"
#include "paqichong_debug_info.h"

#define PQC_TASK_SEND_STACK_SIZE 0x2000
#define PQC_TASK_RECV_STACK_SIZE 0x2000
#define PQC_WEB_TASK_STACK_PRIORITY 17
#define MAX_PQC_WEB_QUEUE_NUM 10

unsigned int connect_cnt = 0;
extern bool get_sleep_flag(void);
static int paqichong_clientFd = -1;
static int close_clientfd[4] = {-1, -1, -1, -1};
bool closing = 0;
bool ws_connected = 0;
pthread_mutex_t g_client_mutex = PTHREAD_MUTEX_INITIALIZER;

osThreadId_t pqc_send_task_id;
unsigned long pqc_send_msgqueue_id;

osThreadId_t pqc_recv_task_id;
unsigned long pqc_recv_msgqueue_id;

osal_timer timer_heartbeat_ref;
osal_timer timer_reconnect_ref;
osal_timer timer_disconnect_ref;
osal_timer timer_charge_status_ref;

osal_semaphore sem_heartbeat;

static uint8_t ws_connect_index = 0;
static uint32_t ws_connect_times[7] = {WS_CONNECT_1S_TIME, WS_CONNECT_3S_TIME, WS_CONNECT_5S_TIME, WS_CONNECT_10S_TIME, WS_CONNECT_10S_TIME, WS_CONNECT_10S_TIME, WS_CONNECT_1M_TIME};

static void ws_charge_status_timer_handler(unsigned long data)
{
    /* 处在充电状态才会去上报 */
    if (g_chargestatus == 1)
    {
        ws_report_chg_status(g_chargestatus);
    }
    osal_timer_mod(&timer_charge_status_ref, 2 * 60 * 1000);
}

static void ws_disconnect_timer_handler(void)
{
    int32_t ret = 0, i;

    WS_PRINT("ws_disconnect_timer_handler start\n");
    closing = 1;
    for (i = 0; close_clientfd[i] != -1 && i < 4; i++)
    {
        WS_PRINT("close client[%d]=%d\n", i, close_clientfd[i]);
        ret = WebSocketClientClose(close_clientfd[i]);
        if (ret != 0)
        {
            WS_PRINT("close client[%d]=%d fail, ret=0x%x\n", i, close_clientfd[i], ret);
        }
        else
        {
            paqichong_save_log("close client successful");
            WS_PRINT("close client[%d]=%d successful\n", i, close_clientfd[i]);
            close_clientfd[i] = -1;
        }
    }
    closing = 0;
    WS_PRINT("ws_disconnect_timer_handler end\n");
}

static void ws_reconnect_timer_handler(unsigned long data)
{
    (void)data;
    WS_PRINT("ws_reconnect_timer_handler start\n");
    paqichong_save_log("ws_reconnect_timer_handler start");
    paqichong_send_cmd(PQC_WEB_EVENT_CONNECT);
}

static void ws_heartbeat_timer_handler(unsigned long data)
{
    (void)data;
    WS_PRINT("ws_heartbeat_timer_handler start\n");

    /* 不在寻宠模式 */
    if (location_flag == 0 && get_sleep_flag() == 0)
    {
        osal_timer_mod(&wifi_bt_timer, WS_HEATBEAT_TIME - 1000);
    }
    ws_report_regular_time_action();
    osal_timer_mod(&timer_heartbeat_ref, WS_HEATBEAT_TIME);
}

/* 处理服务器端发送消息 */
static void pqc_recv_msg_entry(void *data)
{
    int32_t ret = 0;
    pqc_web_event_t msg = {0};
    unsigned int msg_sz = sizeof(pqc_web_event_t);
    (void)data;

    while (1)
    {
        ret = osal_msg_queue_read_copy(pqc_recv_msgqueue_id, (void *)&msg, &msg_sz, OSAL_MSGQ_WAIT_FOREVER);
        if (ret == OSAL_FAILURE)
        {
            WS_PRINT("osal msg queue read copy failed\n");
            continue;
        }
        /* 解析消息 */
        unpack_data(msg.data);

        if (msg.data)
            free(msg.data);
    }
}

/* 接收来自服务器消息的回调 */
static void OnMessage(int client, const char *buf, size_t length, bool isFinished, bool isBinary)
{
    WS_PRINT("client(%d), Message = %s, length=%zu, isfinish=%d, isBinary=%d\n", client, buf, length, isFinished, isBinary);
    int32_t ret = 0;
    pqc_web_event_t msg = {0};

    if (buf && (length > 0))
    {
        msg.length = length;
        msg.data = calloc(length, sizeof(uint8_t));
        memcpy(msg.data, buf, length);
        ret = osal_msg_queue_write_copy(pqc_recv_msgqueue_id, (void *)&msg, sizeof(pqc_web_event_t), OSAL_MSGQ_WAIT_FOREVER);
        if (ret == OSAL_FAILURE)
        {
            if (msg.data)
                free(msg.data);
        }
    }
}

static void OnError(int client, struct WebSocketErrorResult error)
{
    WS_PRINT("OnError,client=%d, errcode=%d, messg=%s\n", client, error.errorCode, error.errorMessage);
    if (error.errorCode < 0)
    {
        paqichong_clientFd = -1;
        /* 暂停定时上报 */
        osal_timer_stop(&timer_heartbeat_ref);
        /* 关闭充电状态上报 */
        osal_timer_stop(&timer_charge_status_ref);

        close_clientfd[client] = client;
        /* 如果已经处在关闭阶段，就不用执行定时 */
        if (!closing)
        {
            paqichong_send_cmd(PQC_WEB_EVENT_CLOSECLIENT);
        }
        osal_msleep(100);
        /* 发送重连 */
        paqichong_send_cmd(PQC_WEB_EVENT_RECONNECT);
    }
}

static void OnClose(int client)
{
    WS_PRINT("OnClose,client=%d, OnClose callback\n", client);
}

/* 回调函数,连接成功会执行，打印client的编号 */
static void OnOpen(int client)
{
    WS_PRINT("OnOpen, client=%d\n", client);
    ws_connect_index = 0;
    ws_connected = 1;
    connect_cnt++;
    /* 上报版本号 */
    ws_report_model_version();
    /* 执行WiFi SCAN*/
    if (location_flag == 0)
    {
        osal_timer_mod(&wifi_bt_timer, 3 * 1000);
    }
    /* 开始充电状态 */
    osal_timer_mod(&timer_charge_status_ref, 10 * 1000);
    /* 开启定时上报 */
    osal_timer_mod(&timer_heartbeat_ref, 20 * 1000);

}

int ws_sentdata(const char *data_to_send, size_t length)
{
    if (paqichong_clientFd < 0)
    {
        WS_PRINT("No active connection!\n");
        return -1;
    }
    if (ws_connected == 0)
    {
        WS_PRINT("websocket not connect\n");
        return -1;
    }
    WS_PRINT("ws data = %s\n", data_to_send);
    // 实际发送数据,data_to_send的字符长度不包括\0，一定要跟length匹配，不然会返回-11
    int ret = WebSocketClientSendText(paqichong_clientFd, data_to_send, length);
    if (ret != 0)
    {
        if (ret == WEBSOCKET_NO_CONNECTION)
        {
            close_clientfd[paqichong_clientFd] = paqichong_clientFd;
            paqichong_clientFd = -1;
            ws_connected = 0;
            WS_PRINT("Perform connect\n");
            if (!closing)
            {
                paqichong_send_cmd(PQC_WEB_EVENT_CLOSECLIENT);
            }
            osal_msleep(100);
            paqichong_send_cmd(PQC_WEB_EVENT_RECONNECT);
        }
        WS_PRINT("Send failed, ret=%d\n", ret);
        return ret;
    }
    WS_PRINT("Data sent successfully!\n");
#if 0
    osDelay(1000);
    if(!get_download_file_state())
    {
        if(location_flag)
        {
            RRC_release();
        }
    }
#endif
    return ret;
}
static char modem_sleep_check_cnt = 0;
int ws_sentheartbeat(const char *data_to_send, size_t length)
{
    if (paqichong_clientFd < 0)
    {
        WS_PRINT("No active connection!\n");
        return -1;
    }
    if (ws_connected == 0)
    {
        WS_PRINT("hearbeat websocket not connect\n");
        return -1;
    }
    WS_PRINT("ws heartbeat = %s\n", data_to_send);
    // 实际发送数据,data_to_send的字符长度不包括\0，一定要跟length匹配，不然会返回-11
    int ret = WebSocketClientSendText(paqichong_clientFd, data_to_send, length);
    WS_PRINT("ws WebSocketClientSendText ret = %d\n", ret);
    if (OSAL_ETIME == osal_sem_down_timeout(&sem_heartbeat, 8*1000))
    {
        WS_PRINT("Send heartbeat timeout\n");
        paqichong_save_log("Send heartbeat timeout");
        close_clientfd[paqichong_clientFd] = paqichong_clientFd;
        paqichong_clientFd = -1;
        ws_connected = 0;
        if (!closing)
        {
            paqichong_send_cmd(PQC_WEB_EVENT_CLOSECLIENT);
        }
        osal_msleep(100);
        WS_PRINT("perform reconnect\n");
        paqichong_send_cmd(PQC_WEB_EVENT_RECONNECT);
        return ret;
    }
    WS_PRINT("RRC_release before!\n");
    osDelay(1000);
    if(!get_download_file_state())
    {
        RRC_release();
    }
    if(modem_sleep_check_cnt >= 12)
    {
        modem_sleep_check();
        modem_sleep_check_cnt = 0;
    }
    else
    {
        modem_sleep_check_cnt++;
    }
    WS_PRINT("RRC_release end!\n");
    WS_PRINT("Data sent successfully!\n");
    return ret;
}

int ws_createclient(void)
{
    char url_buf[256]= "ws://test.t553api.qialg.com/deviceWebsocket?code=11:11:11:11:11:11";
    char id[8] = {0};

    watch_storage_get(STORAGE_HARD_CODE, id, sizeof(id));
    if (id[0] != 0)
    {
        memset(url_buf, 0, 256);
        snprintf(url_buf, 256, "ws://test.t553api.qialg.com/deviceWebsocket?code=%s", id);
    }
    WS_PRINT("url = %s\n", url_buf);
    paqichong_clientFd = WebSocketClientConstructor(OnOpen, OnMessage, OnError, OnClose);
    if (paqichong_clientFd < 0)
    {
        WS_PRINT("Client fd fail %d\n", paqichong_clientFd);
        paqichong_clientFd = -1;
        ws_connected = 0;
        osal_msleep(100);
        paqichong_send_cmd(PQC_WEB_EVENT_RECONNECT);
        return -1;
    }
    WS_PRINT("Perform WebSocketClientConnect=%d\n", paqichong_clientFd);
    int ret = WebSocketClientConnect(paqichong_clientFd, url_buf, NULL);
    /* 断网情况下，connect不成功，ret也会返回0 */
    if (ret != 0)
    {
        WS_PRINT("Connect failed, ret = %d\n", ret);
        paqichong_clientFd = -1;
        ws_connected = 0;
        return -1;
    }
    WS_PRINT("Perform WebSocketClientConnect end\n");
    return 0;
}

int ws_con_discon(void)
{
    int ret;

    if (paqichong_clientFd < 0)
    {
        WS_PRINT("No active connection!\n");
        return -1;
    }

    ret = WebSocketClientClose(paqichong_clientFd);
    if (ret != 0)
    {
        WS_PRINT("close fail, ret=0x%x\n", ret);
    }
    else
    {
        /* 关闭定时上报 */
        osal_timer_stop(&timer_heartbeat_ref);
        osal_timer_stop(&timer_charge_status_ref);
        WS_PRINT("close success!");
    }
    return 0;
}

static void pqc_send_task_entry(void *data)
{
    (void)data;
    pqc_web_event_t event;
    int32_t data_len = 0;
    int32_t ret = 0;

    while (1)
    {
        data_len = sizeof(pqc_web_event_t);
        ret = osal_msg_queue_read_copy(pqc_send_msgqueue_id, (void *)&event, &data_len, OSAL_MSGQ_WAIT_FOREVER);
        if (ret == OSAL_FAILURE)
        {
            WS_PRINT("pqc_send_task_entry osal_msg_queue_read_copy failed\n");
            continue;
        }
        switch (event.event_type)
        {
        case PQC_WEB_EVENT_CONNECT:
            if (paqichong_clientFd < 0)
            {
                ws_createclient();
            }
            else
            {
                WS_PRINT("client has been connected\n");
            }
            break;
        case PQC_WEB_EVENT_DISCONNECT:
            if (paqichong_clientFd >= 0)
            {
                ws_con_discon();
                paqichong_clientFd = -1;
                ws_connected = 0;
            }
            uapi_system_shutdown(0);
            break;
        case PQC_WEB_EVENT_SEND:
            if (event.data != NULL)
            {
                ws_sentdata(event.data, event.length);
                free(event.data);
            }
            break;
        case PQC_WEB_EVENT_RECONNECT:
        {
            WS_PRINT("ws conect index = %d\n", ws_connect_index);
            soc_ue_cell_radio_info signal_info = {0};
            ril_get_radio_signal_info(&signal_info);
            paqichong_save_log("ws conect index = %d, rsrp = %d\n", ws_connect_index, signal_info.rsrp);
            if (ws_connect_index > 60)
            {
                /* 60次之后还未连接上，大概20多分钟执行reset重启 */
                paqichong_save_log("system reboot, rsrp =%d", signal_info.rsrp);
                uapi_system_reboot(SYSTEM_SOFT_REBOOT);
                break;
            }
            if (signal_info.rsrp != 0 && signal_info.rsrp < -1200)
            {
                osal_timer_mod(&timer_reconnect_ref, ws_connect_times[6]);
            }
            else
            {
                if (ws_connect_index >= 5)
                {
                    osal_timer_mod(&timer_reconnect_ref, ws_connect_times[5]);
                }
                else
                {
                    osal_timer_mod(&timer_reconnect_ref, ws_connect_times[ws_connect_index]);
                }
            }
            ws_connect_index++;
        }
            break;
        case PQC_WEB_HEATBEAT_SEND:
            if (event.data != NULL)
            {
                ws_sentheartbeat(event.data, event.length);
                free(event.data);
            }
            break;
        case PQC_WEB_EVENT_CLOSECLIENT:
            ws_disconnect_timer_handler();
            break;
        default:
            break;
        }
    }
}

int ws_task_init(void)
{
    int ret = 0;
    osThreadAttr_t threadAttr_pqc = {0};
    static const char pqc_send_task_name[] = "thread_pqc_send_task";
    static const char pqc_send_queue_name[] = "thread_pqc_send_queue";

    static const char pqc_recv_task_name[] = "thread_pqc_recv_task";
    static const char pqc_recv_queue_name[] = "thread_pqc_recv_queue";

    WS_PRINT("ws_task_init start\n");

    ret = osal_msg_queue_create(pqc_send_task_name, MAX_PQC_WEB_QUEUE_NUM, &pqc_send_msgqueue_id, 0, sizeof(pqc_web_event_t));
    if (ret != 0)
    {
        WS_PRINT("Create msgqueue %s failed: %d\n", pqc_send_task_name, ret);
        goto EXIT0;
    }

    ret = osal_msg_queue_create(pqc_recv_task_name, MAX_PQC_WEB_QUEUE_NUM, &pqc_recv_msgqueue_id, 0, sizeof(pqc_web_event_t));
    if (ret != 0)
    {
        WS_PRINT("Create msgqueue %s failed: %d\n", pqc_recv_task_name, ret);
        goto EXIT1;
    }

    threadAttr_pqc.name = pqc_send_queue_name;
    threadAttr_pqc.stack_size = PQC_TASK_SEND_STACK_SIZE;
    threadAttr_pqc.priority = (osPriority_t)PQC_WEB_TASK_STACK_PRIORITY;
    threadAttr_pqc.stack_mem = memalign(16, threadAttr_pqc.stack_size);

    pqc_send_task_id = osThreadNew(pqc_send_task_entry, NULL, &threadAttr_pqc);
    if (pqc_send_task_id == NULL)
    {
        WS_PRINT("pqc send task create failed\n");
        ret = -1;
        goto EXIT2;
    }

    memset(&threadAttr_pqc, 0, sizeof(osThreadAttr_t));
    threadAttr_pqc.name = pqc_recv_queue_name;
    threadAttr_pqc.stack_size = PQC_TASK_RECV_STACK_SIZE;
    threadAttr_pqc.priority = (osPriority_t)PQC_WEB_TASK_STACK_PRIORITY;
    threadAttr_pqc.stack_mem = memalign(16, threadAttr_pqc.stack_size);

    pqc_recv_task_id = osThreadNew(pqc_recv_msg_entry, NULL, &threadAttr_pqc);
    if (pqc_recv_task_id == NULL)
    {
        WS_PRINT("pqc recv task create failed\n");
        ret = -1;
        goto EXIT3;
    }

    /* 添加定时任务 */
    timer_heartbeat_ref.handler = ws_heartbeat_timer_handler;
    timer_heartbeat_ref.interval = 2.5 * 60 * 1000;
    ret = osal_timer_init(&timer_heartbeat_ref);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("create heart beat timer fail, ret %d", ret);
        goto EXIT4;
    }

    /* 添加重连定时 */
    timer_reconnect_ref.handler = ws_reconnect_timer_handler;
    timer_reconnect_ref.interval = 5 * 60 * 1000;
    ret = osal_timer_init(&timer_reconnect_ref);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("create reconnect timer fail, ret %d", ret);
        goto EXIT5;
    }

    /* 添加一个充电情况状态上报 */
    timer_charge_status_ref.handler = ws_charge_status_timer_handler;
    timer_charge_status_ref.interval = 2 * 60 * 1000;
    ret = osal_timer_init(&timer_charge_status_ref);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("create charge status fail, ret %d", ret);
        goto EXIT7;
    }

    ret = osal_sem_init(&sem_heartbeat, 0);
    if (ret == OSAL_FAILURE) {
        WS_PRINT("create heartbeat sema Fail");
        goto EXIT8;
    }
    return ret;
EXIT8:
    osal_timer_destroy(&timer_charge_status_ref);
EXIT7:
EXIT6:
    osal_timer_destroy(&timer_reconnect_ref);
EXIT5:
    osal_timer_destroy(&timer_heartbeat_ref);
EXIT4:
    osThreadTerminate(pqc_recv_task_id);
EXIT3:
    osThreadTerminate(pqc_send_task_id);
EXIT2:
    osal_msg_queue_delete(pqc_recv_msgqueue_id);
EXIT1:
    osal_msg_queue_delete(pqc_send_msgqueue_id);
EXIT0:
    return ret;
}

/* 用于发送打开关闭websocet */
int32_t paqichong_send_cmd(uint32_t event_id)
{
    int32_t ret = 0;
    pqc_web_event_t event = {0};

    event.event_type = event_id;
    ret = osal_msg_queue_write_copy(pqc_send_msgqueue_id, (void *)&event, sizeof(pqc_web_event_t), OSAL_MSGQ_WAIT_FOREVER);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("send event[%u] failed\n", event_id);
    }
    return ret;
}

/* 用于给服务器发送请求，write_copy使用非阻塞形式  */
int32_t paqichong_send_data_to_server_nonblocking(void *data, size_t length)
{
    int32_t ret = 0;
    pqc_web_event_t event = {0};

    if (!data || length == 0)
    {
        WS_PRINT("data or length invalid\n");
        return -1;
    }
    WS_PRINT("data=%p,length =%d\n", data, length);
    event.data = calloc(length + 1, sizeof(uint8_t));
    if (!event.data)
    {
        WS_PRINT("calloc failed\n");
        return -1;
    }
    memcpy(event.data, data, length + 1);
    event.length = length;
    event.event_type = PQC_WEB_EVENT_SEND;

    ret = osal_msg_queue_write_copy(pqc_send_msgqueue_id, (void *)&event, sizeof(pqc_web_event_t), OSAL_MSGQ_NO_WAIT);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("send event[%u] failed\n", PQC_WEB_EVENT_SEND);
        if (event.data)
            free(event.data);
    }
    return ret;
}

/* 用于给服务器发送请求，write_copy使用阻塞形式 */
int32_t paqichong_send_data_to_server(void *data, size_t length)
{
    int32_t ret = 0;
    pqc_web_event_t event = {0};

    if (!data || length == 0)
    {
        WS_PRINT("data or length invalid\n");
        return -1;
    }
    WS_PRINT("data=%p,length =%d\n", data, length);
    event.data = calloc(length + 1, sizeof(uint8_t));
    if (!event.data)
    {
        WS_PRINT("calloc failed\n");
        return -1;
    }
    memcpy(event.data, data, length + 1);
    event.length = length;
    event.event_type = PQC_WEB_EVENT_SEND;

    ret = osal_msg_queue_write_copy(pqc_send_msgqueue_id, (void *)&event, sizeof(pqc_web_event_t), OSAL_MSGQ_WAIT_FOREVER);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("send event[%u] failed\n", PQC_WEB_EVENT_SEND);
        if (event.data)
            free(event.data);
    }
    return ret;
}

/* 用于给服务器发送HEATBEAT请求 */
int32_t paqichong_send_heatbeat_to_server(void *data, size_t length)
{
    int32_t ret = 0;
    pqc_web_event_t event = {0};

    if (!data || length == 0)
    {
        WS_PRINT("data or length invalid\n");
        return -1;
    }
    WS_PRINT("heatbeat data=%s,length =%d\n", data, length);
    event.data = calloc(length + 1, sizeof(uint8_t));
    if (!event.data)
    {
        WS_PRINT("calloc failed\n");
        return -1;
    }
    memcpy(event.data, data, length + 1);
    event.length = length;
    event.event_type = PQC_WEB_HEATBEAT_SEND;

    ret = osal_msg_queue_write_copy(pqc_send_msgqueue_id, (void *)&event, sizeof(pqc_web_event_t), OSAL_MSGQ_WAIT_FOREVER);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("send event[%u] failed\n", PQC_WEB_HEATBEAT_SEND);
        if (event.data)
            free(event.data);
    }
    return ret;
}

void websocket_response_heart(void)
{
    osal_sem_up(&sem_heartbeat);
}

/* 简单测试接口 */
void paqichong_net_test(void)
{
    ws_report_real_time_action(1, 200);
}
