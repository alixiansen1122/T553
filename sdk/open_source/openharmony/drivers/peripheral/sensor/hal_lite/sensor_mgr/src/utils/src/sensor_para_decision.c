/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor para impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */


#include "sensor_para_decision.h"
#include "sensor_log.h"
#include "sensor_utils.h"
#include "sensor_common.h"

#define OPTION_DEFAULT_VAL 0 /* OPTION初始值 0 */
#define DEFAULT_REALTIME_PERIOD (SENSOR_PARA_REALTIME_PERIOD_MIN * 1000)

void SensorParaDefaultInit(SensorPara *sensorPara)
{
    sensorPara->period = DEFAULT_REALTIME_PERIOD;
    sensorPara->batch = SENSOR_PARA_BATCH_MIN;
    sensorPara->mode = SENSOR_MODE_REALTIME; /* fifo mode OR realtime mode */
    sensorPara->option = 0;
}


void SensorParaClear(SensorPara *para)
{
    if (para == NULL) {
        return;
    }

    para->period = 0;
    para->batch = 0;
    para->mode = SENSOR_MODE_MAX;
    para->option = OPTION_DEFAULT_VAL;
}

void SensorParaCopy(SensorPara *dest, const SensorPara *src)
{
    if (dest == NULL || src == NULL) {
        return;
    }

    dest->period = src->period;
    dest->batch = src->batch;
    dest->mode = src->mode;
    dest->option = src->option;
}

bool SensorParaIsClear(const SensorPara *para)
{
    if (para == NULL) {
        return true;
    }

    if (para->period == 0) {
        return true;
    } else {
        return false;
    }
}

int32_t SensorOptionDecision(enum SensorTypeTag type, int32_t cur, int32_t require)
{
    return OPTION_DEFAULT_VAL;
}

int32_t SensorOpenParaDecision(
    enum SensorTypeTag type, const SensorPara *cur, const SensorPara *require, SensorPara *result)
{
    if (cur == NULL || require == NULL || result == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d para null", type);
        return SENSOR_SENSOR_DECISION_PARA_NULL;
    }

    if (SensorParaIsClear(cur)) {
        SensorParaCopy(result, require);
        return SENSOR_OK;
    }

    result->period = SENSOR_MIN(cur->period, require->period);
    if (result->period == 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d period == 0", type);
        return SENSOR_ERROR;
    }
    result->batch = SENSOR_MIN(cur->batch * cur->period, require->batch * require->period) / result->period;
    result->mode = SENSOR_MIN(cur->mode, require->mode);
    result->option = SensorOptionDecision(type, cur->option, require->option);

    return SENSOR_OK;
}