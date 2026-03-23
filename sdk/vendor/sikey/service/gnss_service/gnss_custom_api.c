/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss common proc
 * Author:
 * Create:
 */

#include "gnss_common_proc.h"
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "gnss_device.h"
#include "gnss_log.h"
#include "gnss_factory_test.h"
#include "tcxo.h"
#include "gnss_nmea_process.h"
#include "sk_board.h"

#define GNSS_BUFF_MAX_SIZE          10240
#if 0
#define GNSS_ENCODE_BUFF_MAX_LEN    9300
#define ONE_CENTURY_YEARS           100
#define FOUR_CENTURY_YEARS          400
#define LEAP_YEAR_LENGTH            4
#define UNIX_TIME_START_YEAR        1970    // start year of unix time
#define TIME_UNIT                   1000
#define SECONDS_PER_MIN             (60)
#define SECONDS_PER_HOUR            (60 * SECONDS_PER_MIN)
#define SECONDS_PER_DAY             (24 * SECONDS_PER_HOUR)
#define SECONDS_PER_NORMAL_YEAR     (365 * SECONDS_PER_DAY)
#endif

#define CUSTOM_GNSS_NONE_STATE 0
#define CUSTOM_GNSS_OPEN_STATE 1
#define CUSTOM_GNSS_IDLE_STATE 2
#define CUSTOM_GNSS_RUNNING_STATE 3

#define GNSS_DELAY_TIME_MS 200 // 200毫秒
#define GNSS_LOG_BASED_PATH "/user/xgnss/"
#define GNSS_OPEN_DATA_PATH "/user/xgnss/gnssdata.log"

static uint8_t g_gnssMsgBuff[GNSS_BUFF_MAX_SIZE] = {0};
static bool g_isNeedSaveBinary = false;
static bool g_isNeedSaveNmea = false;
static bool g_isNeedPrintNmea = true;
static uint32_t g_gnssState = CUSTOM_GNSS_NONE_STATE;

/** days per month on normal and leap year */
//int32_t g_normMonthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//int32_t g_leapMonthDays[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void custom_update_gnss_nmea_print_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedPrintNmea = true;
    } else {
        g_isNeedPrintNmea = false;
    }
}

void custom_update_binary_save_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedSaveBinary = true;
    } else {
        g_isNeedSaveBinary = false;
    }
}

void custom_update_nmea_save_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedSaveNmea = true;
    } else {
        g_isNeedSaveNmea = false;
    }
}

void custom_gnss_save_data(uint8_t dataType, uint8_t *buff, uint32_t size)
{
    FILE* logFile = NULL;
    if ((dataType == GNSS_CALLBACK_MSG) || (dataType == GNSS_CALLBACK_NMEA)) {
        logFile = fopen(GNSS_OPEN_DATA_PATH, "ab");
    } else {
        GNSS_LOG_ERROR("[GNSS ERROR]receive dataType error: %d\n", dataType);
        return;
    }

    if (logFile == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: open log file error\n");
        return;
    }

    size_t written = fwrite(buff, sizeof(uint8_t), size, logFile);
    if (written != size) {
        GNSS_LOG_ERROR("[GNSS ERROR]write binary fail, size is not equal, written:%d, msgLen:%d\n", written, size);
    }

    (void)fclose(logFile);
}

void custom_gnss_proc_binary_msg(uint8_t *buff, uint32_t len)
{
    errno_t ret = memcpy_s(g_gnssMsgBuff, GNSS_BUFF_MAX_SIZE, buff, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_binary_msg memcpy fail\n");
        return;
    }
    if (g_isNeedSaveBinary) {
        custom_gnss_save_data(GNSS_CALLBACK_MSG, g_gnssMsgBuff, len);
    }
    GnssMsg *msg = (GnssMsg*)g_gnssMsgBuff;
    if (msg == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss data is null");
        return;
    }

    if (msg->cmd == GNSS_FACTORY_TEST_RESULT) {
        custom_fac_proc_data(g_gnssMsgBuff);
    }
}

