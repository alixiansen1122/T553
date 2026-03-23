/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: nmea pares
 * Create: 2024-11-11
 */

#include "gnss_nmea_parse.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <ctype.h>
#include <string.h>
#include "securec.h"
#include "gnss_nmea_str_format.h"
#include "location_log.h"

#define NMEA_MODULE_NAME "NMEA"

static void NmeaGetSatelliteStatusInfo(GnssSatelliteStatusInfo **gnssSatInfo, GnssSystem gnssSystem, GnssInfo *gnssInfo)
{
    if (gnssSystem == GNSS_SYSTEM_GPS) {
        *gnssSatInfo = &gnssInfo->satelist.gps;
    } else if (gnssSystem == GNSS_SYSTEM_GLONASS) {
        *gnssSatInfo = &gnssInfo->satelist.glonass;
    } else if (gnssSystem == GNSS_SYSTEM_BDS) {
        *gnssSatInfo = &gnssInfo->satelist.bds;
    } else if (gnssSystem == GNSS_SYSTEM_QZSS) {
        *gnssSatInfo = &gnssInfo->satelist.qzss;
    } else {
        *gnssSatInfo = &gnssInfo->satelist.galileo;
    }
    return;
}

static int NmeaGSV2GnssInfo(const GnssNmeaGSV *nmeaGSV, GnssSystem gnssSystem, GnssInfo *gnssInfo)
{
    unsigned int msgIdx;
    unsigned int satIdx;
    unsigned int satOffset;
    unsigned int satNum;
    GnssSatelliteStatusInfo *gnssSatInfo = NULL;

    if (nmeaGSV == NULL || gnssInfo == NULL) {
        return GNSS_ERR;
    }

    NmeaGetSatelliteStatusInfo(&gnssSatInfo, gnssSystem, gnssInfo);

    msgIdx = nmeaGSV->msgIdx;
    if (msgIdx == 0) {
        msgIdx = 1;
    }

    if (msgIdx > nmeaGSV->msgCnt) {
        msgIdx = nmeaGSV->msgCnt;
    }

    if ((msgIdx * GNSS_SAT_IN_MSG) > GNSS_SAT_NUM_MAX) {
        msgIdx = GNSS_NSAT_MSGS;
    }

    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SMASK;
    gnssInfo->field |= nmeaGSV->fieldType;
    gnssInfo->sentence |= GNSS_NMEA_FRAME_GSV;

    if (nmeaGSV->fieldType & GNSS_NMEA_FIELD_TYPE_SATINVIEW) {
        satOffset = (msgIdx - 1) * GNSS_SAT_IN_MSG;
        satNum = ((satOffset + GNSS_SAT_IN_MSG) > nmeaGSV->satViewedCnt) ? (nmeaGSV->satViewedCnt - satOffset) :
                                                                           GNSS_SAT_IN_MSG;

        if ((satNum > GNSS_SAT_IN_MSG) || ((satOffset + satNum) > GNSS_SAT_NUM_MAX)) {
            return GNSS_ERR;
        }

        for (satIdx = 0; satIdx < satNum; satIdx++) {
            gnssSatInfo->satelliteStatus[satOffset + satIdx].svid = nmeaGSV->satelliteStatus[satIdx].svid;
            gnssSatInfo->satelliteStatus[satOffset + satIdx].ele = nmeaGSV->satelliteStatus[satIdx].ele;
            gnssSatInfo->satelliteStatus[satOffset + satIdx].az = nmeaGSV->satelliteStatus[satIdx].az;
            gnssSatInfo->satelliteStatus[satOffset + satIdx].cnr = nmeaGSV->satelliteStatus[satIdx].cnr;
        }

        gnssSatInfo->satViewedCnt = nmeaGSV->satViewedCnt;
    }

    return GNSS_OK;
}

static int NmeaGSA2GnssInfo(const GnssNmeaGSA *nmeaGSA, GnssSystem gnssSystem, GnssInfo *gnssInfo)
{
    int i;
    GnssSatelliteStatusInfo *gnssSatInfo = NULL;
    if (nmeaGSA == NULL || gnssInfo == NULL) {
        return GNSS_ERR;
    }
    if (gnssSystem == GNSS_SYSTEM_GPS) {
        gnssSatInfo = &gnssInfo->satelist.gps;
    } else if (gnssSystem == GNSS_SYSTEM_GLONASS) {
        gnssSatInfo = &gnssInfo->satelist.glonass;
    } else if (gnssSystem == GNSS_SYSTEM_BDS) {
        gnssSatInfo = &gnssInfo->satelist.bds;
    } else {
        gnssSatInfo = &gnssInfo->satelist.galileo;
    }

    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SMASK;
    gnssInfo->field |= nmeaGSA->fieldType;
    gnssInfo->sentence |= GNSS_NMEA_FRAME_GSA;

    if (nmeaGSA->fieldType & GNSS_NMEA_FIELD_TYPE_OPERMODE) {
        gnssInfo->smode = nmeaGSA->operType;
    }

    if (nmeaGSA->fieldType & GNSS_NMEA_FIELD_TYPE_SATINUSE) {
        gnssSatInfo->satUsedCnt = 0;
        for (i = 0; i < GNSS_SAT_NUM_MAX; i++) {
            gnssSatInfo->satUsedID[i] = nmeaGSA->satPrn[i];
            if (nmeaGSA->satPrn[i] != 0) {
                gnssSatInfo->satUsedCnt++;
            }
        }
        gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SATUSEDNUM;
    }

    if (nmeaGSA->fieldType & GNSS_NMEA_FIELD_TYPE_PDOP) {
        gnssInfo->pdop = nmeaGSA->pdop;
    }

    if (nmeaGSA->fieldType & GNSS_NMEA_FIELD_TYPE_HDOP) {
        gnssInfo->hdop = nmeaGSA->hdop;
        gnssInfo->horizontalAccuracy = gnssInfo->hdop * NMEA_ERROR;  // 估算水平误差
        gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_HORIZONTAL_ACCURACY;
    }

    if (nmeaGSA->fieldType & GNSS_NMEA_FIELD_TYPE_VDOP) {
        gnssInfo->vdop = nmeaGSA->vdop;
        gnssInfo->verticalAccuracy = gnssInfo->vdop * NMEA_ERROR;  // 估算垂直误差
        gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_VERTICAL_ACCURACY;
    }
    return GNSS_OK;
}

