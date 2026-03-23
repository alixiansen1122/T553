#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "chip_system_time.h"
#include "paqichong_web_service.h"
#include "http_api.h"
#include "paqichong_data_process.h"
#include "sh366102.h"
#include "sk_audio.h"
#include "location_service.h"
#include "gnss_service.h"
#include "net_data_process.h"
#include "barometer/barometer.h"
#include "dfx_reboot.h"
#include "dev_storage.h"
#include "watch_version.h"
#include "modem_cmd.h"
#include "paqichong_debug_info.h"

static int necklace_six_status = 0;
extern unsigned int connect_cnt;
extern bool get_sleep_flag(void);
extern uint32_t get_paqichong_pred_cnt(void);
extern void calculate_cnt_from_midnight_seconds(void);
extern char imei[32];
#define POSTURE_BUF_SIZE 4096
static int64_t g_cur_posture = 0;
static uint32_t g_cur_cnt = 0;
static char posture_buf[POSTURE_BUF_SIZE] = {0};
static int remaining = POSTURE_BUF_SIZE;
static int offset = 0;
static bool posture_action = 0;
bool location_flag = 0;
pthread_mutex_t posture_buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t last_posture_buf_mutex = PTHREAD_MUTEX_INITIALIZER;
uint8_t g_remote_crc = 0;
bool model_replacing = 0;
uint8_t g_remote_crc_modem = 0;
uint8_t g_remote_crc_host = 0;
union {
        float float_value;
        unsigned char bytes[4];
    } model_converter;
// 将消息类型字符串转换为枚举
MessageType get_message_type(const char *type_str)
{
    const char *types[] = {
        "REAL_TIME_ACTION",
        "REGULAR_TIME_ACTION",
        "PET_NECKLACE_ELECTRICITY",
        "PET_NECKLACE_SIX",
        "REPLACE_MODEL",
        "MODEL_VERSION",
        "BUZZER",
        "REPEATER_UPGRADATION",
        "SYNCHRONOUS_TIME",
        "SPEECH",
        "ACTIVE_LOCATION",
        "SEARCH_LOCATION",
        "RESTART",
        "SHUTDOWN",
        "PET_NECKLACE_CHARGING",
        "NECKLACE_OTA",
        "GET_AIR_PRESSURE",
        "GET_WIFILIST",
        "DORMANCY",
        "PAQICHONG_DEBUG"
    };

    for (int i = 0; i < sizeof(types) / sizeof(types[0]); i++)
    {
        if (strcmp(type_str, types[i]) == 0)
        {
            return (MessageType)i;
        }
    }
    return UNKNOWN_TYPE;
}

uint8_t download_crc8(uint8_t crc, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
    }
    return crc;
}

void ws_record_action(int64_t posture, uint32_t count)
{
    char buf[120] = {0};
    int length = 0;

    //开机阶段会获取到-1情况，过滤掉
    if (posture == -1)
    {
        return ;
    }
    /* 充电模式下不做上报 */
    if (g_chargestatus == 1)
    {
        return ;
    }
    /* posture_buf为空的话，给它填充一个姿态值 */
    pthread_mutex_lock(&posture_buf_mutex);
    if ((remaining == POSTURE_BUF_SIZE) && (offset == 0))
    {
        length = snprintf(buf, 120, "%lld,%u;", posture, count);
        memcpy(posture_buf + offset, buf, length);
        offset += length;
        remaining -= length;
    }
    pthread_mutex_unlock(&posture_buf_mutex);
    /* 新的姿态没改变，只修改count值 */
    if (posture == g_cur_posture)
    {
        if (count >= g_cur_cnt)
            g_cur_cnt = count;
    }
    else
    {
        pthread_mutex_lock(&last_posture_buf_mutex);
        g_cur_posture = posture;
        g_cur_cnt = count;
        pthread_mutex_unlock(&last_posture_buf_mutex);
    }
}
/****************** 组包部分 (项圈 -> 服务器) ******************/

// 打包实时动作数据
char *pack_real_time_action(const char *pet_mac, const char *data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "REAL_TIME_ACTION");
    cJSON_AddStringToObject(root, "action", data);
    cJSON_AddNumberToObject(root, "timestamp", get_utc_time());

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