void custom_gnss_proc_nmea_msg(uint8_t *buff, uint32_t len)
{
    errno_t ret = memcpy_s(g_gnssMsgBuff, GNSS_BUFF_MAX_SIZE, buff, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_nmea_msg memcpy fail\n");
        return;
    }
    if (g_isNeedSaveNmea) {
        custom_gnss_save_data(GNSS_CALLBACK_NMEA, g_gnssMsgBuff, len);
    }

    // GNSS INIT时的快速start、stop，回放数据保存，但不让用户感知,不打印; 配置不打印NMEA时也不打印
    if (g_gnssState == CUSTOM_GNSS_OPEN_STATE) {
        return;
    }

    g_gnssMsgBuff[len] = '\0';
    if (g_isNeedPrintNmea) {
        GNSS_LOG_INFO("%s\n", (char *)g_gnssMsgBuff);
    }

    set_device_state(DEVICE_GPS);
    gnss_process_nmea(g_gnssMsgBuff);
}

static void check_log_base_path(void)
{
    static const int32_t modetype = 0775; // 文件权限设置
    int32_t ret = access(GNSS_LOG_BASED_PATH, F_OK);
    if (ret != 0) {
        GNSS_LOG_WARNING("[GNSS WARNING] %s not exist", GNSS_LOG_BASED_PATH);
        ret = mkdir(GNSS_LOG_BASED_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s", GNSS_LOG_BASED_PATH, strerror(errno));
            return;
        }

        if (chmod(GNSS_LOG_BASED_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s", GNSS_LOG_BASED_PATH, strerror(errno));
        }
    }
}

errcode_t custom_gnss_init(void)
{
    if (g_gnssState != CUSTOM_GNSS_NONE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = uapi_gnss_power_on();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss power on fail, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_init();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss init fail, error code: %#x\n", ret);
        return ret;
    }

    check_log_base_path();

    ret = uapi_gnss_register_callback(GNSS_CALLBACK_MSG, custom_gnss_proc_binary_msg);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("Failed to register the processing binary callback, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_register_callback(GNSS_CALLBACK_NMEA, custom_gnss_proc_nmea_msg);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]Failed to register the processing nmea callback, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_open();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss open fail, error code: %#x\n", ret);
        return ret;
    }

    g_gnssState = CUSTOM_GNSS_OPEN_STATE;

#ifdef CONFIG_LOWPOWER_GNSS_INIT
    // stop之后待机功耗才最低，所以执行一次快速的start、stop
    ret = uapi_gnss_start();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss start in init state fail, error code: %#x\n", ret);
        (void)gnss_deinit();
        return ret;
    }
    uapi_tcxo_delay_ms(GNSS_DELAY_TIME_MS);

    ret = uapi_gnss_stop();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss stop in init state fail, error code: %#x\n", ret);
        (void)gnss_deinit();
        return ret;
    }
    uapi_tcxo_delay_ms(GNSS_DELAY_TIME_MS);
#endif

    g_gnssState = CUSTOM_GNSS_IDLE_STATE;

    return ret;
}

errcode_t custom_gnss_start(void)
{
    if (g_gnssState != CUSTOM_GNSS_IDLE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already start or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    g_gnssState = CUSTOM_GNSS_RUNNING_STATE;
    errcode_t ret = uapi_gnss_start();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss start fail, error code: %#x\n", ret);
    }
    return ret;
}

errcode_t custom_gnss_stop(void)
{
    if (g_gnssState != CUSTOM_GNSS_RUNNING_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss not start or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = uapi_gnss_stop();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss stop fail, error code: %#x\n", ret);
    }
    g_gnssState = CUSTOM_GNSS_IDLE_STATE;
    return ret;
}