static int NmeaVTG2GnssInfo(const GnssNmeaVTG *nmeaVTG, GnssInfo *gnssInfo)
{
    if (nmeaVTG == NULL || gnssInfo == NULL) {
        return GNSS_ERR;
    }

    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SMASK;
    gnssInfo->field |= nmeaVTG->fieldType;
    gnssInfo->sentence |= GNSS_NMEA_FRAME_VTG;

    if (nmeaVTG->fieldType & GNSS_NMEA_FIELD_TYPE_TRACK) {
        gnssInfo->trackAng = nmeaVTG->trackAng;
    }

    if (nmeaVTG->fieldType & GNSS_NMEA_FIELD_TYPE_MTRACK) {
        gnssInfo->magneticTrackAng = nmeaVTG->magneticTrackAng;
    }
    return GNSS_OK;
}

static void NmeaRMCUTC2GnssInfo(const GnssNmeaRMC *nmeaRMC, GnssInfo *gnssInfo)
{
    if (nmeaRMC == NULL || gnssInfo == NULL) {
        return;
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_UTCDATE) {
        gnssInfo->utc.year = nmeaRMC->utc.year;
        gnssInfo->utc.month = nmeaRMC->utc.month;
        gnssInfo->utc.day = nmeaRMC->utc.day;
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_UTCTIME) {
        gnssInfo->utc.hour = nmeaRMC->utc.hour;
        gnssInfo->utc.min = nmeaRMC->utc.min;
        gnssInfo->utc.sec = nmeaRMC->utc.sec;
        gnssInfo->utc.centisec = nmeaRMC->utc.centisec;
    }
    if ((nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_UTCDATE) &&
        (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_UTCTIME)) {
        gnssInfo->timevalid = 1;
    }
    return;
}

#define NMEA_SPEEDS_SAMPLE_CNT (100)
float g_speeds[NMEA_SPEEDS_SAMPLE_CNT];         // 存储速度样本
int32_t g_sampleCnt = 0;
bool g_isWriteinPlace = false;

// 计算 RMSE
static float NmeaComputeRmse(int32_t sampleCnt)
{
    if (sampleCnt == 0) {
        return 0.0f;
    }

    float sum = 0.0, mean = 0.0, errorSum = 0.0;
    for (int32_t i = 0; i < sampleCnt; i++) {
        sum += g_speeds[i];
    }
    mean = sum / sampleCnt;

    for (int32_t i = 0; i < sampleCnt; i++) {
        float error = g_speeds[i] - mean;
        errorSum += error * error;
    }

    return sqrtf(errorSum / sampleCnt);
}

static int NmeaRMC2GnssInfo(GnssNmeaRMC *nmeaRMC, GnssInfo *gnssInfo)
{
    if (nmeaRMC == NULL || gnssInfo == NULL) {
        return GNSS_ERR;
    }

    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SMASK;
    gnssInfo->field |= nmeaRMC->fieldType;

    gnssInfo->sentence |= GNSS_NMEA_FRAME_RMC;

    NmeaRMCUTC2GnssInfo(nmeaRMC, gnssInfo);

    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SIGNAL;
    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_OPERMODE;

    if (nmeaRMC->curStat == 'A') {
        if (gnssInfo->signal == GNSS_SIG_BAD) {
            gnssInfo->signal = GNSS_SIG_MID;
        }

        if (gnssInfo->smode == GNSS_OPER_BAD) {
            gnssInfo->smode = GNSS_OPER_2D;
        }
    } else {
        gnssInfo->signal = GNSS_SIG_BAD;
        gnssInfo->smode = GNSS_OPER_BAD;
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_LAT) {
        gnssInfo->lat = (GNSS_DIR_NORTH == nmeaRMC->nSouth ? nmeaRMC->latitude : -nmeaRMC->latitude);
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_LON) {
        gnssInfo->lon = (GNSS_DIR_EAST == nmeaRMC->eWest ? nmeaRMC->longitude : -nmeaRMC->longitude);
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_SPEED) {
        // knots to cm/s
        gnssInfo->speed = nmeaRMC->speedN * GNSS_KNOTS;
        if (g_sampleCnt == NMEA_SPEEDS_SAMPLE_CNT) {
            g_sampleCnt = 0;
            g_isWriteinPlace = true;
        }
        g_speeds[g_sampleCnt++] = gnssInfo->speed;
        gnssInfo->speedAccuracy = NmeaComputeRmse(g_isWriteinPlace ? NMEA_SPEEDS_SAMPLE_CNT : g_sampleCnt);
        gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SPEED_ACCURACY;
        
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_TRACK) {
        gnssInfo->trackAng = nmeaRMC->trackAng;
        if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_SPEED) {
            gnssInfo->velocityNorth = gnssInfo->speed * cos(gnssInfo->trackAng * M_PI / GNSS_STRAIGHT_ANGLE);
            gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_NORTH_VELOCITY;
            gnssInfo->velocityEast = gnssInfo->speed * sin(gnssInfo->trackAng * M_PI / GNSS_STRAIGHT_ANGLE);
            gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_EAST_VELOCITY;
        }
    }

    if (nmeaRMC->fieldType & GNSS_NMEA_FIELD_TYPE_MAGVAR) {
        gnssInfo->magneticTrackAng =
            (GNSS_DIR_EAST == nmeaRMC->magVarEW ? nmeaRMC->magVariation : -nmeaRMC->magVariation);
    }
    return GNSS_OK;
}

#define NMEA_HOUR_TO_SEC (3600)
#define NMEA_MIN_TO_SEC (60)
#define NMEA_12_HOUR_TO_SEC (43200)
#define NMEA_24_HOUR_TO_SEC (86400)
// Store previous altitude and time for velocity calculation
// 用于存储前一时刻的高度和时间，用于计算速度
static double g_previousAltitude = 0.0;
static double g_previousTime = 0.0;