// 打包定时数据
char *pack_regular_time_action(const char *pet_mac, const char *action_data)
{
    char buf[16] = {0};
    uint32_t elec_value = 0;
    char *json_str = NULL;
    float pressure = 0.0;
    char version[128] = {0};
    uint16_t voltage = 0;

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "REGULAR_TIME_ACTION");
    cJSON_AddStringToObject(root, "action", action_data);
    cJSON_AddNumberToObject(root, "timestamp", get_utc_time());

    snprintf(version, 128, "%s|%s", VERSION_STRING, version_2131);
    cJSON_AddStringToObject(root, "version", version);
    cJSON_AddNumberToObject(root, "modelVersion", model_bin_version);
    memset(version, 0, 128);
    snprintf(version, 128, "%.2f", paqichong_firmware_version);
    cJSON_AddStringToObject(root, "libversion", version);
    /*从库仑计读取*/
    elec_value = get_battery_level();
    snprintf(buf, 16, "%u", elec_value);
    cJSON_AddStringToObject(root, "electrlcity", buf);
    memset(buf, 0, 16);
    snprintf(buf, 16, "%u", g_chargestatus);
    cJSON_AddStringToObject(root, "chargeStatus", buf);
    /*读取电压值*/
    memset(buf, 0, 16);
    voltage = get_battery_voltage();
    snprintf(buf, 16, "%u", voltage);
    cJSON_AddStringToObject(root, "voltage", buf);
    /*获取气压计值*/
    memset(buf, 0, 16);
    pressure = barometer_read_data();
    snprintf(buf, 16, "%.0f", pressure);
    cJSON_AddStringToObject(root, "pressure", buf);
    /* 获取基站信息 */
    //pthread_mutex_lock(&bts_buf_mutex);
    //cJSON_AddStringToObject(root, "bts", bts_info_buf);
    //cJSON_AddStringToObject(root, "near_bts", nearbts_info_buf);
    //pthread_mutex_unlock(&bts_buf_mutex);
    /* 获取信号强度 */
    cJSON_AddStringToObject(root, "strength", signal_info_buf);
    /*获取scan wifi信息*/
    NET_DATA_PRINT("[net_data]wifi mutex start\n");
    pthread_mutex_lock(&wifi_info_buf_mutex);
    if (get_sleep_flag() == 1)
    {
        cJSON_AddStringToObject(root, "wifi", wifi_info_temp);
    }
    else
    {
        cJSON_AddStringToObject(root, "wifi", wifi_info_buf);
        memset(wifi_info_buf, 0, 1024);
    }
    pthread_mutex_unlock(&wifi_info_buf_mutex);
    NET_DATA_PRINT("[net_data]wifi mutex end\n");
    json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

// 发送定位数据
char *pack_location(const char *pet_mac, const char *posture_data)
{
    char *json_str = NULL;
    char buf[16] = {0};
    uint32_t elec_value = 0;
    float pressure = 0.0;
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "SEARCH_LOCATION");
    cJSON_AddNumberToObject(root, "timestamp", get_utc_time());
    cJSON_AddStringToObject(root, "action", posture_data);

    /* 获取信号强度 */
    cJSON_AddStringToObject(root, "strength", signal_info_buf);
    /* 获取基站信息 */
    pthread_mutex_lock(&bts_buf_mutex);
    cJSON_AddStringToObject(root, "bts", bts_info_buf);
    //cJSON_AddStringToObject(root, "near_bts", nearbts_info_buf);
    pthread_mutex_unlock(&bts_buf_mutex);
    /* 获取wifi信息 */
    // NET_DATA_PRINT("wifi and gnss mutex start\n");
    pthread_mutex_lock(&wifi_info_buf_mutex);
    cJSON_AddStringToObject(root, "wifi", wifi_serach_buf);
    //memset(wifi_serach_buf, 0, 512);
    pthread_mutex_unlock(&wifi_info_buf_mutex);
    /* 获取gps信息 */
    pthread_mutex_lock(&gnss_info_buf_mutex);
    cJSON_AddStringToObject(root, "gps", gnss_info_buf);
    memset(gnss_info_buf, 0, 64);
    pthread_mutex_unlock(&gnss_info_buf_mutex);
    // NET_DATA_PRINT("wifi and gnss mutex end\n");
    /*获取气压计值*/
    pressure = barometer_read_data();
    snprintf(buf, 16, "%.0f", pressure);
    cJSON_AddStringToObject(root, "pressure", buf);
    memset(buf, 0, 16);
    /*从库仑计读取*/
    elec_value = get_battery_level();
    snprintf(buf, 16, "%u", elec_value);
    cJSON_AddStringToObject(root, "electrlcity", buf);
    json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

