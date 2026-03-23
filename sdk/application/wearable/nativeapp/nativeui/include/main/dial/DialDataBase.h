/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialBaseModel
 * Author:
 * Create: 2023-12
 */

#ifndef DIAL_DATA_BASE_H
#define DIAL_DATA_BASE_H
#include "main/dial/DialDataType.h"

enum class DialOldDataType {
    DATA_TYPE_FLOAT_BASE = 0,
    DATA_TYPE_STEPS,
    DATA_TYPE_HEART_RATE,
    DATA_TYPE_CALORIE,
    DATA_TYPE_TEMPERATURE,
    DATA_TYPE_PM25,
    DATA_TYPE_PRESSURE,
    DATA_TYPE_AILTITUDE,
    DATA_TYPE_POWER,
    DATA_TYPE_CLOCK_ANGLE_H,
    DATA_TYPE_CLOCK_ANGLE_M,
    DATA_TYPE_CLOCK_ANGLE_S,
    DATA_TYPE_CLOCK_ANGLE_MS,
    DATA_TYPE_TIME_SECOND,
    DATA_TYPE_TIME_MINUTE,
    DATA_TYPE_TIME_HOUR,
    DATA_TYPE_TIME_DATE,
    DATA_TYPE_TIME_WEEK,
    DATA_TYPE_TIME_MONTH,
    DATA_TYPE_TIME_YEAR,

    DATA_TYPE_TEXT_BASE = 0x2000,

    DATA_TYPE_MULTI_FLOAT_BASE = 0x4000,

    DATA_TYPE_MAX = 0x6000
};

OHOS::DialDataType Convert2DataType(DialOldDataType dialData);
#endif