static void CalculateVelocityDescend(const GnssNmeaGGA *nmeaGGA, GnssInfo *gnssInfo)
{
    // 计算下降速度 (需要上一高度和时间)
    // Calculate descent velocity (needs previous altitude and time)
    if (g_previousTime > 0.0) {
        double currentTime =
            gnssInfo->utc.hour * NMEA_HOUR_TO_SEC + gnssInfo->utc.min * NMEA_MIN_TO_SEC + gnssInfo->utc.sec;
        double deltaTime = currentTime - g_previousTime;

        // 处理跨天情况
        // Handle day crossover
        if (deltaTime < -NMEA_12_HOUR_TO_SEC) {        // 如果时间差小于-12小时，说明跨天了
                                         // If time difference is less than -12 hours, day has changed
            deltaTime += NMEA_24_HOUR_TO_SEC;          // 加上24小时
                                         // Add 24 hours
        } else if (deltaTime > NMEA_12_HOUR_TO_SEC) {  // 如果时间差大于12小时，说明是前一天的数据
                                         // If time difference is more than 12 hours, it's previous day's data
            deltaTime -= NMEA_24_HOUR_TO_SEC;
        }

        if (deltaTime > 0 && deltaTime < NMEA_TEN) {  // 只处理合理的时间差（比如小于10秒）
                                                // Only process reasonable time differences (e.g., less than 10 seconds)
            double deltaAltitude = gnssInfo->altitude - g_previousAltitude;
            // 过滤异常值（比如高度差太大）
            // Filter abnormal values (e.g., too large altitude differences)
            if (fabs(deltaAltitude) < NMEA_NUM_100) {  // 假设最大垂直速度不超过100m/s
                                              // Assume maximum vertical speed doesn't exceed 100m/s
                gnssInfo->velocityDescend =
                    -deltaAltitude / deltaTime;  // 注意符号：上升为负，下降为正
                                                 // Note: negative for ascent, positive for descent
                gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_DESCEND_VELOCITY;
            }
        }
    }
    g_previousAltitude = gnssInfo->altitude;
    g_previousTime = gnssInfo->utc.hour * NMEA_HOUR_TO_SEC + gnssInfo->utc.min * NMEA_MIN_TO_SEC + gnssInfo->utc.sec;
}

static int NmeaGGA2GnssInfo(const GnssNmeaGGA *nmeaGGA, GnssInfo *gnssInfo)
{
    if (nmeaGGA == NULL || gnssInfo == NULL) {
        return GNSS_ERR;
    }

    gnssInfo->field |= GNSS_NMEA_FIELD_TYPE_SMASK;
    gnssInfo->field |= nmeaGGA->fieldType;
    gnssInfo->sentence |= GNSS_NMEA_FRAME_GGA;

    if (nmeaGGA->fieldType & GNSS_NMEA_FIELD_TYPE_UTCTIME) {
        gnssInfo->utc.hour = nmeaGGA->utc.hour;
        gnssInfo->utc.min = nmeaGGA->utc.min;
        gnssInfo->utc.sec = nmeaGGA->utc.sec;
        gnssInfo->utc.centisec = nmeaGGA->utc.centisec;
    }

    if (nmeaGGA->fieldType & GNSS_NMEA_FIELD_TYPE_LAT) {
        gnssInfo->lat = (GNSS_DIR_NORTH == nmeaGGA->nSouth ? nmeaGGA->latitude : -nmeaGGA->latitude);
    }

    if (nmeaGGA->fieldType & GNSS_NMEA_FIELD_TYPE_LON) {
        gnssInfo->lon = (GNSS_DIR_EAST == nmeaGGA->eWest ? nmeaGGA->longitude : -nmeaGGA->longitude);
    }

    if (nmeaGGA->fieldType & GNSS_NMEA_FIELD_TYPE_SIGNAL) {
        gnssInfo->signal = (unsigned char)nmeaGGA->signal;
    }

    if (nmeaGGA->fieldType & GNSS_NMEA_FIELD_TYPE_HDOP) {
        gnssInfo->hdop = nmeaGGA->hdop;
    }

    if (nmeaGGA->fieldType & GNSS_NMEA_FIELD_TYPE_ANTALT) {
        gnssInfo->altitude = nmeaGGA->antAltitude;
        CalculateVelocityDescend(nmeaGGA, gnssInfo);
    }
    return GNSS_OK;
}

static int NmeaIsIlleageMode(const char *mode)
{
    if (*mode != 'A' && *mode != 'D' && *mode != 'E' && *mode != 'F' &&
        *mode != 'M' && *mode != 'N' && *mode != 'P' && *mode != 'R') {
        return true;
    } else {
        return false;
    }
}

static int NmeaProcessTime(const char *timeBuf, unsigned int len, GnssTime *utcTime)
{
    int ret;
    int time[TIME_BIT_NUM_MAX] = {0};

    if (timeBuf == NULL || utcTime == NULL) {
        return GNSS_ERR;
    }

    if (strlen("HHMMSS") == len) {
        utcTime->centisec = 0;
        ret = sscanf_s(timeBuf,
                       "%2d%2d%2d",
                       &time[TIME_BIT_NUM_0],
                       &time[TIME_BIT_NUM_1],
                       &time[TIME_BIT_NUM_2]);
        if (ret != TIME_BIT_NUM_3) {
            return GNSS_ERR;
        }
    } else if (strlen("HHMMSS.S") == len || strlen("HHMMSS.SS") == len || strlen("HHMMSS.SSS") == len) {
        ret = sscanf_s(timeBuf,
                       "%2d%2d%2d.%d",
                       &time[TIME_BIT_NUM_0], &time[TIME_BIT_NUM_1],
                       &time[TIME_BIT_NUM_2], &time[TIME_BIT_NUM_3]);
        if (ret != TIME_BIT_NUM_MAX) {
            return GNSS_ERR;
        }

        if (strlen("HHMMSS.S") == len) {
            time[TIME_BIT_NUM_3] *= NMEA_TEN;
        } else if (strlen("HHMMSS.SSS") == len) {
            time[TIME_BIT_NUM_3] = (time[TIME_BIT_NUM_3] + NMEA_NINE) / NMEA_TEN;
        }
    } else {
        return GNSS_ERR;
    }
    utcTime->hour = (unsigned char)time[TIME_BIT_NUM_0];
    utcTime->min = (unsigned char)time[TIME_BIT_NUM_1];
    utcTime->sec = (unsigned char)time[TIME_BIT_NUM_2];
    utcTime->centisec = (unsigned char)time[TIME_BIT_NUM_3];

    return GNSS_OK;
}