// 打包模型版本
char *pack_model_version(const char *pet_mac, const char *data)
{
    (void)data;
    char version[128] = {0};
    char buf[16] = {0};
    uint32_t elec_value = 0;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "MODEL_VERSION");

    snprintf(version, 128, "%s|%s", VERSION_STRING, version_2131);
    cJSON_AddStringToObject(root, "version", version);
    cJSON_AddNumberToObject(root, "modelVersion", model_bin_version);
    memset(version, 0, 128);
    snprintf(version, 128, "%.2f", paqichong_firmware_version);
    cJSON_AddStringToObject(root, "libversion", version);
    cJSON_AddStringToObject(root, "iccid", sk_iccid);
    cJSON_AddStringToObject(root, "imei", imei);
    /*从库仑计读取*/
    elec_value = get_battery_level();
    snprintf(buf, 16, "%u", elec_value);
    cJSON_AddStringToObject(root, "electrlcity", buf);
    memset(buf, 0, 16);
    snprintf(buf, 16, "%u", g_chargestatus);
    cJSON_AddStringToObject(root, "chargeStatus", buf);
    cJSON_AddNumberToObject(root, "connect_cnt", connect_cnt);
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

// 打包充电状态
char *pack_charing_status(const char *pet_mac, const char *charging_status)
{
    char buf[16] = {0};
    uint32_t elec_value = 0;
    uint16_t voltage = 0;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "PET_NECKLACE_CHARGING");
    cJSON_AddStringToObject(root, "chargeStatus", charging_status);
    /*从库仑计读取*/
    elec_value = get_battery_level();
    snprintf(buf, 16, "%u", elec_value);
    cJSON_AddStringToObject(root, "electrlcity", buf);
    /*读取电池电压*/
    memset(buf, 0, 16);
    voltage = get_battery_voltage();
    snprintf(buf, 16, "%u", voltage);
    cJSON_AddStringToObject(root, "voltage", buf);
    cJSON_AddNumberToObject(root, "timestamp", get_utc_time());

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

// 打包wifi信息
char *pack_wifi_list(const char *pet_mac, const char *data)
{
    (void)data;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "GET_WIFILIST");
    cJSON_AddStringToObject(root, "wifi", wifi_info_buf);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

char *pack_air_pressure(const char *pet_mac, const char *data)
{
    (void)data;
    char buf[16] = {0};
    float pressure = 0.0;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "GET_AIR_PRESSURE");

    pressure = barometer_read_data();
    snprintf(buf, 16, "%.0f", pressure);
    cJSON_AddStringToObject(root, "pressure", buf);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

char *pack_ota_result(const char *pet_mac, const char *data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "DORMANCY");
    cJSON_AddStringToObject(root, "data", data);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

char *pack_debug_info(const char *pet_mac, const char *data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "petNecklaceCode", pet_mac);
    cJSON_AddStringToObject(root, "type", "PAQICHONG_DEBUG");
    cJSON_AddStringToObject(root, "data", data);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

// 通用组包函数
char *pack_data(MessageType type, void *data)
{
    char pet_mac[32] = {0};

    watch_storage_get(STORAGE_HARD_CODE, pet_mac, sizeof(pet_mac));
    if (pet_mac[0] == 0)
    {
        // if not find hardcode, use default value
        snprintf(pet_mac, 32, "11:11:11:11:11:11");
    }

    switch (type)
    {
    case REAL_TIME_ACTION:
        return pack_real_time_action(pet_mac, data);
    case REGULAR_TIME_ACTION:
        return pack_regular_time_action(pet_mac, (char *)data);
    // case PET_NECKLACE_ELECTRICITY:
    // return pack_battery(pet_mac, *(int *)data);
    case MODEL_VERSION:
        return pack_model_version(pet_mac, (char *)data);
    case SEARCH_LOCATION:
        return pack_location(pet_mac, (char *)data);
    case PET_NECKLACE_CHARGING:
        return pack_charing_status(pet_mac, (char *)data);
    case GET_WIFILIST:
        return pack_wifi_list(pet_mac, (char *)data);
    case GET_AIR_PRESSURE:
        return pack_air_pressure(pet_mac, (char *)data);
    case DORMANCY:
        return pack_ota_result(pet_mac, (char *)data);
    case PAQICHONG_DEBUG:
        return pack_debug_info(pet_mac, (char *)data);
    default:
        return NULL;
    }
}

