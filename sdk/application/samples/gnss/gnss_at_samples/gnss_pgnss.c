/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss pgnss sample
 * Author:
 * Create:
 */

#include "gnss_pgnss.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include "gnss_common_proc.h"
#include "gnss_log.h"

#define FILE_PATH_LEN               1024
#define GLO_EPH_VALID_TIME          900
#define OTHER_EPH_VALID_TIME        7200
#define NON_GLO_GNSS_TYPES          4
#define GLO_EPH_NUM_ONE_FILE        8
#define OTHER_ASSIST_TYPES          7
#define PGNSS_FILE_MAX_SIZE         1024000
#define PGNSS_DATA_PATH             "/user/gnss/xgnss/"
#define PGNSS_ASSISTINFO_DATA_PATH  "/user/gnss/xgnss/AssistInfo.dat"
#define PGNSS_NONGLO_DATA_PATH      "/user/gnss/xgnss/NonGlo_"

static FILE* g_gnssLogFp = NULL;
static uint8_t g_gnssServiceBuff[REPORT_MAX_BYTES] = { 0 };

// 以下为使用PGNSS解析数据合并输出接口生成的星历文件进行注入的代码
int32_t get_nonglo_eph_file_name(int32_t ts, char *fileName)
{
    const char prefix[] = "NonGlo";
    DIR *dir = opendir(PGNSS_DATA_PATH);
    if (dir == NULL) {
        GNSS_LOG_ERROR("[PGNSS ERROR] Failed to open /user/gnss/xgnss directory.\n");
        return 0;
    }
    int32_t time, ret;
    int32_t minDist = OTHER_EPH_VALID_TIME;
    int32_t closestTime = 0;
    struct dirent *entry;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        // 检查文件名
        if (strstr(entry->d_name, ".eph") != NULL) {
            char *p = strrchr(entry->d_name, '_');
            if (p != NULL) {
                char *q = strstr(entry->d_name, prefix);
                if (q != NULL) {
                    time = atoi(p + 1);
                    // 查找星历文件时间戳与当前时间相差2小时以内且距离最近的
                    if (abs(time - ts) < minDist) {
                        minDist = abs(time - ts);
                        closestTime = time;
                    }
                }
            }
        }
    }
    if (closestTime != 0) {
        ret = snprintf_s(fileName, FILE_PATH_LEN, (FILE_PATH_LEN - 1), "%s%d%s", PGNSS_NONGLO_DATA_PATH, closestTime,
            ".eph");
        GNSS_LOG_DEBUG("[PGNSS DEBUG] closest eph fileName:%s\n", fileName);
        closedir(dir);
        return ((ret <= 0) ? 0 : 1);
    }
    closedir(dir);
    return 0;
}

errcode_t pgnss_send_nonglo_eph(const char *fileName)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        GNSS_LOG_ERROR("[PGNSS ERROR] open file failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    (void)fseek(fp, 0, SEEK_END);
    int32_t len = ftell(fp);
    (void)fseek(fp, 0, SEEK_SET);
    if (len < 0 || len > PGNSS_FILE_MAX_SIZE) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] get file len failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    uint8_t *buff = (uint8_t *)malloc(len);
    if (buff == NULL) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] pgnss_send_nonglo_eph malloc file len failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    int32_t dataSize = fread(buff, len, 1, fp);
    if (dataSize < 0) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] read file failed: %s\n", fileName);
        free(buff);
        return ERRCODE_FAIL;
    }
    (void)fclose(fp);
    int32_t fileLen = 0;
    for (int32_t i = 0; i < NON_GLO_GNSS_TYPES && fileLen <= len; i++) {
        EphHeadInfo *ephHdr = (EphHeadInfo *)(buff + fileLen);
        GNSS_LOG_DEBUG("[PGNSS DEBUG] nonglo eph, time:%u\n", ephHdr->time);
        (void)encode_and_inject_cmd(ephHdr->cmd, (buff + fileLen + sizeof(EphHeadInfo)), ephHdr->len);
        fileLen += (ephHdr->len + sizeof(EphHeadInfo));
    }
    free(buff);
    return ERRCODE_SUCC;
}

static int32_t format_file_name(char *fileName, char *dName)
{
    int32_t ret = snprintf_s(fileName, FILE_PATH_LEN, (FILE_PATH_LEN - 1), "%s%s", PGNSS_DATA_PATH, dName);
    if (ret <= 0) {
        return 0;
    }
    return 1;
}

int32_t get_glo_eph_file_name(int32_t ts, char *fileName)
{
    const char prefix[] = "GLO";
    DIR *dir = opendir(PGNSS_DATA_PATH);
    if (dir == NULL) {
        GNSS_LOG_ERROR("[PGNSS ERROR] Failed to open /user/gnss/xgnss directory.\n");
        return 0;
    }
    int32_t ephTime, ret;
    struct dirent *entry;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        // 检查文件名
        if (strstr(entry->d_name, ".eph") != NULL) {
            char *p = strrchr(entry->d_name, '_');
            if (p != NULL) {
                char *q = strstr(entry->d_name, prefix);
                if (q != NULL) {
                    ephTime = atoi(p + 1);
                    // 当前时间大于GLO星历文件时间戳,则需要选取相差2小时以内的星历
                    // 当前时间小于GLO星历文件时间戳,则需要选取相差小于15分钟的星历
                    if (((ts - ephTime) >= 0 && abs(ephTime - ts) < OTHER_EPH_VALID_TIME) ||
                        ((ts - ephTime) <= 0 && abs(ephTime - ts) < GLO_EPH_VALID_TIME)) {
                        ret = format_file_name(fileName, entry->d_name);
                        GNSS_LOG_DEBUG("[PGNSS DEBUG] glo eph fileName:%s\n", fileName);
                        closedir(dir);
                        return ret;
                    }
                }
            }
        }
    }
    closedir(dir);
    return 0;
}