static bool NmeaIsIlleageDate(const GnssTime *utcTime)
{
    if (utcTime->year < NMEA_YEAR_MIN ||
        utcTime->year > NMEA_YEAR_MAX ||
        utcTime->month > NMEA_MONTH_MAX ||
        utcTime->day > NMEA_DAY_MAX) {
        return true;
    } else {
        return false;
    }
}

static void NmeaProcessDate(unsigned int date, GnssTime *utcTime)
{
    utcTime->day = (unsigned char)(date / NMEA_NUM_10000);
    utcTime->month = (unsigned char)((date / NMEA_NUM_100) % NMEA_NUM_100);
    utcTime->year = (unsigned short)(date % NMEA_NUM_100);
    if (utcTime->year < NMEA_NUM_90) {
        utcTime->year += NUMEA_YEAR_2000;
    } else if (utcTime->year > NMEA_NUM_90) {
        utcTime->year += NUMEA_YEAR_1900;
    }
    return;
}

static bool NmeaIsIlleageTime(const GnssTime *utcTime)
{
    if (utcTime == NULL) {
        return true;
    }

    if (utcTime->hour >= NMEA_HOUR_MAX || utcTime->min >= NMEA_MIN_MAX ||
        utcTime->sec >= NMEA_SEC_MAX || utcTime->centisec > NMEA_NUM_100) {
        return true;
    } else {
        return false;
    }
}

static int NmeaIsNan(double dNumber)
{
    return isnan(dNumber);
}

static bool NmeaIsIlleageDirection(const char *direction, unsigned int type)
{
    if (type == GNSS_DIR_NSOUTH) {
        if ((*direction != 'N') && (*direction != 'S')) {
            return true;
        }
    } else {
        if ((*direction != 'W') && (*direction != 'E')) {
            return true;
        }
    }

    return false;
}

static void NmeaGGAInit(GnssNmeaGGA *gga)
{
    if (gga == NULL) {
        return;
    }

    gga->latitude = NAN;
    gga->longitude = NAN;
    gga->hdop = NAN;
    gga->antAltitude = NAN;
    gga->signal = 0xFFFF;
    gga->satUsedCnt = 0xFFFF;

    return;
}

static int NmeaGGACheckInvalid(GnssNmeaGGA *gga)
{
    if (NmeaIsIlleageTime(&gga->utc) == true) {
        return GNSS_ERR;
    }

    gga->fieldType |= GNSS_NMEA_FIELD_TYPE_UTCTIME;

    if (!NmeaIsNan(gga->latitude) && gga->nSouth != 0) {
        if (NmeaIsIlleageDirection((const char *)&gga->nSouth, GNSS_DIR_NSOUTH) == true) {
            return GNSS_ERR;
        }
        gga->fieldType |= GNSS_NMEA_FIELD_TYPE_LAT;
    }

    if (!NmeaIsNan(gga->longitude) && gga->eWest != 0) {
        if (NmeaIsIlleageDirection((const char *)&gga->eWest, GNSS_DIR_EWEST) == true) {
            return GNSS_ERR;
        }
        gga->fieldType |= GNSS_NMEA_FIELD_TYPE_LON;
    }

    if (gga->signal > GNSS_SIG_SIM) {
        return GNSS_ERR;
    }

    gga->fieldType |= GNSS_NMEA_FIELD_TYPE_SIGNAL;

    if (gga->satUsedCnt > GNSS_SAT_NUM_MAX) {
        return GNSS_ERR;
    }

    gga->fieldType |= GNSS_NMEA_FIELD_TYPE_SATUSEDNUM;

    if (!NmeaIsNan(gga->hdop)) {
        gga->fieldType |= GNSS_NMEA_FIELD_TYPE_HDOP;
    }

    if (!NmeaIsNan(gga->antAltitude) && gga->antAltUnit != 0) {
        if (gga->antAltUnit != 'M') {
            return GNSS_ERR;
        }
        gga->fieldType |= GNSS_NMEA_FIELD_TYPE_ANTALT;
    }
    return GNSS_OK;
}

static int NmeaProcessGGA(const char *nmeaBuf, unsigned int len, GnssNmeaGGA *gga)
{
    int ret;
    int cnts;
    char timebuf[NMEA_FIELD_TIME_SIZE] = {0};
    char formatStr[] = "GGA,%s,%f,%c,%f,%c,%d,%d,%f,%f,%c,%f,%c,%f,%d*";

    if (nmeaBuf == NULL || gga == NULL) {
        return GNSS_ERR;
    }

    NmeaGGAInit(gga);

    cnts = GnssNmeaFormat(nmeaBuf + NMEA_SENTENCE_START_BIT,
        len - NMEA_SENTENCE_START_BIT,
        formatStr,
        timebuf,
        &gga->latitude,
        &gga->nSouth,
        &gga->longitude,
        &gga->eWest,
        &gga->signal,
        &gga->satUsedCnt,
        &gga->hdop,
        &gga->antAltitude,
        &gga->antAltUnit,
        &gga->geoidalSep,
        &gga->geoidalSepUnit,
        &gga->dGPSTimeSinceLast,
        &gga->dGPSId);
    if (cnts != GNSS_FILEDS_IN_GGA) {
        return GNSS_ERR;
    }
    LBSLOGD(HDI_GNSS,
        "GGA latitude :%f nSouth:%c longitude:%f eWest:%c signal:%u satUsedCnt:%u hdop:%f antAltitude:%f antAltUnit:%c "
        "geoidalSep:%f geoidalSepUnit:%c dGPSTimeSinceLast:%f dGPSId:%u",
        gga->latitude,
        gga->nSouth,
        gga->longitude,
        gga->eWest,
        gga->signal,
        gga->satUsedCnt,
        gga->hdop,
        gga->antAltitude,
        gga->antAltUnit,
        gga->geoidalSep,
        gga->geoidalSepUnit,
        gga->dGPSTimeSinceLast,
        gga->dGPSId);
    ret = NmeaProcessTime((const char *)timebuf, strlen(timebuf), &gga->utc);
    if (ret != GNSS_OK) {
        return GNSS_ERR;
    }
    LBSLOGD(HDI_GNSS,
        "GGA utctime year:%u month:%u day:%u hour:%u min:%u sec:%u centisec:%u!",
        gga->utc.year,
        gga->utc.month,
        gga->utc.day,
        gga->utc.hour,
        gga->utc.min,
        gga->utc.sec,
        gga->utc.centisec);
    return NmeaGGACheckInvalid(gga);
}

