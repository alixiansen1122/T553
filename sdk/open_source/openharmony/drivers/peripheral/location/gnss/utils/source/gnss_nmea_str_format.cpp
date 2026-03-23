/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: nmea??
 * Author: ??
 * Create: 2021-4-23
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>
#include <ctype.h>
#include <string.h>
#include "securec.h"
#include "gnss_nmea_str_format.h"

#define FIELD_MAX_SIZE        24

#define  FORMAT_DEC  10

static const char* NmeaFormat(const char* format, int len)
{
    int i;
    if (format == NULL) {
        return NULL;
    }
    for (i = 0; i < len; i++) {
        if (format[i] == '%') {
            break;
        }
    }
    if (i < (len - 1)) {
        return &format[i + 1];
    }
    return NULL;
}

static void NmeaFormatGetValue(char format, va_list *args, const char *nmea, unsigned char nmeaSize, int *cnt)
{
    if (args == NULL || nmea == NULL || cnt == NULL) {
        return;
    }
    void* arg = NULL;
    switch (format) {
        case 's':
            arg = (void *) va_arg(*args, char *);
            if (nmeaSize > 1) {
                if (memcpy_s(arg, FIELD_MAX_SIZE, nmea, nmeaSize - 1) != EOK) {
                    (*cnt)++;
                    break;
                }
            }
            (*cnt)++;
            break;
        case 'c':
            arg = (void *) va_arg(*args, char *);
            if (nmeaSize > 1) {
                *(char*)arg = nmea[0];
            }
            (*cnt)++;
            break;
        case 'f':
            arg = (void *) va_arg(*args, double *);
            if (nmeaSize > 1) {
                *(double*)arg = strtod(nmea, NULL);
            }
            (*cnt)++;
            break;
        case 'd':
            arg = (void *) va_arg(*args, int *);
            if (nmeaSize > 1) {
                *(int*)arg = strtol(nmea, NULL, FORMAT_DEC);
            }
            (*cnt)++;
            break;
        case 'h':
            arg = (void *) va_arg(*args, short *);
            if (nmeaSize > 1) { *(short*)arg = (short)strtol(nmea, NULL, FORMAT_DEC); }
            (*cnt)++;
            break;
        case '\0':
            break;
        default:
            break;
    }
    return;
}

int GnssNmeaFormat(const char* nmea, unsigned int len, const char* format, ...)
{
    unsigned int i;
    int cnt = 0;
    va_list args = {0};
    char tmpNmea[FIELD_MAX_SIZE] = {0};
    const char* tmpFormat = format;
    unsigned char flag = 0;
    unsigned char tmpSize = 0;

    va_start(args, format);

    for (i = 0; i < len; i++) {
        tmpNmea[tmpSize++] = nmea[i];
        if (nmea[i] == ',' || nmea[i] == '*') {
            if (flag == 0) {
                flag = 1;
                (void)memset_s(tmpNmea, FIELD_MAX_SIZE, 0, FIELD_MAX_SIZE);
                tmpSize = 0;
                continue;
            }

            tmpFormat = NmeaFormat(tmpFormat, (int)strlen(tmpFormat));
            if (tmpFormat == NULL) {
                break;
            }
            NmeaFormatGetValue(*tmpFormat, &args, tmpNmea, tmpSize, &cnt);
            (void)memset_s(tmpNmea, FIELD_MAX_SIZE, 0, FIELD_MAX_SIZE);
            tmpSize = 0;
        }
    }
    va_end(args);
    return cnt;
}

