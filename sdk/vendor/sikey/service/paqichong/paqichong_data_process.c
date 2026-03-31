#include "thread_init.h"
#include "tcxo.h"
#include "log_common.h"
#include "log_def.h"
#include "log_uart.h"
#ifdef __LITEOS__
#include "los_status.h"
#endif
#include "soc_osal.h"
#include "calendar.h"
#include "osal_semaphore.h"
#include "osal_list.h"
#include "soc_osal.h"
#include <time.h>
#include <sys/time.h>
#include <float.h>
#include "NN.h"
#include <stdint.h>
#include <string.h>
#include "basic_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "paqichong_data_process.h"
#include "net_data_process.h"
#include "modem_ctrl.h"
#include "http_api.h"
#include "modem_cmd.h"
#include "rtc.h"
#include "timer.h"
#include "dev_storage.h"
#include "lsm6dsow_api.h"
// Adjust these according to your model's actual dimensions
#define NUM_LAYERS 1
#define INPUT_DIM 6
#define HIDDEN_DIM 42
#define OUTPUT_DIM 10
#define NUM_PARAM_FILES 18
#define MIN_SEQ 15
#define MAX_SEQ 20
#define TIME_SYNC_INTERVAL 1000*300

/* two timer  for collect imu data and predict gesture*/

#define IMU_COLLECT_TIME 100
#define IMU_BUFFER_SIZE 1000
osal_timer paqichong_measure_timer={0};
osal_timer imu_collect_timer={0};
osal_timer sync_time_timer={0};
timer_handle_t g_imu_timer = {0};
static FILE* log_file = NULL;
static bool imu_data_collect_flag = false;
static bool imu_stop_record_flag = false;
static bool imu_data_has_upload = true;
static uint32_t imu_record_time = 0;

static osal_mutex sensor_data_mutex;
extern uint8_t g_facflag ;
const char* merged_file = "user/paqichong/model.bin";
const char* imu_data_file = "user/paqichong/imu_data.csv";

/* task for process predicting gesture */
osal_task *paqichong_task_id;
osal_semaphore paqichong_sem;
void* model;
NeuralNetworkInferenceContainer* container = NULL;
static int32_t pre_pred = -1;

#define HISTORY_WINDOW_SIZE 200
prediction_record_t prediction_ring_buffer[HISTORY_WINDOW_SIZE];
uint32_t ring_buffer_index = 0;
uint32_t total_records = 0;

/* task for process imu data collection */
osal_task *paqichong_event_handler_id;
static paqichong_module_t *g_paqichong_module = NULL;

uint32_t model_bin_version = 0;
float paqichong_firmware_version = 0.0;

uint32_t first_seqlength = 20;
static bool first_prediction = true;
static uint32_t cnt = 0;
static uint32_t s_last_reset_day  = 0;
static bool  time_past_day_flag = false;
static char need_reload_model_flag = 0;

/* get net status*/
net_status_t net_conneted_status = CONNECTED;

net_status_t get_net_status(void) {
    return CONNECTED;
}
uint32_t get_paqichong_pred_cnt(void) {
    return cnt;
}
void check_and_reset_cnt_at_midnight(void)
{
    uint64_t now = get_utc_time();
    time_t time_sec = (time_t)now + g_tz_hours * 3600;
    struct tm *time_info = gmtime(&time_sec);
    if (time_info != NULL) {
        uint32_t current_day = time_info->tm_yday;
        // 检查是否为0点
        if(current_day != s_last_reset_day){
            cnt = 0;
            s_last_reset_day = current_day;
            PAQICHONG_PRINT("Midnight reset: cnt reset to 0\n");
        }
    }
}
void calculate_cnt_from_midnight_seconds(void)
{
    uint64_t now = get_utc_time();
    time_t time_sec = (time_t)now + g_tz_hours * 3600;;
    struct tm *time_info = gmtime(&time_sec);

    if (time_info != NULL) {
        PAQICHONG_PRINT("UTC time: %04d-%02d-%02d %02d:%02d:%02d\n",
               time_info->tm_year + 1900,
               time_info->tm_mon + 1,
               time_info->tm_mday,
               time_info->tm_hour,
               time_info->tm_min,
               time_info->tm_sec);
        s_last_reset_day = time_info->tm_yday;
        }
    uint32_t seconds_from_midnight = time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec;

    cnt = seconds_from_midnight * 10;
    PAQICHONG_PRINT("cnt from midnight: %d\n", cnt);


}
void paqichong_sync_time_handler(void)
{
    sync_time_from_modem();
    // osal_timer_mod(&sync_time_timer);
    calculate_cnt_from_midnight_seconds();
    osal_timer_mod(&sync_time_timer,TIME_SYNC_INTERVAL);
}
void paqichong_measure_timer_handler(void)
{
        uapi_rtc_start(g_imu_timer, PAQICHONG_TEST_TIME, paqichong_measure_timer_handler, 0);
        osal_sem_up(&paqichong_sem);
}

void paqichong_start_measure_timer_handler(void)
{
    int ret = 0;
    ret = uapi_rtc_start(g_imu_timer, PAQICHONG_TEST_TIME, paqichong_measure_timer_handler, 0);
    if(ret != 0)
    {
        printf("paqichong start measure timer failed %p\n",ret);
    }
}

 uint64_t get_utc_time(void)
{

    struct timeval tv={0};
    struct tm tm={0};
    struct timezone tz={0};
    gettimeofday(&tv, &tz);

    localtime_r(&tv.tv_sec, &tm);
    PAQICHONG_PRINT("paqichong get time  sencond is %llu\n",(unsigned long long)tv.tv_sec);


    return tv.tv_sec;                   // 设置为有效
}


void save_prediction_label(int64_t pred_class, uint32_t count, int16_t* acc_sensor_data,int16_t* gyro_sensor_data) {
    uint32_t index = ring_buffer_index % HISTORY_WINDOW_SIZE;

    prediction_ring_buffer[index].timestamp = time(NULL);
    prediction_ring_buffer[index].count = count;
    prediction_ring_buffer[index].predicted_class = pred_class;

    // 保存六轴数据
    for (uint32_t i = 0; i < 6; i++) {
		if(i<3)
        prediction_ring_buffer[index].sensor_data[i] = acc_sensor_data[i];
		else
		prediction_ring_buffer[index].sensor_data[i] = gyro_sensor_data[i-3];
    }

    ring_buffer_index++;
    if (total_records < HISTORY_WINDOW_SIZE) {
        total_records++;
    }

}


void print_all_predictions(uint32_t start_index, uint32_t num_records) {
    PAQICHONG_PRINT("\n=== BATCH PREDICTION RESULTS (%u records) ===\n", num_records);
    for (uint32_t i = 0; i < num_records; i++) {
        uint32_t index = (start_index + i) % HISTORY_WINDOW_SIZE;
        PAQICHONG_PRINT("RECORD[%u]: TS=%llu, CNT=%u, CLASS=%d, DATA=[%d, %d, %d, %d, %d, %d]\n",
               index,
               prediction_ring_buffer[index].timestamp,
               prediction_ring_buffer[index].count,
               prediction_ring_buffer[index].predicted_class,
               prediction_ring_buffer[index].sensor_data[0],
               prediction_ring_buffer[index].sensor_data[1],
               prediction_ring_buffer[index].sensor_data[2],
               prediction_ring_buffer[index].sensor_data[3],
               prediction_ring_buffer[index].sensor_data[4],
               prediction_ring_buffer[index].sensor_data[5]);
    }
    PAQICHONG_PRINT("=== END OF BATCH ===\n\n");
}


