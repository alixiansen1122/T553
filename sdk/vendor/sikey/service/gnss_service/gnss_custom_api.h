/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: agnss sample
 * Author:
 * Create:
 */
#ifndef GNSS_CUSTOM_API_H
#define GNSS_CUSTOM_API_H

#include <stdio.h>
#include <stdint.h>
#include "errcode.h"
#include "securec.h"
#include "gnss_datatypes.h"
#include "gnss_nmea_process.h"

//void update_binary_save_flag(uint8_t flag);
//void update_nmea_save_flag(uint8_t flag);
void custom_gnss_proc_binary_msg(uint8_t *buff, uint32_t len);
void custom_gnss_proc_nmea_msg(uint8_t *buff, uint32_t len);
//int32_t utc_to_unixtime(GnssUtcTime utcTime, uint32_t *unixTime);
errcode_t custom_gnss_init(void);
errcode_t custom_gnss_start(void);
errcode_t custom_gnss_stop(void);
errcode_t custom_gnss_deinit(void);
//errcode_t gnss_inject_time(GnssUtcTime *utcTime);
//errcode_t encode_and_inject_cmd(GnssCmdType cmdType, uint8_t* inBuff, uint32_t inLen);
void custom_update_gnss_nmea_print_flag(uint8_t flag);
void custom_gnss_save_data(uint8_t dataType, uint8_t *buff, uint32_t size);
void custom_update_nmea_save_flag(uint8_t flag);
errcode_t custom_gnss_cold_start(void);
errcode_t custom_gnss_hot_start(void);
void custom_fac_proc_data(uint8_t *buff);
errcode_t custom_gnss_factory_test(void);
#endif