errcode_t custom_gnss_deinit(void)
{
    if (g_gnssState == CUSTOM_GNSS_NONE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already deinit or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = ERRCODE_FAIL;
    if (g_gnssState == CUSTOM_GNSS_RUNNING_STATE) {
        ret = gnss_stop();
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    ret = uapi_gnss_close();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss close fail, error code: %#x\n", ret);
    }
    ret = uapi_gnss_deinit();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss deinit fail, error code: %#x\n", ret);
    }
    ret = uapi_gnss_power_off();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss power off fail, error code: %#x\n", ret);
    }
    g_gnssState = CUSTOM_GNSS_NONE_STATE;
    return ret;
}

errcode_t custom_gnss_cold_start(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG] enter gnss cold start sample\n");

    const uint8_t cfgColdStartCmd[] = {0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};

    errcode_t ret = uapi_gnss_config(cfgColdStartCmd);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss configure cold start fail, error code: %#x\n", ret);
    }
    return ret;
}

errcode_t custom_gnss_hot_start(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG] enter gnss hot start sample\n");

    const uint8_t cfgHotStartCmd[] = {0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00};

    errcode_t ret = uapi_gnss_config(cfgHotStartCmd);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss configure hot start fail, error code: %#x\n", ret);
    }
    return ret;
}

#if 0
/* 计算命令内容校准和 */
static uint16_t calc_check_sum(uint8_t *buff, uint16_t len)
{
    uint16_t checksum = 0;
    uint16_t i = 0;
    while (i++ < len) {
        checksum += *buff;
        buff++;
    }
    return checksum;
}

static void get_utc_time(GnssUtcTime* utcTime)
{
    // 获取当前时间
    time_t now = time(NULL);
    struct tm* utc = gmtime(&now);  // 转换为 UTC 时间
    if (utc == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] get time error");
    }

    // 获取毫秒部分
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint16_t ms = (uint16_t)(tv.tv_usec / 1000);  // 将微秒转换为毫秒

    // 填充结构体
    utcTime->year = 1900 + utc->tm_year;  // tm_year 是从 1900 年开始的年数
    utcTime->month = 1 + utc->tm_mon;     // tm_mon 是从 0 开始的月份
    utcTime->day = utc->tm_mday;          // tm_mday 是月份中的日期
    utcTime->hour = utc->tm_hour;         // tm_hour 是小时
    utcTime->minute = utc->tm_min;        // tm_min 是分钟
    utcTime->second = utc->tm_sec;        // tm_sec 是秒
    utcTime->ms = ms;                     // 毫秒部分
    utcTime->valid = 1;                   // 设置为有效
}

errcode_t gnss_inject_time(GnssUtcTime *utcTime)
{
    uint8_t *buff = (uint8_t *)malloc(GNSS_ENCODE_BUFF_MAX_LEN);
    if (buff == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: malloc failed in gnss_inject_time\n");
        return 0;
    }
    (void)memset_s(buff, GNSS_ENCODE_BUFF_MAX_LEN, 0, GNSS_ENCODE_BUFF_MAX_LEN);

    GnssMsg *hdr = (GnssMsg *)buff;
    hdr->cmd = CMD_XGNSS_INJECT_UTC_TIME;
    hdr->sequence = 0;
    get_utc_time(utcTime);
    errno_t copyRet = memcpy_s(hdr->data, (GNSS_ENCODE_BUFF_MAX_LEN - sizeof(GnssMsg)), utcTime, sizeof(GnssUtcTime));
    if (copyRet != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR]memcpy UtcTime failed! Errno is %d\n", copyRet);
        free(buff);
        return ERRCODE_FAIL;
    }
    hdr->dataLength = sizeof(GnssUtcTime);
    hdr->checkSum = calc_check_sum(hdr->data, hdr->dataLength);
    errcode_t ret = uapi_gnss_config(buff);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss_inject_time error, error code: %#x\n", ret);
        free(buff);
        return ret;
    }

    return ret;
    free(buff);
}