static void NmeaRMCInit(GnssNmeaRMC *rmc)
{
    if (rmc == NULL) {
        return;
    }
    rmc->latitude = NAN;
    rmc->longitude = NAN;
    rmc->speedN = NAN;
    rmc->trackAng = NAN;
    rmc->magVariation = NAN;
    return;
}

static int NmeaRMCCheckInvalid(GnssNmeaRMC *rmc, int cnts, unsigned int date)
{
    if (!NmeaIsNan(rmc->latitude) && rmc->nSouth) {
        if (NmeaIsIlleageDirection((const char *)&rmc->nSouth, GNSS_DIR_NSOUTH) == true) {
            return GNSS_ERR;
        }
        rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_LAT;
    }

    if (!NmeaIsNan(rmc->longitude) && rmc->eWest) {
        if (NmeaIsIlleageDirection((const char *)&rmc->eWest, GNSS_DIR_EWEST) == true) {
            return GNSS_ERR;
        }
        rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_LON;
    }

    if (!NmeaIsNan(rmc->speedN)) {
        rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_SPEED;
    }

    if (!NmeaIsNan(rmc->trackAng)) {
        rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_TRACK;
    }

    if (date < NMEA_RMC_DATE_LIMIT) {
        NmeaProcessDate(date, &rmc->utc);
        if (true == NmeaIsIlleageDate(&rmc->utc)) {
            return GNSS_ERR;
        }
        rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_UTCDATE;
    }

    if (!NmeaIsNan(rmc->magVariation) && rmc->eWest) {
        if (NmeaIsIlleageDirection((const char *)&rmc->eWest, GNSS_DIR_EWEST) == true) {
            return GNSS_ERR;
        }
        rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_MAGVAR;
    }

    if (cnts == NMEA_RMC_FILELDS_CNTS_BEFORE_V23) {
        rmc->mode = 'A';
        return GNSS_OK;
    } else if (cnts < NMEA_RMC_FILELDS_CNTS_AFTER_V41) {
        rmc->navStatus = 'V';
    }

    if (rmc->mode == 0) {
        rmc->mode = 'N';
    } else {
        rmc->mode = (char)toupper(rmc->mode);
        if (true == NmeaIsIlleageMode((const char *)&rmc->mode)) {
            return GNSS_ERR;
        }
    }
    return GNSS_OK;
}

static int NmeaProcessRMC(const char *nmeaBuf, unsigned int len, GnssNmeaRMC *rmc)
{
    int ret;
    int cnts;
    char timebuf[NMEA_FIELD_TIME_SIZE] = {0};
    unsigned int date = 0xFFFFFFFF;
    char formatStr[] = "RMC,%s,%c,%f,%c,%f,%c,%f,%f,%d,%f,%c,%c,%c*";

    if (nmeaBuf == NULL || rmc == NULL) {
        return GNSS_ERR;
    }

    NmeaRMCInit(rmc);

    cnts = GnssNmeaFormat(nmeaBuf + NMEA_SENTENCE_START_BIT,
        len - NMEA_SENTENCE_START_BIT,
        formatStr,
        timebuf,
        &rmc->curStat,
        &rmc->latitude,
        &rmc->nSouth,
        &rmc->longitude,
        &rmc->eWest,
        &rmc->speedN,
        &rmc->trackAng,
        &date,
        &rmc->magVariation,
        &rmc->magVarEW,
        &rmc->mode,
        &rmc->navStatus);
    if (cnts < NMEA_RMC_FILELDS_CNTS_BEFORE_V23 || cnts > NMEA_RMC_FILELDS_CNTS_AFTER_V41) {
        return GNSS_ERR;
    }

    ret = NmeaProcessTime((const char *)timebuf, strlen(timebuf), &rmc->utc);
    if (ret != GNSS_OK) {
        return GNSS_ERR;
    }

    if (true == NmeaIsIlleageTime(&rmc->utc)) {
        return GNSS_ERR;
    }

    rmc->fieldType |= GNSS_NMEA_FIELD_TYPE_UTCTIME;

    if (rmc->curStat == 0) {
        rmc->curStat = 'V';
    } else {
        rmc->curStat = (char)toupper(rmc->curStat);
        if (rmc->curStat != 'A' && rmc->curStat != 'V') {
            return GNSS_ERR;
        }
    }
    return NmeaRMCCheckInvalid(rmc, cnts, date);
}

static void NmeaVTGInit(GnssNmeaVTG *vtg)
{
    if (vtg == NULL) {
        return;
    }
    vtg->magneticTrackAng = NAN;
    vtg->speed = NAN;
    vtg->dSpeedN = NAN;
    vtg->trackAng = NAN;
    return;
}

static int NmeaVTGCheckInvalid(GnssNmeaVTG *vtg)
{
    if (!NmeaIsNan(vtg->trackAng) && vtg->trackT) {
        vtg->trackT = (char)toupper(vtg->trackT);
        if (vtg->trackT != 'T') {
            return GNSS_ERR;
        }
        vtg->fieldType |= GNSS_NMEA_FIELD_TYPE_TRACK;
    }

    if (!NmeaIsNan(vtg->magneticTrackAng) && vtg->trackM != 0) {
        vtg->trackM = (char)toupper(vtg->trackM);
        if (vtg->trackM != 'M') {
            return GNSS_ERR;
        }
        vtg->fieldType |= GNSS_NMEA_FIELD_TYPE_MTRACK;
    }

    if (!NmeaIsNan(vtg->dSpeedN) && vtg->cSpeedN != 0) {
        vtg->cSpeedN = (char)toupper(vtg->cSpeedN);
        if (vtg->cSpeedN != 'N') {
            return GNSS_ERR;
        }
        vtg->fieldType |= GNSS_NMEA_FIELD_TYPE_SPEED;

        if (NmeaIsNan(vtg->speed)) {
            vtg->speed = vtg->dSpeedN * GNSS_KNOTS;
            vtg->speedK = 'K';
        }
    }

    if (!NmeaIsNan(vtg->speed) && vtg->speedK) {
        vtg->speedK = (char)toupper(vtg->speedK);
        if (vtg->speedK != 'K') {
            return GNSS_ERR;
        }
        vtg->fieldType |= GNSS_NMEA_FIELD_TYPE_SPEED;

        if (NmeaIsNan(vtg->dSpeedN)) {
            vtg->dSpeedN = vtg->speed / GNSS_KNOTS;
            vtg->cSpeedN = 'N';
        }
    }
    return GNSS_OK;
}

