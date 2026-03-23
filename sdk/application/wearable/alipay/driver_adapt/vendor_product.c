/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay vendor product adapt.
 * Author:
 * Create:
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "iotsec.h"
#include "systick.h"
#include "sys/time.h"
#include "time.h"
#include "common_def.h"
#include "bts_le_gap.h"
#include "alipay_feature.h"

extern void Ble_SendDataSimple(uint16_t handle, uint8_t *data, uint16_t len);
extern uint32_t Unix_time;

#define ALIPAY_BLE_MAC_ADDR_NUM 6
#define ALIPAY_ADAPT_LOG_MAX_NUM 300

/* ○ 功能描述
    ■
  获取设备ID号(以冒号分割的16进制mac地址)，要求内容以‘\0’结尾且长度不包含'\0'。所有字母大写，长度为17。例如：“AA:BB:CC:00:11:22”
  ○ 接口参数
    ■ buf_did - 存放设备ID数据地址
    ■ len_did - 存放设备ID长度地址
  ○ 返回值
    ■ 0表示成功，非0表示失败 */
csi_error_t csi_get_deviceid(uint8_t *buf_did, uint32_t *len_did)
{
    if (buf_did == NULL || len_did == NULL) {
        return CSI_ERROR;
    }

    bd_addr_t addr_info;
    gap_ble_get_local_addr(&addr_info);
    char str[18] = {0};
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", addr_info.addr[5], addr_info.addr[4], addr_info.addr[3],
        addr_info.addr[2], addr_info.addr[1], addr_info.addr[0]);
#ifdef ALIPAY_CSI_LOG_EXT_ON
    MyPrintf("addr:%s\n", str);
#endif
    *len_did = strlen(str);
    memcpy(buf_did, str, strlen(str));
    return CSI_OK;
}

/* ○ 功能描述
    ■ 获取当前系统时间戳（Unix时间戳格式）
  ○ 接口参数
    ■ tm - 存放系统时间戳的变量地址
  ○ 返回值
    ■ 0表示成功，非0表示失败 */
csi_error_t csi_get_timestamp(uint32_t *tm)
{
    if (tm == NULL) {
        return CSI_ERROR;
    }
    struct timeval tv = { 0 };

    if (gettimeofday(&tv, NULL)) {
        return CSI_ERROR;
    }
    *tm = (uint32_t)(tv.tv_sec);
    MyPrintf("[timestamp:%u\n", (uint32_t)(tv.tv_sec));
    return CSI_OK;
}

/* ○ description:
    ■ get compile timestamp
  ○ param
  ○ return
    ■ compile timestamp */
csi_error_t csi_get_compile_timestamp(uint32_t *timestamp)
{
    if (timestamp == NULL) {
        return CSI_ERROR;
    }

    *timestamp = 0;
    return CSI_OK;
}

/* ○ 功能描述
    ■
  获取设备SN(厂商印刷在卡片上的设备序列号)，长度不超过32个字符，只能包含大小写字母、数字、下划线。仅卡片类产品且有SN在小程序显示需求的厂商实现，其他厂商请输出""(空字符串)，len_sn=0
  ○ 接口参数
    ■ buf_sn - 存放设备SN数据地址
    ■ len_sn - 存放设备SN长度地址
  ○ 返回值
    ■ 0表示成功，非0表示失败 */
csi_error_t csi_get_sn(uint8_t *buf_sn, uint32_t *len_sn)
{
    if (buf_sn == NULL || len_sn == NULL) {
        return CSI_ERROR;
    }

    bd_addr_t addr_info;
    gap_ble_get_local_addr(&addr_info);
    char str[18] = {0};
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", addr_info.addr[5], addr_info.addr[4], addr_info.addr[3],
        addr_info.addr[2], addr_info.addr[1], addr_info.addr[0]);
#ifdef ALIPAY_CSI_LOG_EXT_ON
    MyPrintf("addr:%s\n", str);
#endif
    *len_sn = strlen(str);
    memcpy(buf_sn, str, strlen(str));

    return CSI_OK;
}

/* ○ 功能描述
    ■ 获取设备company name
  ○ 接口参数
    ■ buffer - 存放设备company name数据地址
    ■ len - 存放设备company name长度地址
  ○ 返回值
    ■ 0表示成功，非0表示失败 */
csi_error_t csi_get_companyname(uint8_t *buffer, uint32_t *len)
{
    const char *mock_company = "SPARTA";
    *len = strlen(mock_company);
    memcpy(buffer, mock_company, strlen(mock_company));

    return CSI_OK;
}

/* ○ 功能描述
    ■ 获取设备通讯协议类型
  ○ 接口参数
    ■ ptype - 存放设备通讯协议类型变量地址
  ○ 返回值
    ■ 0表示成功，非0表示失败 */
csi_error_t csi_get_protoctype(uint32_t *ptype)
{
    *ptype = 0;
    return CSI_OK;
}

/* ○ 功能描述
    ■ 发送蓝牙数据
  ○ 接口参数
    ■ data - 存放发送数据地址
    ■ len - 存放发送数据长度(len<=20)
  ○ 返回值
    ■ 0表示成功，非0表示失败 */
csi_error_t csi_ble_write(uint8_t *data, uint16_t len)
{
#ifdef ALIPAY_CSI_LOG_EXT_ON
    MyPrintf("csi_ble_write in %s,len:%u.", data, len);
#endif
    svr_alipay_bt_gatts_noyify(data, len);

    return CSI_OK;
}

/* ○ 功能描述
    ■ 打印日志信息
  ○ 接口参数
    ■ level - 日志调试打印等级
    ■ format - 格式化输出字符串
    ■ value - 输出数据
  ○ 返回值
    ■ 无 */
void csi_log(int level, const char *format, int32_t value)
{
    MyPrintf("[level%d %d] %s\n", level, value, format);
}

/* ○ 功能描述
    ■ 打印日志信息
  ○ 接口参数
    ■ format - 格式化输出字符串
    ■ ... - 可变参数
  ○ 返回值
    ■ 无 */
void csi_log_ext(const char *format, va_list *val_list)
{
#ifdef ALIPAY_CSI_LOG_EXT_ON
    static char log_buf[ALIPAY_ADAPT_LOG_MAX_NUM] = {0};
    vsnprintf(log_buf, ALIPAY_ADAPT_LOG_MAX_NUM, format, *val_list);
    MyPrintf("%s", log_buf);
#else
    unused(format);
    unused(val_list);
#endif
}

csi_error_t csi_get_productmodel(uint8_t *buffer, uint32_t *len)
{
    const char mock_company[] = "WATCH_BRANDY";
    *len = strlen(mock_company);
    memcpy(buffer, mock_company, strlen(mock_company));

    return CSI_OK;
}

void *csi_malloc(uint32_t size)
{
    return malloc(size);
}

void *csi_calloc(uint32_t nblock, uint32_t size)
{
    return calloc(nblock, size);
}

void csi_free(void *pt)
{
    free(pt);
}

void *csi_realloc(void *pt, uint32_t size)
{
    return realloc(pt, size);
}
