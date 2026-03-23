/*
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: gnss process AT cmd
 * STRATEGY : NO
 */

#ifndef GNSS_PROCESS_H
#define GNSS_PROCESS_H

#include <stdint.h>
#include "errcode.h"

// nmeaFlag：NMEA保存到文件选项 0：不保存；1：保存。
// replayFlag: binary保存到文件选项 0：不保存；1：保存。
// pgnssFlag: PGNSS注入方式，0：多文件注入；1：少文件注入
// rsv1: 保留位
errcode_t gnss_proc_msg_init(uint8_t nmeaFlag, uint8_t replayFlag, uint8_t pgnssFlag, uint8_t rsv1);
errcode_t gnss_proc_msg_start(void);
errcode_t gnss_proc_msg_stop(void);
errcode_t gnss_proc_msg_deinit(void);
errcode_t gnss_proc_msg_cfg(uint8_t *data, uint16_t len);
errcode_t gnss_proc_msg_sample(uint8_t index);
errcode_t gnss_proc_msg_nmea(uint8_t index);
errcode_t gnss_proc_log_ctrl_info(uint32_t logNumLimit, uint32_t logSizeLimit);
 
#endif