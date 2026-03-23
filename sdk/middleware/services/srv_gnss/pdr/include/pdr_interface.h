/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: pdr Interface
 * Version: v0.0
 * Create: 2023-2-25
 */
#ifndef PDR_INTERFACE_H
#define PDR_INTERFACE_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

// 放大倍数
#define PVT_REPORT_SCALE            (1 << 24) // PVT上报位置的放大系数
#define COMMON_SCALE                (1 << 20) // PVT上报速度的放大系数

// PDR返回状态列表
#define RET_OK                      0
#define RET_NULL_POINTER            (-1001)
#define RET_CALLBACK_INVALID        (-2001)
#define RET_VERSION_INVALID         (-3001)

// PDR输出数据的来源
#define GNSS_ONLY               0
#define PDR_ONLY                1
#define PDR_FUSION              2

// 1.传感器相关
#define ACCL_GYRO_DATA_TS       0.01    // 加计，陀螺数据采样周期 10 ms
#define SENSOR_BUFFER_SIZE      100     // 存放传感器一次数据的buffer长度
#define AXIS_NUM                3       // 传感器3轴

typedef struct {
    int64_t timestamp;  // 时间戳，单位：ms
    float   x;          // 传感器X轴数据
    float   y;          // 传感器Y轴数据
    float   z;          // 传感器Z轴数据
    int8_t  status;     // 传感器状态标志位
} SensorDataPara;

typedef struct {
    SensorDataPara sensors[SENSOR_BUFFER_SIZE]; // 传感器值
    uint32_t cnt;                               // 每次上报的个数
} SensorData;

typedef struct {
    SensorData accl;    // 加计
    SensorData gyro;    // 陀螺仪
    SensorData mag;     // 磁力计
} SensorDataSet;

typedef enum {
    SENSOR_INVALID = 0, // sensor 数据更新失败
    SENSOR_SYNC_SUCC,   // sensor 数据更新、时间同步成功
    SENSOR_SYNC_FAIL    // sensor 数据更新、时间同步失败
} SensorSts;

typedef struct {
    SensorSts sensorSts;            // sensor数据更新状态
    int64_t utcMs;                  // GNSS UTC 时间戳, 单位：毫秒， 从 January 1, 1970起
    SensorDataSet sensorDataSet;    // 传感器数据集
} EpochSensorData;

#pragma pack(1)
// 2.GNSS结果相关
typedef struct {
    uint16_t flags;          // bit0:validity of timestamp(1:valid, 0:invalid),...,bit11:validity of speedAcc.
    uint64_t timestamp;      // 时间戳utc，unix时间，单位ms，从 January 1, 1970起
    int64_t latitude;        // 单位：度；放大2^24倍
    int64_t longitude;       // 单位：度；放大2^24倍
    int64_t clkBias;         // 单位：m；放大2^20倍
    int64_t clkDrift;        // 单位：hz；放大2^20倍
    int64_t speede;          // 单位：m/s；放大2^20倍
    int64_t speedn;          // 单位：m/s；放大2^20倍
    int64_t speedu;          // 单位：m/s；放大2^20倍
    float altitude;          // 单位：m
    float heading;           // 单位：弧度
    float posAcc;            // 位置精度，单位：m
    float speedAcc;          // 速度精度，单位：m/s
} GnssLocInfo;               // PVT上报的定位定速结果

typedef struct {
    uint32_t deltaTime;     // 收到观测量到pvt结束的时间差，单位: s, 放大2^24倍
    uint8_t context;        // 场景识别结果(0：默认，1：隧道，2：遮挡，3：CBD，4：OPEN，5：室内，6：半面天空，7：室内窗边)
    uint8_t isStatic;       // 是否静态
    uint8_t pvtState;       // PVT的状态(0：LSQ，1：KF)
    uint16_t inuseNum;      // 使用的PSR观测量的个数
    float qop;              // 定位性能质量，单位：米。数值越小定位越准确。
    float hdop;             // 水平位置精度因子
    float ems;              // 伪距残差ems
} GnssPdrCommInfo;          // PVT上报的定位定速过程状态
#pragma pack()

typedef struct {
    GnssLocInfo locInfo;
    GnssPdrCommInfo gnssState;
} GnssRptRslt;              // PVT上报信息

// 3.PDR输出相关
typedef struct {
    double lat;     // 纬度，单位: 度
    double lon;     // 经度，单位: 度
    float alt;      // 高度，单位: 米
    bool valid;     // 是否有效
} LlaCoord;         // 大地坐标系

typedef struct {
    uint8_t src;            // PDR输出的数据来源。0: GNSS_ONLY, 1: PDR_ONLY, 2: PDR_FUSION
    uint64_t timeStampMs;   // 时间戳utc，unix时间，单位ms，从 January 1, 1970起
    float speed;            // 速度，单位：米/秒
    float heading;          // 航向，单位：度
    LlaCoord posLla;        // 位置 lla
} PdrResult;                // PDR输出

#ifdef _WIN32
#define PDR_INTERFACE_EXPORT __declspec(dllexport)
#elif __linux__
#define PDR_INTERFACE_EXPORT __attribute__ ((visibility("default")))
#else
#define PDR_INTERFACE_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 对外提供的接口 */
// 获取PDR版本号。
PDR_INTERFACE_EXPORT const char* GetPdrVersion(void);

// 初始化PDR，正常返回0，异常返回错误码。
PDR_INTERFACE_EXPORT int32_t InitPdrFusion(void);

/* 函数功能：PDR主流程，正常返回0，异常返回错误码。
 * 函数参数：
 * sensorData  入参  传感器数据，接口内部可能改写数据
 * gnssRslt    入参  GNSS结果
 * arState     入参  运动状态识别结果
 * pdrRslt     出参  保存pdr结果
 * 返回值  ：正常返回0，异常返回错误码。
 */
PDR_INTERFACE_EXPORT int32_t ExecutePdrFusion(EpochSensorData *sensorData, const GnssRptRslt *gnssRptRslt,
    PdrResult *pdrRslt);

// 停止PDR，正常返回0，异常返回错误码。
PDR_INTERFACE_EXPORT int32_t QuitPdrFusion(void);

// （可选）日志回调函数声明，调用方按此声明实现回调函数，并注册
typedef int (*HipdrLogFunc)(const char *logLevel, const char* fmt, ...);
PDR_INTERFACE_EXPORT int32_t RegPdrLogHandle(HipdrLogFunc cbFunc);

#ifdef __cplusplus
}
#endif
#endif