static int NmeaProcessVTG(const char *nmeaBuf, unsigned int len, GnssNmeaVTG *vtg)
{
    int cnts;
    char formatStr[] = "VTG,%f,%c,%f,%c,%f,%c,%f,%c,%c*";

    if (nmeaBuf == NULL || vtg == NULL) {
        return GNSS_ERR;
    }

    NmeaVTGInit(vtg);

    cnts = GnssNmeaFormat(nmeaBuf + NMEA_SENTENCE_START_BIT,
        len - NMEA_SENTENCE_START_BIT,
        formatStr,
        &vtg->trackAng,
        &vtg->trackT,
        &vtg->magneticTrackAng,
        &vtg->trackM,
        &vtg->dSpeedN,
        &vtg->cSpeedN,
        &vtg->speed,
        &vtg->speedK,
        &vtg->posModeA);
    if (cnts != GNSS_FILEDS_IN_VTG && cnts != (GNSS_FILEDS_IN_VTG - 1)) {
        return GNSS_ERR;
    }
    LBSLOGD(HDI_GNSS,
        "vtg trackAng :%f trackT:%c magneticTrackAng:%f trackM:%c dSpeedN:%f cSpeedN:%c speed:%f speedK:%c "
        "posModeA:%c ",
        vtg->trackAng,
        vtg->trackT,
        vtg->magneticTrackAng,
        vtg->trackM,
        vtg->dSpeedN,
        vtg->cSpeedN,
        vtg->speed,
        vtg->speedK,
        vtg->posModeA);

    return NmeaVTGCheckInvalid(vtg);
}

static void NmeaGSAInit(GnssNmeaGSA *gsa)
{
    if (gsa == NULL) {
        return;
    }

    gsa->operType = 0xFF;
    gsa->hdop = NAN;
    gsa->pdop = NAN;
    gsa->vdop = NAN;
    return;
}

static void NmeaGSAGetSystemId(const char *nmeaBuf, int systemId, int cnts, GnssSystem *gnsssystem)
{
    if (nmeaBuf == NULL || gnsssystem == NULL) {
        return;
    }

    if (!strncmp(nmeaBuf, "$GN", NMEA_SENTENCE_START_BIT)) {
        if (cnts != NMEA_GSA_FILEDS_CNTS_AFTER_V41) {
            return;
        }
        if (systemId == GNSS_SYSTEM_GPS) {
            *gnsssystem = GNSS_SYSTEM_GPS;
        } else if (systemId == GNSS_SYSTEM_GLONASS) {
            *gnsssystem = GNSS_SYSTEM_GLONASS;
        } else if (systemId == GNSS_SYSTEM_BDS) {
            *gnsssystem = GNSS_SYSTEM_BDS;
        } else {
            *gnsssystem = GNSS_SYSTEM_GALILEO;
        }
    } else if (!strncmp(nmeaBuf, "$GA", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_GALILEO;
    } else if (!strncmp(nmeaBuf, "$GP", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_GPS;
    } else if (!strncmp(nmeaBuf, "$GL", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_GLONASS;
    } else if (!strncmp(nmeaBuf, "$GQ", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_QZSS;
    } else {
        *gnsssystem = GNSS_SYSTEM_BDS;
    }
    return;
}

static int NmeaGSACheckInvalid(GnssNmeaGSA *gsa)
{
    int i;
    gsa->operMode = (unsigned int)toupper((int)gsa->operMode);
    if (gsa->operMode != 'A' && gsa->operMode != 'M') {
        return GNSS_ERR;
    }

    if (gsa->operType > GNSS_OPER_3D) {
        return GNSS_ERR;
    }

    gsa->fieldType |= GNSS_NMEA_FIELD_TYPE_OPERMODE;

    for (i = 0; i < GNSS_SAT_NUM_MAX; i++) {
        if (gsa->satPrn[i]) {
            gsa->fieldType |= GNSS_NMEA_FIELD_TYPE_SATINUSE;
            break;
        }
    }

    if (!NmeaIsNan(gsa->pdop)) {
        gsa->fieldType |= GNSS_NMEA_FIELD_TYPE_PDOP;
    }

    if (!NmeaIsNan(gsa->hdop)) {
        gsa->fieldType |= GNSS_NMEA_FIELD_TYPE_HDOP;
    }

    if (!NmeaIsNan(gsa->vdop)) {
        gsa->fieldType |= GNSS_NMEA_FIELD_TYPE_VDOP;
    }
    return GNSS_OK;
}

static int NmeaProcessGSA(const char *nmeaBuf, unsigned int len, GnssSystem *gnsssystem, GnssNmeaGSA *gsa)
{
    int cnts;
    char formatStr[] = "GSA,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%d*";

    if (nmeaBuf == NULL || gsa == NULL || gnsssystem == NULL) {
        return GNSS_ERR;
    }

    NmeaGSAInit(gsa);

    cnts = GnssNmeaFormat(nmeaBuf + NMEA_SENTENCE_START_BIT,
        len - NMEA_SENTENCE_START_BIT,
        formatStr,
        &gsa->operMode,
        &gsa->operType,
        &gsa->satPrn[NMEA_SATE_NUM_BIT_0],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_1],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_2],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_3],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_4],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_5],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_6],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_7],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_8],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_9],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_10],
        &gsa->satPrn[NMEA_SATE_NUM_BIT_11],
        &gsa->pdop,
        &gsa->hdop,
        &gsa->vdop,
        &gsa->systemId);
    LBSLOGD(HDI_GNSS,
        "GGA operMode :%u operType:%u pdop:%f hdop:%f vdop:%f systemId:%d",
        gsa->operMode,
        gsa->operType,
        gsa->pdop,
        gsa->hdop,
        gsa->vdop,
        gsa->systemId);
    for (int i = 0; i <= NMEA_SATE_NUM_BIT_11; i++) {
        LBSLOGD(HDI_GNSS, "GGA Prn i:%d Prn:%u", i, gsa->satPrn[i]);
    }
    if (cnts != NMEA_GSA_FILEDS_CNTS_BEFORE_V41 && cnts != NMEA_GSA_FILEDS_CNTS_AFTER_V41) {
        return GNSS_ERR;
    }

    NmeaGSAGetSystemId(nmeaBuf, gsa->systemId, cnts, gnsssystem);

    return NmeaGSACheckInvalid(gsa);
}