void get_prediction_history(void) {
    PAQICHONG_PRINT("=== Prediction History (Total: %u) ===\n", total_records);
    for (uint32_t i = 0; i < total_records; i++) {
        uint32_t index = (ring_buffer_index - total_records + i) % HISTORY_WINDOW_SIZE;
        PAQICHONG_PRINT("Time: %llu, Class: %d\n",
               prediction_ring_buffer[index].timestamp,
               prediction_ring_buffer[index].predicted_class);
    }
}
int64_t paqichong_process_prediction(uint32_t counter,int16_t* acc_sensor_data,int16_t* gyro_sensor_data)
{

    IntermediateOutput_external *output = NULL;
    int64_t pred_result = -1LL;
	Matrix_float* input = matrix_alloc(1, INPUT_DIM); // 1 time step, INPUT_DIM features
	if(input == NULL)
	{
		PAQICHONG_PRINT("Failed to allocate input matrix\n");
        return pred_result;
	}

	for (uint32_t k = 0; k < INPUT_DIM; ++k) {
		if(k<3)
		matrix_set(input, 0, k, acc_sensor_data[k]);
		else
		matrix_set(input, 0, k, gyro_sensor_data[k-3]);
	}
	output = NeuralNetwork_Container_forward(container, input, counter,false,true);
	if (output == NULL){
		PAQICHONG_PRINT("Inference failed!, Sequence Length Insufficient: %d\n", container->counter);
	} else {
		PAQICHONG_PRINT("Predicted class: %lld ,count is %d\n", output->final_output6,counter);
        pred_result = output->final_output6;
        free_intermediate_output(output);
	}
    matrix_free(input);
    return pred_result;
}

static imu_data_t data_buffer[IMU_BUFFER_SIZE];
static uint32_t buffer_index = 0;
static bool header_written = false;

void imu_data_collect_handler_with_params(uint32_t cnt, int16_t *acc_data, int16_t *gyro_data)
{
    if (buffer_index < IMU_BUFFER_SIZE) {
        data_buffer[buffer_index].cnt = cnt;
        data_buffer[buffer_index].timestamp = get_utc_time();
        memcpy(data_buffer[buffer_index].acc_data, acc_data, 3 * sizeof(int16_t));
        memcpy(data_buffer[buffer_index].gyro_data, gyro_data, 3 * sizeof(int16_t));
        buffer_index++;
    }
    else  // if not receive stop save imu info , then auto stop self
    {
        imu_data_collect_flag = false;
        imu_stop_record_flag = true;
    }
    imu_data_has_upload = false;

}

void imu_buffer_write_and_upload(void)
{
    if (buffer_index >= IMU_BUFFER_SIZE || (imu_stop_record_flag && buffer_index > 0)) {
        FILE* log_file = fopen(imu_data_file, header_written ? "a" : "w");
        if (log_file != NULL) {
            if (!header_written) {
                fprintf(log_file, "cnt,timestamp,acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z\n");
                header_written = true;
            }

            for (uint32_t i = 0; i < buffer_index; i++) {
                fprintf(log_file, "%u,%llu,%d,%d,%d,%d,%d,%d\n",
                       data_buffer[i].cnt,
                       (unsigned long long)data_buffer[i].timestamp,
                       data_buffer[i].acc_data[0], data_buffer[i].acc_data[1], data_buffer[i].acc_data[2],
                       data_buffer[i].gyro_data[0], data_buffer[i].gyro_data[1], data_buffer[i].gyro_data[2]);
            }

            fflush(log_file);
            fclose(log_file);
            buffer_index = 0;
        }
    }
    http_upload_file(imu_data_file);
    header_written = false;
    imu_data_has_upload=true;
}