errcode_t pgnss_send_glo_eph(const char *fileName, int32_t ts)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        GNSS_LOG_ERROR("[PGNSS ERROR] open file failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    (void)fseek(fp, 0, SEEK_END);
    int32_t len = ftell(fp);
    (void)fseek(fp, 0, SEEK_SET);
    if (len < 0 || len > PGNSS_FILE_MAX_SIZE) {
        fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] get file len failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    uint8_t *buff = (uint8_t *)malloc(len);
    if (buff == NULL) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] malloc file len failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    int32_t dataSize = fread(buff, len, 1, fp);
    if (dataSize < 0) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] pgnss_send_glo_eph read file failed: %s\n", fileName);
        free(buff);
        return ERRCODE_FAIL;
    }
    (void)fclose(fp);
    int32_t fileLen = 0;
    for (int32_t i = 0; i < GLO_EPH_NUM_ONE_FILE && fileLen <= len; i++) {
        EphHeadInfo *ephHdr = (EphHeadInfo *)(buff + fileLen);
        GNSS_LOG_DEBUG("[PGNSS DEBUG] glo eph, time:%u, ts:%d\n", ephHdr->time, ts);
        if (abs((int32_t)ephHdr->time - ts) < GLO_EPH_VALID_TIME) {
            (void)encode_and_inject_cmd(ephHdr->cmd, (buff + fileLen + sizeof(EphHeadInfo)), ephHdr->len);
            break;
        }
        fileLen += (ephHdr->len + sizeof(EphHeadInfo));
    }
    free(buff);
    return ERRCODE_SUCC;
}

static errcode_t pgnss_inject_eph_by_less_files(uint32_t ts)
{
    char fileName[FILE_PATH_LEN] = {0};
    if (get_nonglo_eph_file_name(ts, fileName) == 0) {
        GNSS_LOG_ERROR("[PGNSS ERROR] get_nonglo_eph_file_name failed\n");
        return ERRCODE_FAIL;
    }

    errcode_t ret = pgnss_send_nonglo_eph(fileName);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[PGNSS ERROR] gnss inject pgnss noglo eph fail, error code: %#x\n", ret);
    }

    if (get_glo_eph_file_name(ts, fileName) == 0) {
        GNSS_LOG_ERROR("[PGNSS ERROR] get_glo_eph_file_name failed\n");
        return ERRCODE_FAIL;
    }

    ret = pgnss_send_glo_eph(fileName, ts);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[PGNSS ERROR] gnss inject pgnss glo eph fail, error code: %#x\n", ret);
    }

    return ret;
}

static errcode_t pgnss_send_other_assist_info(const char *fileName)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        GNSS_LOG_ERROR("[PGNSS ERROR] open file failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    (void)fseek(fp, 0, SEEK_END);
    int32_t len = ftell(fp);
    (void)fseek(fp, 0, SEEK_SET);
    if (len < 0 || len > PGNSS_FILE_MAX_SIZE) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] get file len failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    uint8_t *buff = (uint8_t *)malloc(len);
    if (buff == NULL) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] malloc file len failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    int32_t dataSize = fread(buff, len, 1, fp);
    if (dataSize < 0) {
        (void)fclose(fp);
        GNSS_LOG_ERROR("[PGNSS ERROR] read file failed: %s\n", fileName);
        free(buff);
        return ERRCODE_FAIL;
    }
    (void)fclose(fp);
    int32_t fileLen = 0;
    for (int32_t i = 0; i < OTHER_ASSIST_TYPES && fileLen <= len; i++) {
        OtherAssistHeadInfo *otherHdr = (OtherAssistHeadInfo *)(buff + fileLen);
        (void)encode_and_inject_cmd(otherHdr->cmd, (buff + fileLen + sizeof(OtherAssistHeadInfo)), otherHdr->len);
        fileLen += (otherHdr->len + sizeof(OtherAssistHeadInfo));
    }
    free(buff);
    return ERRCODE_SUCC;
}

static errcode_t pgnss_inject_others_by_less_files(void)
{
    errcode_t ret = pgnss_send_other_assist_info(PGNSS_ASSISTINFO_DATA_PATH);
    return ret;
}

static errcode_t gnss_inject_pgnss_by_less_files(uint32_t ts)
{
    errcode_t ret = pgnss_inject_eph_by_less_files(ts);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[PGNSS ERROR] gnss inject pgnss eph fail, error code: %#x\n", ret);
    }

    ret = pgnss_inject_others_by_less_files();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[PGNSS ERROR] gnss inject pgnss other assit fail, error code: %#x\n", ret);
    }
    return ret;
}

errcode_t gnss_pgnss(void)
{
    GNSS_LOG_DEBUG("[PGNSS DEBUG] enter gnss pgnss sample\n");
    GnssUtcTime utcTime;
    errcode_t ret = gnss_inject_time(&utcTime);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[PGNSS ERROR] gnss inject pgnss time fail, error code: %#x\n", ret);
        return ret;
    }

    uint32_t ts;
    if (utc_to_unixtime(utcTime, &ts) == 0) {
        GNSS_LOG_ERROR("[PGNSS ERROR] utc_to_unixtime error\n");
        return ERRCODE_FAIL;
    }
    GNSS_LOG_DEBUG("[PGNSS DEBUG]now utc time: %ld \n", ts);

    return gnss_inject_pgnss_by_less_files(ts);
}