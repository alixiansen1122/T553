/**
* Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
* Description: nfc hal adapter interface header. \n
* Author: @CompanyNameTag \n
* Date: 2024-05-24 \n
*/

#ifndef __NFC_DEV_H__
#define __NFC_DEV_H__

#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os.h"
#include "std_def.h"

enum {
    NFC_SUCC = 0,
    NFC_EFAIL,
};

#define NCI_HEAD_LEN 3

#define PLAT_LOG_NONE 0
#define PLAT_LOG_ALERT 1
#define PLAT_LOG_ERR 2
#define PLAT_LOG_WARNING 3
#define PLAT_LOG_INFO 4
#define PLAT_LOG_DEBUG 5

#define PLAT_NFC_LOGLEVEL PLAT_LOG_ALERT

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_DEBUG)
#define PS_PRINT_FUNCTION_NAME printf("NFC Func:%s", __func__)
#else
#define PS_PRINT_FUNCTION_NAME
#endif

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_DEBUG)
#define PS_PRINT_DBG(s, args...) printf(s, ##args)
#else
#define PS_PRINT_DBG(s, args...)
#endif

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_INFO)
#define PS_PRINT_INFO(s, args...) printf(s, ##args)
#else
#define PS_PRINT_INFO(s, args...)
#endif

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_INFO)
#define PS_PRINT_SUC(s, args...) printf(s, ##args)
#else
#define PS_PRINT_SUC(s, args...)
#endif

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_WARNING)
#define PS_PRINT_WARNING(s, args...) printf(s, ##args)
#else
#define PS_PRINT_WARNING(s, args...)
#endif

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_ERR)
#define PS_PRINT_ERR(s, args...) printf(s, ##args)
#else
#define PS_PRINT_ERR(s, args...)
#endif

#if (PLAT_NFC_LOGLEVEL >= PLAT_LOG_ALERT)
#define PS_PRINT_ALERT(s, args...) printf(s, ##args)
#else
#define PS_PRINT_ALERT(s, args...)
#endif

int HW_NFC_Open(void);
int HW_NFC_Close(void);
int HW_NFC_Write(uint8_t *buf, uint16_t count);
int HW_NFC_NciRead(uint8_t *buf, uint16_t bufLen, unsigned long timeout);
void NFC_HAL_Msleep(unsigned int msecs);

#endif