static int  paqichong_task_entry(void *data)
{
	unused(data);
    static int64_t pred_last = -1LL;
    static int64_t pred_now;
    uint8_t net_status;
    int16_t acc_sensor_data[3] = {0};
	int16_t gyro_sensor_data[3] = {0};
    uint16_t sensor_data_num = 0;
    static bool acc_data_ready = true;
    static bool gyro_data_ready = true;
    static bool cnt_has_corrected = false;
	while(1)
	{
		osal_sem_down(&paqichong_sem);
        //uint32_t start_buffer_index = ring_buffer_index;
        if(need_reload_model_flag)
        {
            PAQICHONG_PRINT("need reload model\n");
            paqichong_model_reload();
            need_reload_model_flag = 0;
            continue;
        }
        if(container == NULL)
        {
            PAQICHONG_PRINT("container is null\n");
             continue;

        }


        if(!cnt_has_corrected)
        {
            calculate_cnt_from_midnight_seconds();
            cnt_has_corrected = true;
        }
        //every 10 second check once
        if(cnt%100 == 0)
        {
            check_and_reset_cnt_at_midnight();
        }
        osal_mutex_lock(&sensor_data_mutex);
        gsensor_read_data(acc_sensor_data, gyro_sensor_data);
        cnt = cnt+1;
        if(imu_data_collect_flag)
            imu_data_collect_handler_with_params(cnt, acc_sensor_data, gyro_sensor_data);
        if(imu_stop_record_flag == true && imu_data_has_upload==false)
            {
                imu_buffer_write_and_upload();
                if(get_sleep_flag())
                    uapi_rtc_stop(g_imu_timer);
            }

        pred_now = paqichong_process_prediction(cnt,acc_sensor_data,gyro_sensor_data);
        osal_mutex_unlock(&sensor_data_mutex);
        PAQICHONG_PRINT("pred_now is %lld, pred_last is %lld\n", pred_now, pred_last);
        if (pred_now != pred_last) {
            pred_last = pred_now;
            PAQICHONG_PRINT("Prediction changed: %lld cnt is %d\n", pred_now, cnt);
            ws_report_real_time_action(pred_now,cnt);
        }
        else {
            ws_record_action(pred_now, cnt);
        }


    }

}


static int paqichong_event_handler_entry(void *arg)
{
    int64_t ret = 0;
    paqichong_event_mail_t mail = {0};
    uint32_t msgsize = (uint32_t)sizeof(paqichong_event_mail_t);
    static uint8_t temp_frameno = 0;
    (void)arg;


    while (1) {

        ret = osal_msg_queue_read_copy(g_paqichong_module->g_paqichong_event_queue,
                                     (void *)&mail, &msgsize, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            PAQICHONG_PRINT("osal_msg_queue_read_copy failed\n");
            continue;
        }

        switch (mail.event_type) {
            case PAQICHONG_START_SEND_REAL_DATA:
            case PAQICHONG_STOP_SEND_REAL_DATA:
            case PAQICHONG_START_SEND_REGULAR_DATA:
                break;
            case PAQICHONG_START_MEASURE_TIMER:
                cell_reselection_enable(1);
                uapi_rtc_start(g_imu_timer, PAQICHONG_TEST_TIME, paqichong_measure_timer_handler, 0);
                break;
            case PAQICHONG_STOP_MEASURE_TIMER:
                cell_reselection_enable(0);
                if(imu_data_collect_flag == false)
                uapi_rtc_stop(g_imu_timer);
                break;
            case PAQICHONG_START_SAVE_IMU_DATA:
                 if(get_sleep_flag())
                 uapi_rtc_start(g_imu_timer, PAQICHONG_TEST_TIME, paqichong_measure_timer_handler, 0);
                break;

            default:
                PAQICHONG_PRINT("Unknown event type: %d\n", mail.event_type);
                break;
        }
    }

    return 0;
}