/* 获取begin和end年份之间的闰年数 */
static int32_t get_leap_year_num(uint16_t begin, uint16_t end)
{
    if (end <= begin || end < 1) {
        return 0;
    }
    int32_t numPer400Year = (end - 1) / FOUR_CENTURY_YEARS - begin / FOUR_CENTURY_YEARS;
    int32_t numPer100Year = (end - 1) / ONE_CENTURY_YEARS - begin / ONE_CENTURY_YEARS;
    int32_t numPer4Year = (end - 1) / LEAP_YEAR_LENGTH - begin / LEAP_YEAR_LENGTH;
    return (numPer400Year + numPer4Year - numPer100Year);
}

/* 闰年判断 */
static int32_t check_if_leap_year(uint16_t year)
{
    if ((year % ONE_CENTURY_YEARS) == 0) {
        if ((year % FOUR_CENTURY_YEARS) == 0) {
            return 1;
        }
    } else {
        if ((year % LEAP_YEAR_LENGTH) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 将日历时(年月日时分秒) 转换为 unix时间(1970年1月1日起的时间秒数) */
int32_t utc_to_unixtime(GnssUtcTime utcTime, uint32_t *unixTime)
{
    if (unixTime == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] [utc_to_unixtime] null pointer!\n");
        return 0;
    }

    // 判断时间有效性（一年最多12个月）
    if (utcTime.month > 12 || utcTime.valid == 0) {
        GNSS_LOG_ERROR("[GNSS ERROR] [utc_to_unixtime] utcTime invalid!\n");
        return 0;
    }

    // 判断是否为闰年
    int32_t *monthDays = (check_if_leap_year(utcTime.year) == 1) ? g_leapMonthDays : g_normMonthDays;

    // 计算从1970.01.01开始的unix 秒数
    uint32_t timestamp = 0;
    timestamp += (uint32_t)(utcTime.year - UNIX_TIME_START_YEAR) * SECONDS_PER_NORMAL_YEAR;
    timestamp += (uint32_t)get_leap_year_num(UNIX_TIME_START_YEAR, utcTime.year) * SECONDS_PER_DAY;

    for (int32_t i = 1; i < utcTime.month; i++) {
        timestamp += (uint32_t)monthDays[i - 1] * SECONDS_PER_DAY;
    }

    timestamp += (uint32_t)(utcTime.day - 1) * SECONDS_PER_DAY;
    timestamp += (uint32_t)utcTime.hour * SECONDS_PER_HOUR;
    timestamp += (uint32_t)utcTime.minute * SECONDS_PER_MIN;
    uint32_t timestampSec = (uint32_t)(utcTime.second + utcTime.ms / TIME_UNIT);

    *unixTime = timestamp + timestampSec;
    return 1;
}

errcode_t encode_and_inject_cmd(GnssCmdType cmdType, uint8_t* inBuff, uint32_t inLen)
{
    // 获取GNSS业务消息头
    int32_t len = 0;
    uint8_t *buff = (uint8_t *)malloc(GNSS_ENCODE_BUFF_MAX_LEN);
    if (buff == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] malloc failed in encode_and_inject_cmd\n");
        return ERRCODE_FAIL;
    }
    memset_s(buff, GNSS_ENCODE_BUFF_MAX_LEN, 0, GNSS_ENCODE_BUFF_MAX_LEN);

    GnssMsg *gnssHdr = (GnssMsg *)buff;
    len += sizeof(GnssMsg);

    gnssHdr->sequence = 0;

    errno_t err = memcpy_s(buff + len, (GNSS_ENCODE_BUFF_MAX_LEN - len), inBuff, inLen);
    if (err != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] Encode Cmd failed: %#x, err:%d\n", cmdType, err);
        free(buff);
        return ERRCODE_FAIL;
    }
    gnssHdr->cmd = cmdType;
    gnssHdr->dataLength = inLen;
    gnssHdr->checkSum = calc_check_sum(gnssHdr->data, gnssHdr->dataLength);
    errcode_t ret = uapi_gnss_config(gnssHdr);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] inject cmd fail, error cmd:%#x code: %#x\n", gnssHdr->cmd, ret);
        free(buff);
        return ret;
    }

    GNSS_LOG_DEBUG("[GNSS DEBUG] inject cmd success: %#x\n", cmdType);
    free(buff);
    return ret;
}
#endif