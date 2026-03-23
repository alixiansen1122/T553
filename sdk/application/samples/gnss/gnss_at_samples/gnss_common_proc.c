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

#define BUFF_MAX_SIZE               10240
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
#define GNSS_NONE_STATE             0
#define GNSS_OPEN_STATE             1
#define GNSS_IDLE_STATE             2
#define GNSS_RUNNING_STATE          3
#define GNSS_DELAY_TIME             200    // 200毫秒
#define LOG_DIR_PATH                "/user/gnss/"
#define XGNSS_DATA_PATH             "/user/gnss/xgnss/"
#define LOG_BASED_PATH              "/user/gnss/log/"
#define GNSS_DATA_PATH              "/user/gnss/log/gnss_data"
#define LOG_FILE_NAME_LEN           100
#define BYTE_TO_MB                  1048576 // 1MB大小 1024 * 1024
#define DEFAULT_LOG_NUM             10
#define DEFAULT_LOG_SIZE            5 // 单位：MB

typedef struct {
    uint32_t logNumLimit; // 日志最大保存个数
    uint32_t logSizeLimit; // 日志大小限制，单位：MB
    uint32_t logNum; // 当前日志个数
    uint32_t logSize; // 最新日志文件大小
    char fileName[LOG_FILE_NAME_LEN]; // with index
} LogCtrlInfo;