int32_t paqichong_event_write(paqichong_event_type event_type,uint32_t event_data){
    paqichong_event_mail_t mail = {0};
    int32_t ret = -1;
    unused(event_data);
    mail.event_type = event_type;

    PAQICHONG_PRINT("paqichong event_type is %d\n", mail.event_type);
    if(mail.event_type == PAQICHONG_START_SAVE_IMU_DATA)
    {
        imu_data_collect_flag = true;
        imu_stop_record_flag = false;
    }
    else if(mail.event_type == PAQICHONG_STOP_SAVE_IMU_DATA)
    {
        imu_data_collect_flag = false;
        imu_stop_record_flag = true;

    }

    if(osal_msg_queue_write_copy(g_paqichong_module->g_paqichong_event_queue, (void*)&mail, sizeof(paqichong_event_mail_t), 0) != OSAL_SUCCESS) {
        PAQICHONG_PRINT("osal_msg_queue_write_copy failed\n");
        goto exit;
    }
    ret = 0;
    exit:
        return ret;
}


bool imu_logger_init(const char* filename) {

    log_file = fopen(filename, "a");
    if (log_file == NULL) {
        PAQICHONG_PRINT("Error: Cannot open file %s\n", filename);
        return false;
    }

    // 写入文件头
    fPAQICHONG_PRINT(log_file, "timestamp_ms,acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z\n");
    fflush(log_file);

    PAQICHONG_PRINT("IMU logger initialized with file: %s\n", filename);
    return true;
}

static int paqichong_init_hardware_and_task(void)
{
    int ret = 0;
     PAQICHONG_PRINT("paqichong_init_hardware_and_task\n");
    //  if(g_facflag == 0)
    //  {
    //     return 0;
    //  }
    /* gesture predict timer */
    paqichong_measure_timer.handler = paqichong_start_measure_timer_handler;
    paqichong_measure_timer.interval = 1000*25;
    ret = osal_timer_init(&paqichong_measure_timer);
    if(ret != OSAL_SUCCESS) {
        PAQICHONG_PRINT("osal_timer_init failed\n");
    } else {
        osal_timer_start(&paqichong_measure_timer);
    }

    sync_time_timer.handler = paqichong_sync_time_handler;
    sync_time_timer.interval = 1000*20;
    ret = osal_timer_init(&sync_time_timer);
    if(ret != OSAL_SUCCESS) {
        PAQICHONG_PRINT("osal_timer_init failed\n");
    } else {
        osal_timer_start(&sync_time_timer);
    }

    ret = uapi_rtc_init();
    if(ret != ERRCODE_SUCC) {
        PAQICHONG_PRINT("uapi_rtc_init failed 0x%x", ret);
        free_container(container);
        return ret;
    }

    ret = uapi_rtc_adapter(0, RTC_0_IRQN, irq_prio(RTC_0_IRQN)); /* 1: using timer1 */
    if (ret != ERRCODE_SUCC) {
        PRINT("uapi_rtc_init FAILED 0x%x", ret);
        free_container(container);
        return ret;
    }

    ret = uapi_rtc_create(0, &g_imu_timer); /* 1: using timer1 */
    if (ret != ERRCODE_SUCC) {
        PRINT("uapi_rtc_create FAILED 0x%x", ret);
        free_container(container);
        return ret;
    }

    /* 事件处理task 相关结构体 */
    g_paqichong_module = (paqichong_module_t *)malloc(sizeof(paqichong_module_t));
    if (g_paqichong_module == NULL) {
        PAQICHONG_PRINT("malloc paqichong_module failed\n");
        free_container(container);
        return -1;
    }
    memset(g_paqichong_module, 0, sizeof(paqichong_module_t));

    // 创建数据解析task
    osThreadAttr_t threadAttr = {0};
    memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = "paqichong_task";
    threadAttr.stack_size = 0x3000;
    threadAttr.priority = 17;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);
    paqichong_task_id = osThreadNew(paqichong_task_entry, NULL, &threadAttr);
    if(paqichong_task_id == NULL) {
        PAQICHONG_PRINT("osal_kthread_create paqichong failed\r\n");
        free_container(container);
        ret = ERRCODE_FAIL;
    }


	if(osal_msg_queue_create("paqichong_event_queue", 10,
                         &g_paqichong_module->g_paqichong_event_queue, 0,
                         sizeof(paqichong_event_mail_t)) != OSAL_SUCCESS) {
        PAQICHONG_PRINT("create paqichong_event_queue failed\n");
        return -1;
	}

    // 创建事件处理task
	osThreadAttr_t paqichong_threadAttr={0};
	memset(&paqichong_threadAttr, 0, sizeof(paqichong_threadAttr));
    paqichong_threadAttr.name = "paqichong_handler_task";
    paqichong_threadAttr.stack_size = 0x1000;
    paqichong_threadAttr.priority = 17;
    paqichong_threadAttr.stack_mem = memalign(16, paqichong_threadAttr.stack_size);
	g_paqichong_module->paqichong_event_task_id = osThreadNew(paqichong_event_handler_entry, NULL, &paqichong_threadAttr);
    if ( g_paqichong_module->paqichong_event_task_id ==  NULL) {
        PAQICHONG_PRINT("osal_kthread_create paqichong_event_handler failed\r\n");
        ret = ERRCODE_FAIL;
    }
    PAQICHONG_PRINT("osal_kthread_create paqichong_event_handler sucess\r\n");

    osal_sem_init(&paqichong_sem, 0);
    osal_mutex_init(&sensor_data_mutex);
    PAQICHONG_PRINT("paqichong_init_hardware_and_task end\n");

    return ret;
}


