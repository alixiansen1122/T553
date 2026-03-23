#ifndef GNSS_SERVICE_H
#define GNSS_SERVICE_H
#include "cmsis_os2.h"
#include "osal_timer.h"
#include "soc_osal.h"
#include "debug_print.h"
#include "gnss_custom_api.h"
#include "app_at_process.h"
#include "gnss_agnss.h"
#include "http_api.h"
#include <pthread.h>

#define GNSS_MSGQ_NAME "gnss_msgq"
#define GNSS_MSGQ_SIZE 32
#define GNSS_TASK_NAME "gnss_msg_task"
#define GNSS_STACK_SIZE 4 * 1024
#define GNSS_TASK_PRIORITY 18

#define GNSS_TEST_MSGQ_NAME "gnss_test_msgq"
#define GNSS_TEST_MSGQ_SIZE 10
#define GNSS_TEST_TASK_NAME "gnss_test_msg_task"
#define GNSS_TEST_STACK_SIZE 2 * 1024

typedef struct gnss_context
{
    uint8_t isInit;
    uint8_t gnss_inited;
    osThreadId_t gnss_task_ref;
    unsigned long gnss_queue_id;
    osal_timer timer_agps;
} gnss_context_t;

#pragma pack(1)
typedef struct gnss_msg_item
{
    uint32_t msg_id;
    uint32_t length;
    void *data;
} gnss_msg_item_t;
#pragma pack()

enum
{
    MSG_GNSS_INIT = 0,
    MSG_GNSS_START,
    MSG_GNSS_STOP,
    MSG_GNSS_DEINIT,
};

enum
{
    MSG_GNSS_COLD_TEST = 1,
    MSG_GNSS_HOT_TEST,
    MSG_GNSS_STOP_TEST,
    MSG_GNSS_END_TEST,
    MSG_GNSS_CNR_TEST_START,
    MSG_GNSS_CNR_TEST_END,
};

#define GNSS_SERVICE_PRINT(s, ...)       factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)

extern int32_t gnss_task_init(void);
extern int32_t gnss_send_msg(uint32_t msg_id, uint32_t length, void *data);
void gnss_device_check_start(void);
void gnss_device_check_end(void);
void gnss_check_init(void);
void gnss_mode_test_init(void);
int32_t gnss_test_send_msg(uint32_t msg_id, uint32_t length, void *data);
extern int gnss_test_mode;
extern bool gnss_factory_flag;
#endif