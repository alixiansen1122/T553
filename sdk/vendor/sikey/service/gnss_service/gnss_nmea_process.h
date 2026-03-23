#ifndef GNSS_NMEA_PROCESS_H
#define GNSS_NMEA_PROCESS_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define GNSS_PRN_OFFSET_GPS 0
#define GNSS_PRN_OFFSET_GLO 0
#define GNSS_PRN_OFFSET_GAL 100
#define GNSS_PRN_OFFSET_BDS 150
#define GNSS_PRN_OFFSET_QZS 220

#define WATCH_GPS_CN_NUM 255

typedef enum
{
    GPS_CMD_UNKNOW = 0,
    GPS_CMD_GPRMC,
    GPS_CMD_GPGGA,
    GPS_CMD_GPGSV,
    GPS_CMD_GPGSA,
    GPS_CMD_GPTXT,
    GPS_CMD_GBGSV,
    GPS_CMD_GAGSV,
    GPS_CMD_GQGSV,
    GPS_CMD_GLGSV,
    GPS_CMD_BDGSV,
    GPS_CMD_FCTAINFO,
    GPS_CMD_ACK
} GPS_CMD_ID_E;

typedef enum
{
    GNSS_TYPE_GPS = 0,
    GNSS_TYPE_BDS,
    GNSS_TYPE_GLO,
    GNSS_TYPE_GAL,
    GNSS_TYPE_QZS,
    GNSS_TYPE_MAX
} GNSS_TYPE_E;

typedef enum
{
    RECEIVE_CMD_MODE = 0,
    RECEIVE_DATA_MODE,
    RECEIVE_END_MODE,
} RECEIVE_MODE_E;

#pragma pack(1)

typedef struct _GPS_DATA_INFO_T
{
    char fixed_status;
    char Longitude[11];
    char Longitude_Dir;
    char Latitude[10];
    char Latitude_Dir;
    char Satellites_Used[3];
    char Satellites_in_View[3];
    char Satellites_list[4][4];
    char Satellites_snr_table[4][3];
    char Speed[6];
    char Speed_num;
    char Angle[6];
    char Angle_num;
    char utc_time[7];
    char utc_date[7];
    char HDOP[5];
    char PDOP[5];
    char VDOP[5];
    uint32_t all_satellites_view;
    uint32_t all_satellites_use;
    uint8_t gps_cn_buf[WATCH_GPS_CN_NUM];
} gnss_parse_info_t;

typedef struct
{
    char Satellites_in_View[3];
    char Satellites_list[4][4];
    char Satellites_snr_table[4][3];
} COMMON_SNR_INFO_T;

typedef struct gnss_location_info
{
    char utc_date[7];
    char status;
    char latitude[10];
    char latitude_dir;
    char longitude[11];
    char longitude_dir;
    char speed[8];
    char utc_time[7];
    char angle[8];
    char satellites_view[3];
    char satellites_use[3];
    char hdop[5];
    char pdop[5];
    char vdop[5];
} gnss_location_info_t;

#pragma pack()
void gnss_process_nmea(uint8_t *gnssMsgBuff);
void gnss_info_init(void);
#endif