static bool NmeaIsIlleageSatStat(const GnssSatelliteStatus *satStat, int *cnts)
{
    int i;
    int cnt = 0;

    if (satStat == NULL || cnts == NULL) {
        return true;
    }

    for (i = 0; i < GNSS_SAT_IN_MSG; i++) {
        if (satStat[i].svid != 0) {
            if (satStat[i].ele < NMEA_ELE_MIN || satStat[i].ele > NMEA_ELE_MAX) {
                return true;
            }

            if (satStat[i].az >= NMEA_AZ_MAX) {
                return true;
            }

            if (satStat[i].cnr > NMEA_CNR_MAX) {
                return true;
            }
            cnt++;
            *cnts = cnt;
        }
    }
    return false;
}

static void NmeaGSVGetSystemId(const char *nmeaBuf, GnssSystem *gnsssystem)
{
    if (nmeaBuf == NULL || gnsssystem == NULL) {
        return;
    }

    if (!strncmp(nmeaBuf, "$GP", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_GPS;
    } else if (!strncmp(nmeaBuf, "$GL", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_GLONASS;
    } else if (!strncmp(nmeaBuf, "$GA", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_GALILEO;
    } else if (!strncmp(nmeaBuf, "$GQ", NMEA_SENTENCE_START_BIT)) {
        *gnsssystem = GNSS_SYSTEM_QZSS;
    } else {
        *gnsssystem = GNSS_SYSTEM_BDS;
    }
    return;
}

static int NmeaProcessGSV(const char *nmeaBuf, unsigned int len, GnssSystem *gnssSystem, GnssNmeaGSV *gsv)
{
    int ret;
    int cnts;
    int excnts;
    int satlegalcnts = 0;
    char formatStr[] = "GSV,%d,%d,%d,%d,%h,%h,%h,%d,%h,%h,%h,%d,%h,%h,%h,%d,%h,%h,%h*";

    if (gsv == NULL || nmeaBuf == NULL) {
        return GNSS_ERR;
    }

    gsv->msgCnt = 0xFFFF;
    gsv->msgIdx = 0xFFFF;
    gsv->satViewedCnt = 0xFFFF;
    // NMEA_SAT_TYPE_BIT Remove the last comma and asterisk to prevent the number of last digit
    // parsing errors in less than four groups {,svid,ele,azi,cn0}.
    cnts = GnssNmeaFormat(nmeaBuf + NMEA_SENTENCE_START_BIT,
        len - NMEA_SENTENCE_START_BIT - NMEA_SAT_TYPE_BIT,
        formatStr,
        &gsv->msgCnt,
        &gsv->msgIdx,
        &gsv->satViewedCnt,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_0].svid,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_0].ele,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_0].az,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_0].cnr,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_1].svid,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_1].ele,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_1].az,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_1].cnr,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_2].svid,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_2].ele,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_2].az,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_2].cnr,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_3].svid,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_3].ele,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_3].az,
        &gsv->satelliteStatus[NMEA_SATE_NUM_BIT_3].cnr);
    if (gsv->msgCnt > GNSS_NSAT_MSGS || gsv->msgIdx > gsv->msgCnt || gsv->satViewedCnt > GNSS_SAT_NUM_MAX) {
        return GNSS_ERR;
    }

    NmeaGSVGetSystemId(nmeaBuf, gnssSystem);
    LBSLOGD(HDI_GNSS,
        "GSV gnssSystem :%d msgCnt:%u msgIdx:%u satViewedCnt:%u",
        *gnssSystem,
        gsv->msgCnt,
        gsv->msgIdx,
        gsv->satViewedCnt);
    for (int i = 0; i <= NMEA_SATE_NUM_BIT_3; i++) {
        LBSLOGD(HDI_GNSS,
            "GSV gnssSystem :%d sat index:%u svid:%u ele:%u az:%u cnr:%u",
            *gnssSystem,
            i,
            gsv->satelliteStatus[i].svid,
            gsv->satelliteStatus[i].ele,
            gsv->satelliteStatus[i].az,
            gsv->satelliteStatus[i].cnr);
    }
    if (NmeaIsIlleageSatStat(gsv->satelliteStatus, &satlegalcnts)) {
        LBSLOGE(HDI_GNSS, "GSV IsIlleageSatStat nmeaBuf:%s", nmeaBuf);
        return GNSS_ERR;
    }

    excnts = (int)(satlegalcnts * NMEA_ONE_SENTENCE_FOUR_SAT + NMEA_GSV_OTHER_BIT);

    if (cnts < excnts) {
        return GNSS_ERR;
    }

    if (gsv->satViewedCnt > 0) {
        gsv->fieldType |= GNSS_NMEA_FIELD_TYPE_SATINVIEW;
    }
    return GNSS_OK;
}

static bool NmeaIsIlleageChar(char ch)
{
    if (ch < ILLEAGECHAR_MIN || ch > ILLEAGECHAR_MAX || ch == GNSS_FRAME_START_FLAG) {
        return true;
    } else {
        return false;
    }
}

static int NmeaGetAFrame(GnssNmeaFrameProcess *nmeaFrameProcess, const char *frame, unsigned int frameLen)
{
    unsigned int i;
    unsigned int crc = 0;
    char hexCRC[NMEA_CRC_MAX] = {0};
    unsigned char headFound = 0;

    for (i = 0; i < frameLen; i++) {
        if (frame[i] != GNSS_FRAME_START_FLAG && headFound == 0) {
            continue;
        }
        headFound = 1;
        nmeaFrameProcess->nmeaBuf[nmeaFrameProcess->length++] = frame[i];
        if (nmeaFrameProcess->length > 1) {
            if (frame[i] == GNSS_FRAME_CRC_FLAG) {
                hexCRC[NMEA_CRC_BIT_0] = frame[i + 1];
                hexCRC[NMEA_CRC_BIT_1] = frame[i + NMEA_CRC_BIT_2];
                break;
            }
            if (NmeaIsIlleageChar(frame[i])) {
                LBSLOGE(HDI_GNSS, "lleageChar :%c", frame[i]);
                return GNSS_ERR;
            }
            crc ^= (unsigned int)((unsigned char)frame[i]);
        }
    }

    if (headFound == 0) {
        LBSLOGE(HDI_GNSS, "head not found");
        return GNSS_ERR;
    }
    char *endptr = NULL;
    if (crc != strtoul(hexCRC, &endptr, NMEA_HEX)) {
        return GNSS_ERR;
    }
    return GNSS_OK;
}

