#ifndef _PAQICHONG_DATA_PROCESS_H_
#define _PAQICHONG_DATA_PROCESS_H_
#include "stdlib.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "osal_semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif
#define PAQICHONG_ENABLE_DEBUG 0
#if  PAQICHONG_ENABLE_DEBUG
#define PAQICHONG_PRINT(fmt, ...)      printf("[PAQICHONG] " fmt, ##__VA_ARGS__)
#else
#define PAQICHONG_PRINT(fmt, ...)
#endif

extern uint32_t model_bin_version;
extern float paqichong_firmware_version;
#define PAQICHONG_TEST_TIME 90



int paqichong_tack_init(void);
typedef struct {
    osThreadId_t paqichong_event_task_id;
    int32_t g_paqichong_event_queue;
    struct osal_list_head head;
    osal_semaphore paqichong_sem;
} paqichong_module_t;

typedef struct {
    uint64_t timestamp;
    uint32_t count;
    int32_t predicted_class;
    int16_t sensor_data[6]; // 存储六轴数据: [ax, ay, az, gx, gy, gz]
} prediction_record_t;

typedef enum {
    PAQICHONG_CLIENT_EVENT_NONE             = 0,
    PAQICHONG_START_SEND_REAL_DATA          = 1,
    PAQICHONG_STOP_SEND_REAL_DATA           = 2,
    PAQICHONG_START_SAVE_IMU_DATA           = 3,
    PAQICHONG_STOP_SAVE_IMU_DATA            = 4,
    PAQICHONG_START_SEND_REGULAR_DATA       = 5,
    PAQICHONG_START_MEASURE_TIMER           = 6,
    PAQICHONG_STOP_MEASURE_TIMER            = 7,
    PAQICHONG_CLIENT_EVENT_MAX,
} paqichong_event_type;

typedef struct {
    uint16_t year;   // YYYY
    uint8_t month;   // MM, 1~12
    uint8_t day;     // DD, 1~31
    uint8_t hour;    // hh, 0~23
    uint8_t minute;  // mm, 0~59
    uint8_t second;  // ss, 0~59
    uint16_t ms;     // ms, 0~999
    uint8_t valid;   // 0: invalid, 1: valid
} PaqichongTime;

typedef struct {
    paqichong_event_type event_type;
} paqichong_event_mail_t;

typedef enum{
	DISCONNECTED,
	CONNECTED
}net_status_t;
typedef struct {
    uint32_t cnt;
    uint64_t timestamp;
    int16_t acc_data[3];
    int16_t gyro_data[3];
} imu_data_t;
extern int32_t paqichong_event_write(paqichong_event_type event_type,uint32_t event_data);
extern uint64_t get_utc_time(void);
int paqichong_model_reload(void);
void set_reload_model_flag(void);
void paqichong_measure_timer_handler(void);
uint32_t get_paqichong_pred_cnt(void);
#ifdef __cplusplus
}
#endif
#endif