/* 上报宠物实时动作 */
int32_t ws_report_real_time_action(int64_t posture, uint32_t count)
{
    int ret = 0;
    char buf[120] = {0};
    char real_posture_buf[120] = {0};
    char *data = NULL;
    int length = 0;

    /* 充电模式下不做上报 */
    if (g_chargestatus == 1)
    {
        return 0;
    }
    pthread_mutex_lock(&last_posture_buf_mutex);
    g_cur_posture = posture;
    g_cur_cnt = count;
    pthread_mutex_unlock(&last_posture_buf_mutex);
    snprintf(real_posture_buf, 120, "%lld,%u", posture, count);
    if (posture_action)
    {
        data = pack_data(REAL_TIME_ACTION, (void *)real_posture_buf);
        if (data)
        {
            ret = paqichong_send_data_to_server(data, strlen(data));
            free(data);
        }
        else
        {
            ret = -1;
        }
    }
    length = snprintf(buf, 120, "%lld,%u;", posture, count);
    /* 用于定时任务，保存姿态状态 */
    pthread_mutex_lock(&posture_buf_mutex);
    remaining = remaining - length;
    if (remaining > 0)
    {
        memcpy(posture_buf + offset, buf, length);
        offset += length;
    }
    else
    {
        remaining = POSTURE_BUF_SIZE;
        offset = 0;
        memset(posture_buf, 0, POSTURE_BUF_SIZE);
        memcpy(posture_buf + offset, buf, length);
        offset += length;
    }
    pthread_mutex_unlock(&posture_buf_mutex);
    return ret;
}