static uint8_t g_gnssMsgBuff[BUFF_MAX_SIZE] = {0};
static bool g_isNeedSaveBinary = false;
static bool g_isNeedSaveNmea = false;
static bool g_isNeedPrintNmea = true;
static uint32_t g_gnssState = GNSS_NONE_STATE;
static LogCtrlInfo g_logCtrlInfo = {0};
/** days per month on normal and leap year */
int32_t g_normMonthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int32_t g_leapMonthDays[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static void check_log_base_path(void)
{
    static const int32_t modetype = 0775; // 文件权限设置
    int32_t ret = access(LOG_DIR_PATH, F_OK);
    if (ret != 0) {
        ret = mkdir(LOG_DIR_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s\n", LOG_DIR_PATH, strerror(errno));
            return;
        }

        if (chmod(LOG_DIR_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s\n",
                LOG_DIR_PATH, strerror(errno));
        }
    }

    ret = access(LOG_BASED_PATH, F_OK);
    if (ret != 0) {
        ret = mkdir(LOG_BASED_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s\n", LOG_BASED_PATH, strerror(errno));
            return;
        }

        if (chmod(LOG_BASED_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s\n",
                LOG_BASED_PATH, strerror(errno));
        }
    }

    ret = access(XGNSS_DATA_PATH, F_OK);
    if (ret != 0) {
        ret = mkdir(XGNSS_DATA_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s\n", XGNSS_DATA_PATH, strerror(errno));
            return;
        }

        if (chmod(XGNSS_DATA_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s\n",
                XGNSS_DATA_PATH, strerror(errno));
        }
    }
}

errcode_t update_gnss_log_ctrl_info(uint32_t logNumLimit, uint32_t logSizeLimit)
{
    g_logCtrlInfo.logNumLimit = logNumLimit;
    g_logCtrlInfo.logSizeLimit = logSizeLimit;
    GNSS_LOG_DEBUG("[GNSS DEBUG] logNumLimit: %u, "
                   "logSizeLimit: %u\n",
                   g_logCtrlInfo.logNumLimit, g_logCtrlInfo.logSizeLimit);
    return ERRCODE_SUCC;
}

static void init_gnss_log_ctrl_file_info(void)
{
    char logName[LOG_FILE_NAME_LEN];
    for (uint32_t i = g_logCtrlInfo.logNumLimit; i > 0; i--) {
        (void)memset_s(logName, sizeof(logName), 0, sizeof(logName));
        (void)sprintf_s(logName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", GNSS_DATA_PATH, i);
        int32_t ret = access(logName, F_OK);
        if (ret == 0) {
            g_logCtrlInfo.logNum = i; // 确认下当前日志文件数量
            break;
        }
    }
    (void)sprintf_s(logName, LOG_FILE_NAME_LEN * sizeof(char), "%s_1.log", GNSS_DATA_PATH);
    (void)memcpy_s(g_logCtrlInfo.fileName, LOG_FILE_NAME_LEN * sizeof(char), logName, sizeof(logName));
    FILE* logFile = fopen(g_logCtrlInfo.fileName, "ab");
    if (logFile == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: open log file %s error\n", g_logCtrlInfo.fileName);
        return;
    }
    int result = fseek(logFile, 0, SEEK_END);
    if (result != 0) {
        /* 处理错误，记录日志 */
        GNSS_LOG_ERROR("[GNSS ERROR] fseek failed, errno:%d\n", result);
    }
    g_logCtrlInfo.logSize = (uint32_t)ftell(logFile);
    (void)fclose(logFile);
}

static uint32_t FindMaxLogIndex(uint32_t logNumLimit)
{
    uint32_t maxIndex = 0;
    char currentLogName[LOG_FILE_NAME_LEN];
    for (uint32_t i = logNumLimit; i > 0; i--) {
        (void)memset_s(currentLogName, sizeof(currentLogName), 0, sizeof(currentLogName));
        (void)sprintf_s(currentLogName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", GNSS_DATA_PATH, i);
        if (access(currentLogName, F_OK) == 0) {
            maxIndex = i;
            break;
        }
    }
    return maxIndex;
}

static int RenameLogs(uint32_t maxIndex)
{
    for (uint32_t i = maxIndex; i > 0; i--) {
        char oldName[LOG_FILE_NAME_LEN];
        char newName[LOG_FILE_NAME_LEN];
        (void)memset_s(oldName, sizeof(oldName), 0, sizeof(oldName));
        (void)memset_s(newName, sizeof(newName), 0, sizeof(newName));
        (void)sprintf_s(oldName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", GNSS_DATA_PATH, i);
        (void)sprintf_s(newName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", GNSS_DATA_PATH, i + 1);

        if (rename(oldName, newName) != 0) {
            GNSS_LOG_ERROR("Failed to rename %s to %s, errno:%s", oldName, newName, strerror(errno));
            return -1;
        }
    }
    return 0;
}

static int DeleteOldestLog(uint32_t logNumLimit)
{
    char oldestLogName[LOG_FILE_NAME_LEN];
    (void)memset_s(oldestLogName, sizeof(oldestLogName), 0, sizeof(oldestLogName));
    (void)sprintf_s(oldestLogName, LOG_FILE_NAME_LEN * sizeof(char),
                    "%s_%d.log", GNSS_DATA_PATH, logNumLimit + 1);
    if (access(oldestLogName, F_OK) == 0) {
        if (remove(oldestLogName) != 0) {
            GNSS_LOG_ERROR("Failed to remove %s, errno: %s", oldestLogName, strerror(errno));
            return -1;
        }
    }
    return 0;
}

static int CreateNewLogFile(const char* dataPath, char* fileName)
{
    (void)sprintf_s(fileName, LOG_FILE_NAME_LEN * sizeof(char), "%s_1.log", dataPath);
    FILE* newLogFile = fopen(fileName, "ab");
    if (newLogFile == NULL) {
        GNSS_LOG_ERROR("Failed to create new log file %s, errno: %s", fileName, strerror(errno));
        return -1;
    }

    if (fclose(newLogFile) != 0) {
        GNSS_LOG_ERROR("Failed to close new log file %s, errno: %s", fileName, strerror(errno));
        return -1;
    }
    
    return 0;
}

static void gnss_check_log_size(void)
{
    if (g_logCtrlInfo.logSize >= g_logCtrlInfo.logSizeLimit * BYTE_TO_MB) {
        uint32_t maxIndex = FindMaxLogIndex(g_logCtrlInfo.logNumLimit);

        int renameResult = RenameLogs(maxIndex);
        if (renameResult != 0) {
            GNSS_LOG_ERROR("Failed to rename files, log management aborted");
            return;
        }

        if (maxIndex >= g_logCtrlInfo.logNumLimit) {
            int deleteResult = DeleteOldestLog(g_logCtrlInfo.logNumLimit);
            if (deleteResult != 0) {
                GNSS_LOG_ERROR("Failed to delete oldest log file, log management aborted");
                return;
            }
        }

        int createResult = CreateNewLogFile(GNSS_DATA_PATH, g_logCtrlInfo.fileName);
        if (createResult != 0) {
            GNSS_LOG_ERROR("Failed to create new log file, log management aborted");
            return;
        }

        g_logCtrlInfo.logSize = 0;
        g_logCtrlInfo.logNum++;
    }
}

static void init_gnss_log_ctrl_info(void)
{
    check_log_base_path();
    g_logCtrlInfo.logNumLimit = DEFAULT_LOG_NUM;
    g_logCtrlInfo.logSizeLimit = DEFAULT_LOG_SIZE;
    g_logCtrlInfo.logNum = 1;
    g_logCtrlInfo.logSize = 0;
    init_gnss_log_ctrl_file_info();
    gnss_check_log_size();
}

void update_gnss_nmea_print_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedPrintNmea = true;
    } else {
        g_isNeedPrintNmea = false;
    }
}

void update_binary_save_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedSaveBinary = true;
    } else {
        g_isNeedSaveBinary = false;
    }
}

void update_nmea_save_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedSaveNmea = true;
    } else {
        g_isNeedSaveNmea = false;
    }
}

