/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss datatypes
 * Author:
 * Create:
 */

#ifndef GNSS_DATATYPES_H
#define GNSS_DATATYPES_H

#include <stdint.h>

#define MSG_CONTENT_IDX             2
#define LISTEN_TIME_THR             1000
#define REPORT_MAX_BYTES            10000
#define GNSS_FACTORY_TEST_COMMAND   0x0004
#define GNSS_FACTORY_TEST_RESULT    0x1007

// 消息类型
typedef enum {
    CMD_INJECT_HIS_INFO = 0x0101,               // 历史信息注入
    CMD_PGNSS_INJECT_REF_GPS_TIME = 0x0201,     // GPS时间注入
    CMD_PGNSS_INJECT_REF_LOC = 0x0202,          // 参考位置注入
    CMD_PGNSS_INJECT_EPH_GPS = 0x0203,          // GPS星历注入
    CMD_PGNSS_INJECT_EPH_GANSS = 0x0204,        // 非GPS星历注入
    CMD_PGNSS_INJECT_UTC_GPS = 0x0205,          // GPS UTC注入
    CMD_PGNSS_INJECT_RTI_GPS = 0x0206,          // GPS完好性信息注入
    CMD_PGNSS_INJECT_RTI_GANSS = 0x0207,        // 非GPS完好性信息注入
    CMD_PGNSS_INJECT_ION_GPS = 0x0208,          // GPS电离层参数注入
    CMD_PGNSS_INJECT_AUX_GANSS = 0x0209,        // 非GPS频点转换关系信息注入

    CMD_AGNSS_INJECT_REF_LOC = 0x0302,         // 位置预测注入
    CMD_AGNSS_INJECT_NAV = 0x0303,             // 星历数据注入
    CMD_AGNSS_INJECT_IONO = 0x0304,            // 电离层参数注入
    CMD_AGNSS_INJECT_UTC = 0x0305,             // UTC时间注入
    CMD_AGNSS_INJECT_RTI = 0x0306,             // 完好性信息注入
    CMD_AGNSS_INJECT_GANSS_ADDL_IONO = 0x0308, // 非GPS电离层参数注入
    CMD_AGNSS_INJECT_GANSS_NAV = 0x0309,       // 非GPS星历数据注入
    CMD_AGNSS_INJECT_GANSS_RTI = 0x030A,       // 非GPS完好性信息注入
    CMD_AGNSS_INJECT_GANSS_AUXI = 0x030C,      // 频点对应关系注入，GLONASS
    CMD_XGNSS_INJECT_UTC_TIME = 0x0404,        // 最开始注入的UTC时间
    CMD_BUTT,
} GnssCmdType;

typedef enum {
    AGNSS_GPS_INJECT_EPH = 0x0002,
    AGNSS_GPS_INJECT_UTC = 0x0003,
    AGNSS_GPS_INJECT_ION = 0x0004,
    AGNSS_GPS_INJECT_RTI = 0x0005,
    AGNSS_GLO_INJECT_EPH = 0x0022,
    AGNSS_GLO_INJECT_RTI = 0x0025,
    AGNSS_GLO_INJECT_AUX = 0x0026,
    AGNSS_BDS_INJECT_EPH = 0x0042,
    AGNSS_BDS_INJECT_ION = 0x0044,
    AGNSS_BDS_INJECT_RTI = 0x0045,
    AGNSS_GAL_INJECT_EPH = 0x0062,
    AGNSS_GAL_INJECT_RTI = 0x0065
} AgnssDataType;

#pragma pack(1)
typedef struct {
    uint16_t  cmd;          // 消息类型
    uint16_t  checkSum;     // 校验和，只计算实际数据部分，不包含头
    uint16_t  sequence;     // 序列号，host下发
    uint16_t  dataLength;   // 数据长度，实际数据的长度，不包含头
    uint8_t   data[0];
} GnssMsg;

typedef struct {
    uint16_t year;          // YYYY
    uint8_t  month;         // MM, 1~12
    uint8_t  day;           // DD, 1~31
    uint8_t  hour;          // hh, 0~23
    uint8_t  minute;        // mm, 0~59
    uint8_t  second;        // ss, 0~59
    uint16_t ms;            // ms, 0~999
    uint8_t  valid;         // 0: invalid, 1: valid
} GnssUtcTime;

typedef struct {
    uint16_t dataType;
    uint16_t len;
    uint8_t data[0];
} AgnssDataHeader;

typedef struct {
    uint8_t checkSum;
} AgnssDataTail;

typedef struct {
    uint32_t time;          // eph reference time
    uint16_t cmd;           // cmd type to injected eph
    uint32_t len;           // actual length of injected eph
} EphHeadInfo;

typedef struct {
    uint16_t cmd;           // cmd type to injected other assist info
    uint32_t len;           // actual length of injected other assist info
} OtherAssistHeadInfo;

typedef struct {
    uint16_t cmd;
    uint16_t status;
} GnssAck;

typedef struct {
    uint32_t test_item;         // must set 0
    uint32_t signal_source;     // 0: monophonic signal(default), 1: gnss signal

    uint32_t satellite_type;    // 0: GPS L1CA(PRN 1~32)
                                // 1: GLONASS L1OF(FN -7~6)
                                // 2: BDS B1I(PRN 6~45)
                                // 3: GAL E1(PRN 1~36)

    int32_t  satellite_svid;    // 0: use default value(PRN 1 for GPS/GAL, PRN 6 for BDS, PFN 0 for GLO)
                                // not 0: user set PRN/FN

    int32_t  satellite_doppler; // doppler of satellite signal, default to set 0, unit: Hz

    uint32_t expected_cn0;      // 0: use default value 400 i.e. 40dBHz
                                // not 0: user set, valid range [390,450], unit dBHz/10

    uint32_t max_cn0_bias;      // 0: use default value 20 i.e. 2dBHz
                                // not 0: user set, valid range [1-100], unit dBHz/10

    uint32_t max_freq_bias;     // 0: use default value 3000
                                // not 0: user set, valid range [1-10000], unit ppb

    uint32_t max_freq_drift;    // 0: use default value 20
                                // not 0: user set, valid range [1-500], unit ppb/s

    uint32_t reserve[11];
} gnss_factory_test_config;     // size: 80 bytes

typedef struct {
    uint32_t test_item;   // should always be 0
    uint32_t test_status; // 0x01: success
                          // 0x02: search satellite failed
                          // 0x04: cn0 failed
                          // 0x08: frequency bias failed
                          // 0x10: frequency drift failed

    int32_t  satellite_cn0; // unit dBHz/10
    int32_t  freq_bias;     // unit ppb
    int32_t  freq_drift;    // unit ppbs/s
    uint32_t reserve[51];
} gnss_factory_test_result; // size: 224 bytes

#pragma pack()

#endif