/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss normal
 * Author:
 * Create:
 */

#ifndef GNSS_NORMAL_H
#define GNSS_NORMAL_H

#include "errcode.h"

errcode_t gnss_cold_start(void);
errcode_t gnss_hot_start(void);

#endif