static void gnss_save_data(uint8_t dataType, uint8_t *buff, uint32_t size)
{
    FILE* logFile = NULL;
    if ((dataType == GNSS_CALLBACK_MSG) || (dataType == GNSS_CALLBACK_NMEA)) {
        logFile = fopen(g_logCtrlInfo.fileName, "ab");
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

    int result = fseek(logFile, 0, SEEK_END);
    if (result != 0) {
        /* 处理错误，记录日志 */
        GNSS_LOG_ERROR("[GNSS ERROR] fseek failed, error code: %d\n", result);
    }
    g_logCtrlInfo.logSize = (uint32_t)ftell(logFile);

    (void)fclose(logFile);
    gnss_check_log_size();
}

void gnss_proc_binary_msg(uint8_t *buff, uint32_t len)
{
    errno_t ret = memcpy_s(g_gnssMsgBuff, BUFF_MAX_SIZE, buff, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_binary_msg memcpy fail\n");
        return;
    }
    if (g_isNeedSaveBinary) {
        gnss_save_data(GNSS_CALLBACK_MSG, g_gnssMsgBuff, len);
    }
    GnssMsg *msg = (GnssMsg*)g_gnssMsgBuff;
    if (msg == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss data is null");
        return;
    }

    if (msg->cmd == GNSS_FACTORY_TEST_RESULT) {
        fac_proc_data(g_gnssMsgBuff);
    }
}

void gnss_proc_nmea_msg(uint8_t *buff, uint32_t len)
{
    errno_t ret = memcpy_s(g_gnssMsgBuff, BUFF_MAX_SIZE, buff, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_nmea_msg memcpy fail\n");
        return;
    }
    if (g_isNeedSaveNmea) {
        gnss_save_data(GNSS_CALLBACK_NMEA, g_gnssMsgBuff, len);
    }

    // GNSS INIT时的快速start、stop，回放数据保存，但不让用户感知,不打印; 配置不打印NMEA时也不打印
    if (g_gnssState == GNSS_OPEN_STATE || !g_isNeedPrintNmea) {
        return;
    }
    g_gnssMsgBuff[len] = '\0';
    GNSS_LOG_INFO("%s\n", (char *)g_gnssMsgBuff);
}

errcode_t gnss_init(void)
{
    if (g_gnssState != GNSS_NONE_STATE) {
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

    init_gnss_log_ctrl_info();

    ret = uapi_gnss_register_callback(GNSS_CALLBACK_MSG, gnss_proc_binary_msg);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("Failed to register the processing binary callback, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_register_callback(GNSS_CALLBACK_NMEA, gnss_proc_nmea_msg);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]Failed to register the processing nmea callback, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_open();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss open fail, error code: %#x\n", ret);
        return ret;
    }

    g_gnssState = GNSS_OPEN_STATE;

#ifdef CONFIG_LOWPOWER_GNSS_INIT
    // stop之后待机功耗才最低，所以执行一次快速的start、stop
    ret = uapi_gnss_start();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss start in init state fail, error code: %#x\n", ret);
        (void)gnss_deinit();
        return ret;
    }
    uapi_tcxo_delay_ms(GNSS_DELAY_TIME);

    ret = uapi_gnss_stop();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss stop in init state fail, error code: %#x\n", ret);
        (void)gnss_deinit();
        return ret;
    }
    uapi_tcxo_delay_ms(GNSS_DELAY_TIME);
#endif

    g_gnssState = GNSS_IDLE_STATE;

    return ret;
}

errcode_t gnss_start(void)
{
    if (g_gnssState != GNSS_IDLE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already start or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    g_gnssState = GNSS_RUNNING_STATE;
    errcode_t ret = uapi_gnss_start();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss start fail, error code: %#x\n", ret);
    }
    return ret;
}

errcode_t gnss_stop(void)
{
    if (g_gnssState != GNSS_RUNNING_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss not start or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = uapi_gnss_stop();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss stop fail, error code: %#x\n", ret);
    }
    g_gnssState = GNSS_IDLE_STATE;
    return ret;
}

errcode_t gnss_deinit(void)
{
    if (g_gnssState == GNSS_NONE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already deinit or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = ERRCODE_FAIL;
    if (g_gnssState == GNSS_RUNNING_STATE) {
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
    g_gnssState = GNSS_NONE_STATE;
    return ret;
}

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
    free(buff);
    return ret;
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