/* 上报宠物定时数据 */
int32_t ws_report_regular_time_action(void)
{
    int ret = 0;
    char *data = NULL;
    size_t len = 0;
    char *buf = malloc(POSTURE_BUF_SIZE + 120);
    if (!buf)
    {
        printf("ws_report_regular_time_action malloc failed\n");
        return -1;
    }
    else
    {
        memset(buf, 0, POSTURE_BUF_SIZE + 120);
    }

    pthread_mutex_lock(&posture_buf_mutex);
    if (g_chargestatus == 1)
    {
        /* 在充电状态确认下，是否之前还有数据遗留 */
        len = strlen(posture_buf);
        if (len != 0)
        {
            snprintf(buf, POSTURE_BUF_SIZE + 120, "%s%lld,%u;", posture_buf, g_cur_posture, g_cur_cnt);
        }
    }
    else
    {
        /*静止不动情况*/
        if (get_sleep_flag() == 1 && necklace_six_status == 0)
        {
            /*自动获取cnt值*/
            calculate_cnt_from_midnight_seconds();
            len = strlen(posture_buf);
            if (len != 0)
            {
                snprintf(buf, POSTURE_BUF_SIZE + 120, "%s%lld,%u;", posture_buf, g_cur_posture, get_paqichong_pred_cnt());
            }
            //else
            //{
                //snprintf(buf, POSTURE_BUF_SIZE + 120, "%lld,%u;%lld,%u;", g_cur_posture, g_cur_cnt+1, g_cur_posture, get_paqichong_pred_cnt());
            //}
            g_cur_cnt = get_paqichong_pred_cnt();
        }
        else
        {
            snprintf(buf, POSTURE_BUF_SIZE + 120, "%s%lld,%u;", posture_buf, g_cur_posture, g_cur_cnt);
        }
    }
    memset(posture_buf, 0, POSTURE_BUF_SIZE);
    remaining = POSTURE_BUF_SIZE;
    offset = 0;
    pthread_mutex_unlock(&posture_buf_mutex);

    data = pack_data(REGULAR_TIME_ACTION, (void *)buf);
    if (data)
    {
        ret = paqichong_send_heatbeat_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    free(buf);
    //修复下从静止到动态，再到静止，cnt值记录不全
    if (g_chargestatus == 0)
    {
        g_cur_cnt += 1;
        ws_record_action(g_cur_posture, g_cur_cnt);
    }
    return ret;
}

/* 上报宠物定位数据 */
int32_t ws_report_location(void)
{
    int ret = 0;
    char *data = NULL;
    char buf[120] = {0};

    pthread_mutex_lock(&last_posture_buf_mutex);
    snprintf(buf, 120, "%lld,%u", g_cur_posture, g_cur_cnt);
    pthread_mutex_unlock(&last_posture_buf_mutex);
    /*将wifi info 和 gnss info */
    data = pack_data(SEARCH_LOCATION, (void *)buf);
    if (data)
    {
        ret = paqichong_send_data_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

/* 上报充电状态 状态:0=未充电,1=充电中*/
uint8_t ws_report_chg_status(uint8_t chargeStatus)
{
    int ret = 0;
    char *data = NULL;
    char buf[8] = {0};

    snprintf(buf, 8, "%d", chargeStatus);
    data = pack_data(PET_NECKLACE_CHARGING, (void *)buf);
    if (data)
    {
        ret = paqichong_send_data_to_server_nonblocking(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

/* 上报wifilist信息 */
int32_t ws_report_wifilist(void)
{
    int ret = 0;
    char *data = NULL;

    data = pack_data(GET_WIFILIST, NULL);
    if (data)
    {
        ret = paqichong_send_data_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

int32_t ws_report_model_version(void)
{
    int ret = 0;
    char *data = NULL;

    data = pack_data(MODEL_VERSION, NULL);
    if (data)
    {
        ret = paqichong_send_data_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

int32_t ws_report_air_pressure(void)
{
    int ret = 0;
    char *data = NULL;

    data = pack_data(GET_AIR_PRESSURE, NULL);
    if (data)
    {
        ret = paqichong_send_data_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

int32_t ws_report_ota_result(char *buf)
{
    int ret = 0;
    char *data = NULL;

    data = pack_data(DORMANCY, buf);
    if (data)
    {
        ret = paqichong_send_data_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

int32_t ws_report_debug_info(char *buf)
{
    int ret = 0;
    char *data = NULL;

    data = pack_data(PAQICHONG_DEBUG, buf);
    if (data)
    {
        ret = paqichong_send_data_to_server(data, strlen(data));
        free(data);
    }
    else
    {
        ret = -1;
    }
    return ret;
}
/****************** 解包部分 (服务器 -> 项圈) ******************/
// 处理实时动作命令
void handle_real_time_action(cJSON *action)
{
    char *data = NULL;
    char buf[120] = {0};

    if (cJSON_IsString(action))
    {
        const char *value = action->valuestring;
        if (strcmp(value, "1") == 0)
        {
            printf("start action monitor\n");
            /* 不在充电状态下 */
            if (g_chargestatus == 0)
            {
                pthread_mutex_lock(&last_posture_buf_mutex);
                snprintf(buf, 120, "%lld,%u", g_cur_posture, g_cur_cnt);
                pthread_mutex_unlock(&last_posture_buf_mutex);
                data = pack_data(REAL_TIME_ACTION, (void *)buf);
                if (data)
                {
                    paqichong_send_data_to_server(data, strlen(data));
                    free(data);
                }
            }
            posture_action = 1;
        }
        else if (strcmp(value, "0") == 0)
        {
            printf("stop action monitor\n");
            posture_action = 0;
        }
    }
}

// 处理六轴数据命令
void handle_six_axis(cJSON *data)
{
    const char *data_str = data->valuestring;
    printf("Original data string: %s\n", data_str);

    // 创建可修改的副本
    char *copy = strdup(data_str);
    if (copy == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    // 使用 strtok 分割字符串
    char *token = strtok(copy, ";");
    char *command = NULL;
    int time_s = 0;
    int token_count = 0;
    int action = 0;

    while (token != NULL)
    {
        switch (token_count)
        {
        case 0: // 第一个token: "start" 或者 "end"
            command = token;
            printf("command: %s\n", command);
            if (strncmp(command, "start", 5) == 0)
            {
                action = 1;
            }
            else if (strncmp(command, "end", 3) == 0)
            {
                action = 0;
                token_count++; // 再次加1,跳过 case 1
            }
            break;
        case 1: // 第二个token是录制时间,单位为秒
            time_s = atoi(token);
            printf("set time is = %d\n", time_s);
            break;
        case 2: // 第三个token为code,上传文件时候需要携带
            watch_storage_set(STORAGE_DATA_CODE, token, strlen(token) + 1);
            printf("code is %s\n", token);
            break;
        default:
            printf("Extra token: %s\n", token);
            break;
        }
        token_count++;
        token = strtok(NULL, ";");
    }
    necklace_six_status = action;
    // 清理内存
    free(copy);
    if (action)
    {
        printf("start save imu data\n");
        paqichong_event_write(PAQICHONG_START_SAVE_IMU_DATA, (uint32_t)time_s);
    }
    else
    {
        printf("stop save imu data\n");
        paqichong_event_write(PAQICHONG_STOP_SAVE_IMU_DATA, 0);
    }
}

void handle_buzzer(cJSON *code, cJSON *level)
{
    char *filePath = NULL;
    const char *audio_code = code->valuestring;
    int32_t volme = level->valueint;
    int32_t number = atoi(audio_code);

    filePath = malloc(128);
    if (!filePath)
    {
        return;
    }
    memset(filePath, 0, 128);
    snprintf(filePath, 128, "/user/ring/bell_%d.ogg", number);
    sk_audio_player(filePath, 20000, 0, (uint32_t)volme);
}

// 处理用户语音,audioTimes是播放时长，returnAudioTimes是录音时长（ms)
// level: 1:2:3==>低：中：高
void handle_speech(cJSON *data, cJSON *audioTimes, cJSON *returnAudioTimes, cJSON *level)
{
    int ret = 0;
    const char *url = data->valuestring;
    int32_t play_time = audioTimes->valueint;
    int32_t record_time = returnAudioTimes->valueint;
    int32_t volme = level->valueint;
    int32_t retry_times = 5;

    if (!url && !play_time && !record_time && !volme)
    {
        printf("url is nuill or play and record is 0 or volme is 0\n");
        return;
    }
    printf("[handle_speech]url string: %s\n", url);
    printf("[handle_speech]play string: %d\n", play_time);
    printf("[handle_speech]record string: %d\n", record_time);
    printf("[handle_speech]volme string: %d\n", volme);

    if (access("/user/audio.ogg", F_OK) == 0)
    {
        // 文件存在，删除它
        if (remove("/user/audio.ogg") == 0)
        {
            printf("file /user/audio.ogg\n");
        }
    }
    while (retry_times > 0)
    {
        ret = http_download_audio(url);
        if (ret == 0)
        {
            ret = sk_audio_player("/user/audio.ogg", (uint32_t)play_time, (uint32_t)record_time, (uint32_t)volme);
            if (ret != 0)
            {
                printf("sk_audio_player failed, result = %d\n", ret);
            }
            break;
        }
        retry_times--;
    }
}

int update_model_file(void)
{
    const char *new_file = "/user/model.bin";
    const char *old_file = "/user/paqichong/model.bin";
    const char *temp_file = "/user/paqichong/model.bin.tmp";

    if (access(new_file, F_OK) != 0)
    {
        printf("err: new file %s not exist\n", new_file);
        return -1;
    }
    else /*judge if new file version is compatible with libverion, if 'yes' go on , otherwise return -1*/
    {
        FILE* f = fopen(new_file, "rb");
        if (!f) {
            printf("Failed to open download new file\n");
        }
        else{
             size_t bytes_read = fread(model_converter.bytes, 1, 4, f);
             if(bytes_read == 4)
                {
                    printf("Model bin -> lib version is %f\n", model_converter.float_value);
                    if(model_converter.float_value != paqichong_firmware_version)
                    {
                        fclose(f);
                        return -1;
                    }
                }
            }
            fclose(f);
    }

    FILE *src = fopen(new_file, "rb");
    FILE *dst = fopen(temp_file, "wb");

    if (!src || !dst)
    {
        printf("err: open failed\n");
        if (src)
            fclose(src);
        if (dst)
            fclose(dst);
        return -1;
    }

    char buffer[4096];
    size_t bytes;
    uint8_t crc = 0x00;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        crc = download_crc8(crc, (const uint8_t *)buffer, bytes);
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);

    printf("local crc = %u, remote crc = %u\n", crc, g_remote_crc);
    if (crc != g_remote_crc)
    {
        printf("err: model bin crc not compare\n");
        return -2;
    }

    if (rename(temp_file, old_file) != 0)
    {
        printf("err: not rename file: %s\n", strerror(errno));
        remove(temp_file); // 清理临时文件
        return -1;
    }

    printf("file update: %s -> %s\n", new_file, old_file);
    set_reload_model_flag();
    return 0;
}

void handle_location(cJSON *data)
{
    const char *value = data->valuestring;
    if (strcmp(value, "1") == 0)
    {
        printf("start location\n");
        location_flag = 1;
        location_start(1);
    }
    else if (strcmp(value, "0") == 0)
    {
        printf("stop location\n");
        location_flag = 0;
        searching_stop();
    }
}

void handle_necklace_ota(cJSON *modemUrl, cJSON *hostUrl, cJSON *modemCrc, cJSON *hostCrc)
{
    size_t url_len = 0, file_path_len = 0;
    char buf[1024] = {0};
    const char *modem_url = (modemUrl != NULL) ? modemUrl->valuestring : NULL;
    const char *modem_file = "/update/Hi2131EV100.fwpkg.bin";
    const char *hosturl = (hostUrl != NULL) ? hostUrl->valuestring : NULL;
    const char *hosturl_file = "/update/update.fwpkg";
    sk_update_mode_t update_mode = 0;
    uint8_t other_type = 0;
    if (modem_url && hosturl)
    {
        update_mode = SK_UPDATE_MODEM_AND_MCU;
    }
    else
    {
        if (modem_url)
        {
            update_mode = SK_UPDATE_MODEM_ONLY;
        }
        else
        {
            update_mode = SK_UPDATE_MCU_ONLY;
        }
    }
    if (modem_url)
    {
        url_len = strlen(modem_url);
        file_path_len = strlen(modem_file);

        if (url_len >= MAX_URL_LENGTH || file_path_len >= MAX_FILE_PATH_LENGTH)
        {
            printf("modem url or file len is too long\n");
            return;
        }
        snprintf(buf, 1024, "%s;%s", modem_url, modem_file);
        if (update_mode == SK_UPDATE_MODEM_AND_MCU)
        {
            other_type = TYPE_DOWN_FIRST;
        }
        else
        {
            other_type = TYPE_DOWN_MODEM;
        }
        g_remote_crc_modem = (uint8_t)modemCrc->valueint;
        http_send_data_to_server(buf, url_len + file_path_len + 2, HTTP_DOWNlOAD_OTA, other_type);
    }
    memset(buf, 0, 1024);
    if (hosturl)
    {
        url_len = strlen(hosturl);
        file_path_len = strlen(hosturl_file);

        if (url_len >= MAX_URL_LENGTH || file_path_len >= MAX_FILE_PATH_LENGTH)
        {
            printf("host url or file len is too long\n");
            return;
        }
        snprintf(buf, 1024, "%s;%s", hosturl, hosturl_file);
        if (update_mode == SK_UPDATE_MODEM_AND_MCU)
        {
            other_type = TYPE_DOWN_SECOND;
        }
        else
        {
            other_type = TYPE_DOWN_HOST;
        }
        g_remote_crc_host = (uint8_t)hostCrc->valueint;
        http_send_data_to_server(buf, url_len + file_path_len + 2, HTTP_DOWNlOAD_OTA, other_type);
    }
}

// 主解包函数
void unpack_data(const char *json_str)
{
    cJSON *audioTimes = NULL, *returnAudioTimes = NULL;
    cJSON *searchStatus = NULL, *actionStatus = NULL;
    cJSON *fileUrl = NULL, *modelVersion = NULL, *modelCrc = NULL;
    cJSON *level = NULL, *code = NULL;
    cJSON *sixStatus = NULL;
    cJSON *modemUrl = NULL, *hostUrl = NULL, *modemCrc = NULL, *hostCrc = NULL;

    cJSON *root = cJSON_Parse(json_str);
    if (!root)
    {
        printf("JSON parse error\n");
        return;
    }

    // 获取公共字段
    cJSON *pet_mac = cJSON_GetObjectItemCaseSensitive(root, "petNecklaceCode");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(root, "type");
    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");

    if (!cJSON_IsString(pet_mac) || !cJSON_IsString(type))
    {
        cJSON_Delete(root);
        return;
    }

    MessageType msg_type = get_message_type(type->valuestring);
    // 根据类型处理数据
    switch (msg_type)
    {
    case REAL_TIME_ACTION:
        actionStatus = cJSON_GetObjectItemCaseSensitive(root, "actionStatus");
        if (cJSON_IsString(actionStatus))
        {
            handle_real_time_action(actionStatus);
        }
        break;
    case PET_NECKLACE_ELECTRICITY:
        // 请求电量
        // TODO: 触发电量上报
        break;
    case REPLACE_MODEL:
        fileUrl = cJSON_GetObjectItemCaseSensitive(root, "fileUrl");
        modelVersion = cJSON_GetObjectItemCaseSensitive(root, "modelVersion");
        modelCrc = cJSON_GetObjectItemCaseSensitive(root, "crc");
        printf("start replace model\n");
        if (cJSON_IsString(fileUrl) && cJSON_IsNumber(modelVersion) && cJSON_IsNumber(modelCrc))
        {
            /* 正在模型替换 */
            if (model_replacing == 1)
            {
                printf("model is placing\n");
                break;
            }
            else
            {
                model_replacing = 1;
            }
            g_remote_crc = (uint8_t)modelCrc->valueint;
            http_send_data_to_server(fileUrl->valuestring, 256, HTTP_DOWNLOAD_MODEL, 0);
        }
        break;
    case BUZZER:
        code = cJSON_GetObjectItemCaseSensitive(root, "code");
        level = cJSON_GetObjectItemCaseSensitive(root, "level");
        if (cJSON_IsString(code) && cJSON_IsNumber(level))
        {
            handle_buzzer(code, level);
        }
        break;
    case PET_NECKLACE_SIX:
        sixStatus = cJSON_GetObjectItemCaseSensitive(root, "sixStatus");
        if (cJSON_IsString(sixStatus))
        {
            handle_six_axis(sixStatus);
        }
        break;
    case NECKLACE_OTA:
        modemUrl = cJSON_GetObjectItemCaseSensitive(root, "modemUrl");
        hostUrl = cJSON_GetObjectItemCaseSensitive(root, "hostUrl");
        modemCrc = cJSON_GetObjectItemCaseSensitive(root, "modemCrc");
        hostCrc = cJSON_GetObjectItemCaseSensitive(root, "hostCrc");
        /*已经在ota下载文件中*/
        if (get_download_file_state() == 1)
        {
            NET_DATA_PRINT("return in ota downloading\n");
            break;
        }
        if ((cJSON_IsString(modemUrl) && cJSON_IsNumber(modemCrc)) || (cJSON_IsString(hostUrl) && cJSON_IsNumber(hostCrc)))
        {
            NET_DATA_PRINT("in ota downloading mode\n");
            handle_necklace_ota(modemUrl, hostUrl, modemCrc, hostCrc);
        }
        break;
    case SPEECH:
        fileUrl = cJSON_GetObjectItemCaseSensitive(root, "fileUrl");
        audioTimes = cJSON_GetObjectItemCaseSensitive(root, "audioTimes");
        returnAudioTimes = cJSON_GetObjectItemCaseSensitive(root, "returnAudioTimes");
        level = cJSON_GetObjectItemCaseSensitive(root, "level");
        if (cJSON_IsString(fileUrl) && cJSON_IsNumber(audioTimes) && cJSON_IsNumber(returnAudioTimes) && cJSON_IsNumber(level))
        {
            handle_speech(fileUrl, audioTimes, returnAudioTimes, level);
        }
        break;
    case REGULAR_TIME_ACTION:
        if (cJSON_IsBool(data))
        {
            bool pong = data->valueint;
            printf("heart response pong=%d\n", pong);
            if (pong)
            {
                websocket_response_heart();
            }
            printf("heart response end\n");
        }
        break;
    case SEARCH_LOCATION:
    {
        searchStatus = cJSON_GetObjectItemCaseSensitive(root, "searchStatus");
        if (cJSON_IsString(searchStatus))
        {
            handle_location(searchStatus);
        }
        break;
    }
    case RESTART:
    {
        uapi_system_reboot(SYSTEM_SOFT_REBOOT);
        break;
    }
    case SHUTDOWN:
    {
        paqichong_send_cmd(PQC_WEB_EVENT_DISCONNECT);
        break;
    }
    case GET_WIFILIST:
    {
        get_wifilist();
    }
    break;
    case MODEL_VERSION:
    {
        ws_report_model_version();
    }
    break;
    case GET_AIR_PRESSURE:
    {
        ws_report_air_pressure();
    }
    break;
    case PAQICHONG_DEBUG:
    {
        if (cJSON_IsString(data))
        {
            handle_debug_data(data);
        }
    }
    break;
    default:
        fprintf(stderr, "Unknown message type: %s\n", type->valuestring);
        break;
    }

    cJSON_Delete(root);
}
