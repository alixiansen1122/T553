/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor type def
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef HDI_SENSOR_TYPEDEF_H
#define HDI_SENSOR_TYPEDEF_H

#include <stdint.h>
#include <stdbool.h>
#include "sensor_type.h"

/* person info */
#define MIN_USER_WEIGHT 10      /* 单位：公斤 */
#define DEFAULT_USER_WEIGHT 60  /* 单位：公斤 */
#define MAX_USER_WEIGHT 250     /* 单位：公斤 */
#define MIN_USER_HEIGHT 50      /* 单位：厘米 */
#define DEFAULT_USER_HEIGHT 170 /* 单位：厘米 */
#define MAX_USER_HEIGHT 250     /* 单位：厘米 */
#define DEFAULT_USER_AGE 30     /* 单位：年 */
#define MAX_USER_AGE 150        /* 单位：年 */
#define ALG_USER_SEX_WOMEN 0    /* 女 */
#define ALG_USER_SEX_MAN 1      /* 男 */
#define ALG_USER_SEX_UNKNOWN 2  /* UNKNOWN */
#define MAX_USER_GENDER 2

typedef struct {
    uint8_t weight; /* kg */
    uint8_t height; /* cm */
    uint8_t age;    /* YEAR */
    uint8_t gender; /* MALE为 1, FEMALE为 0, UNKNOWN 为2 */
} SensorPersonInfo;

typedef enum {
    SENSOR_IOCTL_SET_PARA = 0,
    SENSOR_IOCTL_GET_PARA,
    SENSOR_IOCTL_GET_INTERVAL,
    SENSOR_IOCTL_GET_INFO,
    SENSOR_IOCTL_CURRENT_ADJUST,
    SENSOR_IOCTL_GET_CHIP_TYPE,
    SENSOR_IOCTL_GET_MAX_BATCHCOUNT,
    SENSOR_IOCTL_GET_FW_VERSION,
    SENSOR_IOCTL_GET_MAX_RANGE,
    SENSOR_IOCTL_GET_PRECISION,
    SENSOR_IOCTL_GET_SENSOR_DATA,
    SENSOR_IOCTL_MAX
} SensorIoctlCmd;

typedef enum {
    SENSOR_STATE_IDLE = 0,
    SENSOR_STATE_OPEN,
    SENSOR_STATE_CLOSE,

    SENSOR_STATE_MAX
} SensorState;

typedef struct {
    uint32_t period; /* 采样间隔单位 ms */
    uint32_t batch;  /* 一次上报的数据组数 */
    uint32_t option;
    enum SensorModeType mode; /* fifo mode OR realtime mode */
} SensorPara;

typedef struct {
    enum SensorTypeTag type;
    SensorPara param;
} RelySensorInfo;

typedef struct {
    RelySensorInfo *relySensorArray;
    uint32_t relySensorCnt;
    uint8_t reserve[3]; /* 4字节对齐，3个字节保留 */
} RelySensorsInfo;

/* sensor信息 */
typedef struct {
    enum SensorTypeTag type; /* sensor 类型 */
    const char *name;        /* sensor 信息 “ACCEL” 、“HR v2.0” 等 */
    const void *privateData; /* sensor 的私有信息，私有接口 */
    uint32_t privateDataLen; /* sensor 的私有信息长度 */
    SensorState state;       /* 当前传感器运行状态，开、关、使能、去使能、暂停等 */
    SensorPara openPara;     /* 当前传感器运行参数 */
} SensorInfo;

/* sensor操作句柄结构 */
typedef struct Sensor Sensor;
struct Sensor {
    SensorInfo sensorInfo;
    /* 方法 */
    int32_t (*open)(const Sensor *sensor, const SensorPara *openPara); /* 传感器开，设置传感器 */
    int32_t (*close)(const Sensor *sensor);                            /* 传感器关 */
    int32_t (*setBatch)(Sensor *sensor, int64_t samplingInterval, int64_t reportInterval);
    int32_t (*setMode)(Sensor *sensor, int32_t mode);
    int32_t (*setOption)(Sensor *sensor, uint32_t option);
    int32_t (*registerResultCallback)(const Sensor *sensor, RecordDataCallback cb);
    int32_t (*unregisterResultCallback)(const Sensor *sensor, RecordDataCallback cb);
    int32_t (*calibrate)(const Sensor *sensor, void *para, uint32_t len); /* 物理传感器专用校准接口,暂不支持 */
    int32_t (*selfTest)(
        const Sensor *sensor, void *para, uint32_t len); /* 厂测模式下使用，物理传感器专用自检接口,暂不支持 */
    int32_t (*ioctl)(const Sensor *sensor, SensorIoctlCmd cmd, void *data, uint32_t len); /* 其他io操作 */
};

typedef struct {
    float maxRange;            /**< Maximum measurement range of the sensor, not support now */
    float accuracy;            /**< Sensor accuracy, not support now */
    int64_t minDelay;          /**< Minimum sample period allowed in microseconds, not support now */
    int64_t maxDelay;          /**< Maxmum sample period allowed in microseconds, not support now */
    uint32_t sampleElementlen; /**<  one sample len */
    uint32_t maxBatchCount;    /**< Maxmum number of data of this sensor that could be batched */
    uint32_t reserved;         /**< Reserved fields */
} SensorCapability;

/* 物理传感器数据结构定义，注册时需要实现 */
typedef struct {
    enum SensorTypeTag type; /* 传感器类型 */
    const char *name;        /* sensor 信息 “ACCEL” “GYRO”等 */
    SensorCapability caps;
    const void *privateData; /* sensor 的私有信息，私有接口 */
    uint32_t privateDataLen; /* sensor 的私有信息长度 */
    /* 方法 */
    int32_t (*init)(void);                        /* 上电，sensor初始化 */
    int32_t (*deinit)(void);                      /* 下电，去初始化 */
    int32_t (*open)(SensorPara *para);            /* 传感器开 */
    int32_t (*close)(void);                       /* 传感器关 */
    int32_t (*read)(uint8_t *data, uint32_t len); /* 从传感器读数据 */
    /* 其他io操作 */
    int32_t (*ioctl)(SensorIoctlCmd cmd, void *data, uint32_t len);
} Hsensor;

/* 虚拟传感器（算法）数据结构定义，注册时需要实现 */
typedef struct {
    enum SensorTypeTag type;      /* 虚拟传感器类型，注册时写明 */
    const char *name;             /* 虚拟传感器信息，注册时写明 */
    const void *privateData;      /* 虚拟传感器的私有信息 */
    uint32_t privateDataLen;      /* 虚拟传感器的私有信息长度 */
    RelySensorsInfo *relySensors; /* 注册时写明，写清依赖的传感器 */

    int32_t (*registDataCb)(RecordDataCallback cb);   /* 注册结果回调函数 */
    int32_t (*unRegistDataCb)(RecordDataCallback cb); /* 取消注册结果回调函数 */
    /* 方法 */
    int32_t (*open)(SensorPara *para); /* 算法开，模式设置 */
    int32_t (*close)(void);            /* 算法关 */
    /* 算法接收传感器（包括依赖的算法）数据的接口 ts 微秒 */
    int32_t (*process)(enum SensorTypeTag type, uint8_t *data, uint64_t ts, uint32_t len);
    int32_t (*ioctl)(SensorIoctlCmd cmd, void *data, uint32_t len);
} Vsensor;

#endif /* HDI_SENSOR_TYPEDEF_H */
