/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: sensor manager interface definition
 * Author: zhaoxingguang3@huawei.com
 * Create: 2020-10-31
 */

/**
 * @addtogroup Sensor
 * @{
 *
 * @brief 传感器管理服务框架的基础功能集，包括API和对应的数据结构
 */

/**
 * @file sensor_agent_type_ext.h
 *
 * @brief 传感器管理服务的数据结构定义
 */

#ifndef SENSOR_AGENT_TYPE_EXT_H
#define SENSOR_AGENT_TYPE_EXT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 加速度传感器的数据结构
 */
typedef struct AccelData {
    /** 加速度X轴 */
    int16_t axisX;
    /** 加速度Y轴 */
    int16_t axisY;
    /** 加速度Z轴 */
    int16_t axisZ;
} AccelData;

/**
 * @brief 角速度传感器的数据结构
 */
typedef struct GyroData {
    /** 加速度X轴 */
    int16_t axisX;
    /** 加速度Y轴 */
    int16_t axisY;
    /** 加速度Z轴 */
    int16_t axisZ;
} GyroData;

/**
 * @brief 气压传感器的数据结构
 */
typedef struct PressureData {
    /** 气压计气压值 */
    uint32_t pressure;
    /** 气压计温度值 */
    uint32_t temperature;
    /** 气压计数据有效性 */
    uint32_t dataValid;
} PressureData;

/**
 * @brief Defines the sensor data.
 *
 * @since 5
 */
typedef struct SensorData {
    int64_t timestamp;     /**< Time when sensor data was reported */
    uint32_t batchCnt;     /**< Sensor batch cnt */
    uint8_t *data;         /**< Sensor data */
    uint32_t dataLen;      /**< Sensor data length */
} SensorData;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SENSOR_AGENT_TYPE_EXT_H */
/** @} */
