/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss agnss sample
 * Author:
 * Create:
 */

#include "gnss_agnss.h"
#include <stdint.h>
#include <time.h>
#include "gnss_common_proc.h"
#include "gnss_log.h"

#define GNSS_ENCODE_BUFF_MAX_LEN 9300
#define AGNSS_FILE_MAX_SIZE      1024000
#define AGNSS_DATA_PATH          "/user/gnss/xgnss/AGNSS.dat" // 可自行配置路径

/* AGNSS计算内容校准和, 校验和模256，与芯片侧的命令校验和计算不可混用 */
static uint8_t calc_agnss_check_sum(uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

static errcode_t gnss_inject_agnss_cmd(uint16_t dataType, uint8_t *data, uint16_t dataLen)
{
    /* 1.根据AGNSS dataType类型，转换为芯片侧消息类型 */
    const struct {
        AgnssDataType agnssDataType;    // AGNSS type类型，
        GnssCmdType innerType;            // 芯片侧消息类型
    } table[] = {
        {AGNSS_GPS_INJECT_EPH, CMD_AGNSS_INJECT_NAV},
        {AGNSS_GPS_INJECT_UTC, CMD_AGNSS_INJECT_UTC},
        {AGNSS_GPS_INJECT_ION, CMD_AGNSS_INJECT_IONO},
        {AGNSS_GPS_INJECT_RTI, CMD_AGNSS_INJECT_RTI},
        {AGNSS_GLO_INJECT_EPH, CMD_AGNSS_INJECT_GANSS_NAV},
        {AGNSS_GLO_INJECT_RTI, CMD_AGNSS_INJECT_GANSS_RTI},
        {AGNSS_GLO_INJECT_AUX, CMD_AGNSS_INJECT_GANSS_AUXI},
        {AGNSS_BDS_INJECT_EPH, CMD_AGNSS_INJECT_GANSS_NAV},
        {AGNSS_BDS_INJECT_ION, CMD_AGNSS_INJECT_GANSS_ADDL_IONO},
        {AGNSS_BDS_INJECT_RTI, CMD_AGNSS_INJECT_GANSS_RTI},
        {AGNSS_GAL_INJECT_EPH, CMD_AGNSS_INJECT_GANSS_NAV},
        {AGNSS_GAL_INJECT_RTI, CMD_AGNSS_INJECT_GANSS_RTI}
    };
    uint16_t cmdType = 0;
    errcode_t ret = 1;
    // 2.遍历对应关系表
    for (uint32_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        if (dataType == table[i].agnssDataType) {
            cmdType = (uint16_t)table[i].innerType;
            // 3.找到对应芯片侧消息类型，并下发消息
            ret = encode_and_inject_cmd(cmdType, data, dataLen);
            break;
        }
    }
    return ret;
}

static errcode_t gnss_inject_agnss_all_content(uint8_t *buff, int32_t len)
{
    int32_t remainLen = len;
    // tlv
    while ((uint32_t)remainLen >= sizeof(AgnssDataHeader) + sizeof(AgnssDataTail)) {
        AgnssDataHeader *header = (AgnssDataHeader *)buff;
        uint16_t dataLen = header->len;
        if ((uint32_t)remainLen < sizeof(AgnssDataHeader) + sizeof(AgnssDataTail) + dataLen) {
            GNSS_LOG_ERROR("[GNSS ERROR]agnss remainLen %d and dataLen %d not match\n", remainLen, dataLen);
            return ERRCODE_FAIL;
        }
        uint8_t localCheckSum = calc_agnss_check_sum((uint8_t *)header, (header->data - (uint8_t *)header) + dataLen);
        uint8_t checkSum = ((AgnssDataTail *)(header->data + dataLen))->checkSum;
        if (localCheckSum != checkSum) {
            GNSS_LOG_ERROR("[GNSS ERROR]agnss local CheckSum %d and CheckSum %d not match\n",
                localCheckSum, checkSum);
            return ERRCODE_FAIL;
        }
        errcode_t ret = gnss_inject_agnss_cmd(header->dataType, header->data, dataLen);
        // 文件中存在不支持cmd时，ret = 1
        if (ret != ERRCODE_SUCC && ret != 1) {
            GNSS_LOG_ERROR("[GNSS ERROR]gnss inject fail, error code: %#x, cmdType: %d\n", ret, header->dataType);
        }
        buff += sizeof(AgnssDataHeader) + sizeof(AgnssDataTail) + dataLen;
        remainLen -= sizeof(AgnssDataHeader) + sizeof(AgnssDataTail) + dataLen;
    }
    return ERRCODE_SUCC;
}

static errcode_t gnss_inject_agnss_data(void)
{
    char fileName[] = AGNSS_DATA_PATH;
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: open file failed: %s\n", fileName);
        return ERRCODE_FAIL;
    }
    (void)fseek(fp, 0, SEEK_END);
    int32_t len = ftell(fp);
    (void)fseek(fp, 0, SEEK_SET);
    if (len < 0 || len > AGNSS_FILE_MAX_SIZE) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: get file len failed: %s\n", fileName);
        (void)fclose(fp);
        return ERRCODE_FAIL;
    }
    uint8_t *buff = (uint8_t *)malloc(len);
    if (buff == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: malloc file len failed: %s\n", fileName);
        (void)fclose(fp);
        return ERRCODE_FAIL;
    }
    int32_t dataSize = fread(buff, len, 1, fp);
    if (dataSize < 0) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: read file failed: %s\n", fileName);
        (void)fclose(fp);
        free(buff);
        return ERRCODE_FAIL;
    }

    errcode_t ret = gnss_inject_agnss_all_content(buff, len);
    (void)fclose(fp);
    free(buff);
    return ret;
}

static errcode_t gnss_inject_agnss(void)
{
    GnssUtcTime utcTime;
    errcode_t ret = gnss_inject_time(&utcTime);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss inject agnss time fail, error code: %#x\n", ret);
        return ret;
    }
    ret = gnss_inject_agnss_data();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss inject agnss assit data fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

errcode_t gnss_agnss(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]enter gnss agnss sample\n");
    return gnss_inject_agnss();
}