int paqichong_tack_init(void)
{
    int ret = 0;

    paqichong_firmware_version = get_calculation_lib_version();
    FILE* f = fopen(merged_file, "rb");
    if (!f) {
        PAQICHONG_PRINT("Failed to open merged model file");
        model_bin_version = 0;
    }

    // 1. Initialize inference container with user-defined MIN_SEQ/MAX_SEQ
    if(f!=NULL)
    {
        container = init_container(f, 0);
        PAQICHONG_PRINT("Model Initialized\n");
        fclose(f);
    }

    if (!container)
    {
        PAQICHONG_PRINT("Failed to initialize inference container!\n");
        model_bin_version = 0;
    }
    else
    {
        model_bin_version = obtain_model_version_num(container);

        printf("Model bin version is %d\n", model_bin_version);
    }

    // 使用封装函数初始化硬件和任务
    ret = paqichong_init_hardware_and_task();
    if (ret != 0) {
        return ret;
    }

    return 0;
}

int paqichong_model_reload(void)
{
    int ret = 0;
    uapi_rtc_stop(g_imu_timer);
    paqichong_firmware_version = get_calculation_lib_version();
    FILE* f = fopen(merged_file, "rb");
    if (!f) {
        PAQICHONG_PRINT("Failed to open merged model file");
        model_bin_version = 0;
        model_replacing = 0;
        return -1;

    }
    free_container(container);
    // 1. Initialize inference container with user-defined MIN_SEQ/MAX_SEQ
    container = init_container(f, 0);
    PAQICHONG_PRINT("reload Model Initialized\n");
    fclose(f);

    if (!container) {
        PAQICHONG_PRINT("Failed to initialize inference container!\n");
       model_bin_version = 0;
    }
    else
    {
        model_bin_version = obtain_model_version_num(container);

        printf("reload Model bin version is %d\n", model_bin_version);
    }

    //ret = osal_sem_init(&paqichong_sem, 0);

    ret = uapi_rtc_start(g_imu_timer, PAQICHONG_TEST_TIME, paqichong_measure_timer_handler, 0);
    if(ret != ERRCODE_SUCC) {
        PAQICHONG_PRINT("uapi_rtc_start failed 0x%x", ret);
        free_container(container);
        model_replacing = 0;
        return -1;
    }
    model_replacing = 0;

    printf("reload Model bin version is finish\n");
    return 0;
}

void set_reload_model_flag(void)
{
    need_reload_model_flag = 1;
}