GnssNmeaFrameType GnssNmeaGetFrameType(const char *nmeaBuf, unsigned int length)
{
    unsigned int i;
    const char *apcTypes[] = {"GGA", "GSA", "GSV", "RMC", "VTG", "GLL", "ZDA", "TH000", "TH001", "TH002"};
    GnssNmeaFrameType aeTypes[] = {GNSS_NMEA_FRAME_GGA,
        GNSS_NMEA_FRAME_GSA,
        GNSS_NMEA_FRAME_GSV,
        GNSS_NMEA_FRAME_RMC,
        GNSS_NMEA_FRAME_VTG,
        GNSS_NMEA_FRAME_GLL,
        GNSS_NMEA_FRAME_ZDA,
        GNSS_NMEA_FRAME_TH000,
        GNSS_NMEA_FRAME_TH001,
        GNSS_NMEA_FRAME_TH002};
    if (length < NMEA_SENTENCE_START_BIT) {
        LBSLOGW(HDI_GNSS, "nmeaBuf invalid length:%d", length);
        return GNSS_NMEA_FRAME_MAX;
    }
    // A valid nmea starts with "$".
    if (*nmeaBuf != '$') {
        LBSLOGW(HDI_GNSS, "nmeaBuf invalid not starts with $");
        return GNSS_NMEA_FRAME_MAX;
    }
    for (i = 0; i < (sizeof(aeTypes) / sizeof(aeTypes[0])); i++) {
        if (strncmp(nmeaBuf + NMEA_SENTENCE_START_BIT, apcTypes[i], strlen(apcTypes[i])) == 0) {
            return aeTypes[i];
        }
    }
    return GNSS_NMEA_FRAME_MAX;
}

static int NmeaFrameProc(GnssNmeaFrameType frameType, GnssNmeaFrameProcess *nmeaFrameProcess, GnssInfo *gnssInfo)
{
    int ret;
    GnssSystem gnssSystem = GNSS_SYSTEM_INVALID;

    switch (frameType) {
        case GNSS_NMEA_FRAME_GGA:
            ret = NmeaProcessGGA((const char *)nmeaFrameProcess->nmeaBuf, nmeaFrameProcess->length,
                                 &(nmeaFrameProcess->nmeaFrame.nmeaGGA));
            if (ret == GNSS_OK) {
                ret = NmeaGGA2GnssInfo(&(nmeaFrameProcess->nmeaFrame.nmeaGGA), gnssInfo);
            }
            break;
        case GNSS_NMEA_FRAME_RMC:
            ret = NmeaProcessRMC((const char *)nmeaFrameProcess->nmeaBuf, nmeaFrameProcess->length,
                                 &(nmeaFrameProcess->nmeaFrame.nmeaRMC));
            if (ret == GNSS_OK) {
                ret = NmeaRMC2GnssInfo(&(nmeaFrameProcess->nmeaFrame.nmeaRMC), gnssInfo);
            }
            break;
        case GNSS_NMEA_FRAME_VTG:
            ret = NmeaProcessVTG((const char *)nmeaFrameProcess->nmeaBuf, nmeaFrameProcess->length,
                                 &(nmeaFrameProcess->nmeaFrame.nmeaVTG));
            if (ret == GNSS_OK) {
                ret = NmeaVTG2GnssInfo(&(nmeaFrameProcess->nmeaFrame.nmeaVTG), gnssInfo);
            }
            break;
        case GNSS_NMEA_FRAME_GSA:
            ret = NmeaProcessGSA((const char *)nmeaFrameProcess->nmeaBuf, nmeaFrameProcess->length, &gnssSystem,
                                 &(nmeaFrameProcess->nmeaFrame.nmeaGSA));
            if (ret == GNSS_OK) {
                ret = NmeaGSA2GnssInfo(&(nmeaFrameProcess->nmeaFrame.nmeaGSA), gnssSystem, gnssInfo);
            }
            break;
        case GNSS_NMEA_FRAME_GSV:
            ret = NmeaProcessGSV((const char *)nmeaFrameProcess->nmeaBuf, nmeaFrameProcess->length, &gnssSystem,
                                 &(nmeaFrameProcess->nmeaFrame.nmeaGSV));
            if (ret == GNSS_OK) {
                ret = NmeaGSV2GnssInfo(&(nmeaFrameProcess->nmeaFrame.nmeaGSV), gnssSystem, gnssInfo);
            }
            break;
        default:
            ret = GNSS_OK;
            break;
    }
    return ret;
}

int GnssNmeaDecodeFrame(const char *nmea, unsigned int nmeaLen, GnssInfo *gnssInfo)
{
    GnssNmeaFrameProcess nmeaFrameProcess;
    GnssNmeaFrameType nmeaFrameType;
    (void)memset_s(&nmeaFrameProcess, sizeof(GnssNmeaFrameProcess), 0, sizeof(GnssNmeaFrameProcess));
    int ret = NmeaGetAFrame(&nmeaFrameProcess, (const char *)nmea, nmeaLen);
    if (ret != 0) {
        LBSLOGE(HDI_GNSS, "NmeaGetAFrame failed:%d", ret);
        return ret;
    }
    nmeaFrameType = GnssNmeaGetFrameType((const char *)&nmeaFrameProcess.nmeaBuf, nmeaFrameProcess.length);
    return NmeaFrameProc(nmeaFrameType, &nmeaFrameProcess, gnssInfo);
}

void GnssNmeaInit()
{
    (void)memset_s(g_speeds, sizeof(g_speeds), 0, sizeof(g_speeds));
    g_isWriteinPlace = false;
    g_sampleCnt = 0;
    g_previousAltitude = 0.0;
    g_previousTime = 0.0;
}
