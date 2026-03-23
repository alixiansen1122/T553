#ifndef __PAQICHONG_WEB_SERVICE_H__
#define __PAQICHONG_WEB_SERVICE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>
#include "app_at_process.h"

//#define WS_PRINT(fmt, ...) printf("[WS] " fmt, ##__VA_ARGS__)
#define WS_PRINT(s, ...)       factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)

typedef enum {
    PQC_WEB_EVENT_CONNECT,
    PQC_WEB_EVENT_SEND,
    PQC_WEB_EVENT_RECONNECT,
    PQC_WEB_EVENT_DISCONNECT,
    PQC_WEB_HEATBEAT_SEND,
    PQC_WEB_EVENT_CLOSECLIENT,
} pqc_web_event_type_t;

typedef struct {
    pqc_web_event_type_t event_type;  // 事件类型
    void *data;                       // 事件数据指针
    size_t length;                    // 数据长度
} pqc_web_event_t;

//now time(ms)
#define WS_HEATBEAT_TIME            (5*60*1000)
#define WS_CONNECT_1S_TIME          (1*1000)
#define WS_CONNECT_3S_TIME          (3*1000)
#define WS_CONNECT_5S_TIME          (5*1000)
#define WS_CONNECT_10S_TIME         (10*1000)
#define WS_CONNECT_1M_TIME          (60*1000)
#define WS_CONNECT_2M_TIME          (120*1000)
#define WS_CONNECT_3M_TIME          (180*1000)
#define WS_CONNECT_5M_TIME          (300*1000)
#define WS_CONNECT_8M_TIME          (480*1000)
#define WS_CONNECT_10M_TIME         (600*1000)

void paqichong_net_test(void);
int ws_task_init(void);

int32_t paqichong_send_cmd(uint32_t event_id);
int32_t paqichong_send_data_to_server(void *data, size_t length);
int32_t paqichong_send_data_to_server_nonblocking(void *data, size_t length);
int32_t paqichong_send_heatbeat_to_server(void *data, size_t length);
int32_t start_connect_paqichong(void);
void websocket_response_heart(void);

extern char version_2131[64];
#endif // WEBSOCKET_CLIENT_H