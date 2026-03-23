#ifndef LOCATION_SERVICE_H
#define LOCATION_SERVICE_H

#include "cmsis_os2.h"
#include "osal_timer.h"
#include "soc_osal.h"
#include "debug_print.h"
#include <string.h>
#include "app_at_process.h"
#include <pthread.h>

#define LOCATION_MSGQ_NAME "location_msgq"
#define LOCATION_MSGQ_SIZE 32
#define LOCATION_TASK_NAME "location_msg_task"
#define LOCATION_STACK_SIZE 4096
#define LOCATION_TASK_PRIORITY 18

#pragma pack(1)
typedef struct location_context
{
    uint8_t isInit;
    uint8_t gnss_inited;
    osThreadId_t location_task_ref;
    osThreadId_t search_task_ref;
    unsigned long location_queue_id;
    osal_timer timer_location;
} location_context_t;

typedef struct location_msg_item
{
    uint32_t msg_id;
    uint32_t length;
    void *data;
} location_msg_item_t;

typedef struct {
    bool valid;
    uint16_t pci;
    uint32_t earfcn;
    int16_t rsrp;
    int16_t rsrq;
    int16_t rssi;
    int16_t snr;
    int16_t srxlev;
} primary_cell_info_t;

typedef struct {
    bool valid;
    uint16_t pci;
    uint32_t earfcn;
    int16_t rsrp;
    int16_t rsrq;
    int16_t rssi;
    int16_t snr;
    int16_t srxlev;
} neighbor_cell_info_t;

#pragma pack()

enum
{
    MSG_LOCATION_START = 0,
    MSG_CELL_GET_DONE,
    MSG_WIFI_SCAN_DONE,
    MSG_GNSS_SCAN_DONE,
};

//#define LOCATION_SERVICE_PRINT(fmt, ...) PRINT("[LOCATION] " fmt, ##__VA_ARGS__)

#define LOCATION_SERVICE_PRINT(s, ...)       factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)
#define WIFI_BUF_SIZE  1024

extern int location_task_init(void);
extern int32_t location_send_msg(uint32_t msg_id, uint32_t length, void *data);
extern void location_start(uint8_t mode);
extern void searching_stop(void);
extern void get_wifilist(void);

extern char signal_info_buf[64];

extern bool getting_cell;
extern char bts_info_buf[128];
//extern char nearbts_info_buf[128];
extern pthread_mutex_t bts_buf_mutex;

extern bool wifi_scan_done;
extern char wifi_info_buf[WIFI_BUF_SIZE];
extern char wifi_serach_buf[WIFI_BUF_SIZE];
extern char wifi_info_temp[WIFI_BUF_SIZE];
extern pthread_mutex_t wifi_info_buf_mutex;

extern char gnss_info_buf[64];
extern pthread_mutex_t gnss_info_buf_mutex;

extern osal_timer wifi_bt_timer;
#endif