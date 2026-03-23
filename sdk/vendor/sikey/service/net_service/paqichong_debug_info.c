#include "paqichong_debug_info.h"
#include <unistd.h>
#include "gnss_custom_api.h"
#include <sys/stat.h>

unsigned int uploading_log = 0;

void paqichong_save_log(const char *fmt, ...)
{
    FILE *logFile = NULL;
    char buf[256] = {0};
    size_t size = 0;

    char user_msg[128] = {0};
    va_list args;

    if (uploading_log == 1)
    {
        return;
    }
    va_start(args, fmt);
    vsnprintf(user_msg, sizeof(user_msg), fmt, args);
    va_end(args);

    const char *logPath = "/user/ws.log";
    struct stat fileStat;
    // 获取文件状态信息
    if (stat(logPath, &fileStat) == 0)
    {
        // 文件存在，检查大小（stat结构体的st_size成员是文件大小，单位字节）
        if (fileStat.st_size > 20 * 1024)  // 20KB = 20 * 1024 字节
        {
            // 删除原文件
            if (remove(logPath) != 0)
            {
                printf("Failed to delete old log file\n");
                // 可以选择继续写入或返回
            }
        }
    }

    logFile = fopen(logPath, "ab");
    if (logFile == NULL)
    {
        printf("open /user/ws.log error\n");
        return;
    }

    size = snprintf(buf, sizeof(buf), "%d,%s\r\n", get_utc_time(), user_msg);

    size_t written = fwrite(buf, sizeof(char), size, logFile);
    if (written != size)
    {
    }

    (void)fclose(logFile);
}

void handle_debug_data(cJSON *data)
{
    char buf[128] = {0};
    const char *value = data->valuestring;
    snprintf(buf, 128, "%s\r\n", value);
    if (0 == strncmp(value, "gs_cal", 6))
    {
        int ret = 0;
        char result[64] = {0};
        ret = gsensor_calibrate();
        if (ret == 0)
        {
            snprintf(result, sizeof(result), "%s", "gsensor pass");
        }
        else
        {
            snprintf(result, sizeof(result), "%s", "gsensor failed");
        }
        ws_report_debug_info(result);
    }
    else if (0 == strncmp(value, "gs_rd", 5))
    {
        int16_t acc_data[3] = {0};
        int16_t gyro_data[3] = {0};
        memset(buf, 0, 128);
        gsensor_read_data(acc_data, gyro_data);
        if (acc_data[2] == 0 && acc_data[1] == 0 && acc_data[0] == 0)
        {
            osal_mdelay(50);
            gsensor_read_data(acc_data, gyro_data);
        }
        snprintf(buf, 128, "acc[mg]:%d,%d,%d,gro[mdps]:%d,%d,%d", acc_data[0], acc_data[1], acc_data[2],
                 gyro_data[0], gyro_data[1], gyro_data[2]);
        ws_report_debug_info(buf);
    }
    else if (0 == strncmp(value, "report_log", 10))
    {
        http_upload_log("/user/ws.log");
    }
    else if (0 == strncmp(value, "remove_log", 10))
    {
        if (access("/user/ws.log", F_OK) == 0)
        {
            // 文件存在，删除它
            if (remove("/user/ws.log") == 0)
            {
                printf("file /user/ws.log removed\n");
            }
        }
    }
    else if (0 == strncmp(value, "save_nmea_log", 13))
    {
        if (access("/user/xgnss/gnssdata.log", F_OK) == 0)
        {
            // 文件存在，删除它
            if (remove("/user/xgnss/gnssdata.log") == 0)
            {
                printf("file /user/xgnss/gnssdata.log removed\n");
            }
        }
        custom_update_nmea_save_flag(1);
    }
    else if (0 == strncmp(value, "close_nmea_log", 14))
    {
        custom_update_nmea_save_flag(0);
        http_upload_log("/user/xgnss/gnssdata.log");
    }
    else if (0 == strncmp(value, "close_debug", 11))
    {
        net_debug_flag_set(0);
    }
    else if (0 == strncmp(value, "start_debug", 11))
    {
        net_debug_flag_set(1);
        SendnetCommand((uint8_t *)buf, NULL);
    }
}