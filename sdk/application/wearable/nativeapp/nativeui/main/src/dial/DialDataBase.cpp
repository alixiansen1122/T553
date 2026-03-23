/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialDataBase
 * Author:
 * Create: 2025-04-08
 */

#include <unordered_map>
#include "main/dial/DialDataBase.h"

namespace {
struct DataTypeHash {
    size_t operator()(const DialOldDataType &data) const
    {
        return std::hash<int16_t>{}(static_cast<int16_t>(data));
    }
};

static std::unordered_map<DialOldDataType, OHOS::DialDataType, DataTypeHash> g_data2std = {
    {DialOldDataType::DATA_TYPE_STEPS, OHOS::DialDataType::STEP_COUNT},
    {DialOldDataType::DATA_TYPE_HEART_RATE, OHOS::DialDataType::HEART_RATE},
    {DialOldDataType::DATA_TYPE_POWER, OHOS::DialDataType::BATTERY},
    {DialOldDataType::DATA_TYPE_CLOCK_ANGLE_H, OHOS::DialDataType::CLOCK_ANGLE},
    {DialOldDataType::DATA_TYPE_CLOCK_ANGLE_M, OHOS::DialDataType::MINUTE_ANGLE},
    {DialOldDataType::DATA_TYPE_CLOCK_ANGLE_S, OHOS::DialDataType::SECOND_ANGLE},
    {DialOldDataType::DATA_TYPE_CLOCK_ANGLE_MS, OHOS::DialDataType::MILLISECOND_ANGLE},
    {DialOldDataType::DATA_TYPE_TIME_SECOND, OHOS::DialDataType::SECOND_TIME},
    {DialOldDataType::DATA_TYPE_TIME_MINUTE, OHOS::DialDataType::MINUTE_TIME},
    {DialOldDataType::DATA_TYPE_TIME_HOUR, OHOS::DialDataType::CLOCK},
    {DialOldDataType::DATA_TYPE_TIME_DATE, OHOS::DialDataType::DATE},
    {DialOldDataType::DATA_TYPE_TIME_WEEK, OHOS::DialDataType::WEEKDAY_DATA},
    {DialOldDataType::DATA_TYPE_TIME_MONTH, OHOS::DialDataType::MONTH},
    {DialOldDataType::DATA_TYPE_TIME_YEAR, OHOS::DialDataType::SOLAR_YEAR},
};
}

 OHOS::DialDataType Convert2DataType(DialOldDataType dialData)
{
    if (g_data2std.find(dialData) != g_data2std.end()) {
        return g_data2std[dialData];
    }
    return OHOS::DialDataType::DATA_TYPE_MAX;
}