/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Nmea Format
 * Create: 2024-10-17
 */
#ifndef GNSS_NMEA_STR_FORMAT_H
#define GNSS_NMEA_STR_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

int GnssNmeaFormat(const char* nmea, unsigned int len, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif