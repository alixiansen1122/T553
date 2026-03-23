/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: nmea parse
 * Create: 2024-11-11
 */
#ifndef GNSS_NMEA_PARSE_H
#define GNSS_NMEA_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef GNSS_ERR
#define GNSS_ERR                    (1)  /* 出错 */
#endif

#ifndef GNSS_OK
#define GNSS_OK                     (0)  /* 正确 */
#endif

#define GNSS_NMEA_FRAME_MAX_SIZE    256
#define GNSS_SAT_NUM_MAX       64
#define GNSS_SAT_IN_MSG        4
#define GNSS_NSAT_MSGS         (GNSS_SAT_NUM_MAX / GNSS_SAT_IN_MSG)

#define GNSS_KNOTS              (0.514444 * 100) // knots to m/s
#define GNSS_STRAIGHT_ANGLE      (180.0)

#define NMEA_SENTENCE_START_BIT     (3)     // $-- 正式语句从第四位开始
#define NMEA_SENTENCE_TYPE_BIT      (3)     // 语句类型标识长度
#define NMEA_SAT_TYPE_BIT      (2)     // 卫星类型类型标识长度

#define GNSS_DIR_NORTH          'N'
#define GNSS_DIR_SOUTH          'S'
#define GNSS_DIR_EAST           'E'
#define GNSS_DIR_WEST           'W'

#define NMEA_FIELD_TIME_SIZE     32
#define GNSS_FRAME_START_FLAG        '$'
#define GNSS_FRAME_CRC_FLAG          '*'
#define GNSS_FRAME_SUPPORT_NUM        4
#define GNSS_FILEDS_IN_GGA            14
#define GNSS_FILEDS_IN_VTG            9

#define NMEA_GSA_FILEDS_CNTS_BEFORE_V41  17
#define NMEA_GSA_FILEDS_CNTS_AFTER_V41  18

#define GNSS_DIR_NSOUTH               0x1
#define GNSS_DIR_EWEST                0x2

#define GNSS_FRAME_GGA               0x0
#define GNSS_FRAME_GSA               0x1
#define GNSS_FRAME_GSV               0x2
#define GNSS_FRAME_RMC               0x3
#define GNSS_FRAME_VTG               0x4
#define GNSS_FRAME_NUM               0x5

#define NMEA_RMC_FILELDS_CNTS_AFTER_V41      13
#define NMEA_RMC_FILELDS_CNTS_AFTER_V23      12
#define NMEA_RMC_FILELDS_CNTS_BEFORE_V23     11
#define NMEA_RMC_DATE_LIMIT                  999999

#define ILLEAGECHAR_MIN     32
#define ILLEAGECHAR_MAX     126

#define TIME_BIT_NUM_0  0
#define TIME_BIT_NUM_1  1
#define TIME_BIT_NUM_2  2
#define TIME_BIT_NUM_3  3
#define TIME_BIT_NUM_MAX 4

#define NMEA_SATE_NUM_BIT_0 0
#define NMEA_SATE_NUM_BIT_1 1
#define NMEA_SATE_NUM_BIT_2 2
#define NMEA_SATE_NUM_BIT_3 3
#define NMEA_SATE_NUM_BIT_4 4
#define NMEA_SATE_NUM_BIT_5 5
#define NMEA_SATE_NUM_BIT_6 6
#define NMEA_SATE_NUM_BIT_7 7
#define NMEA_SATE_NUM_BIT_8 8
#define NMEA_SATE_NUM_BIT_9 9
#define NMEA_SATE_NUM_BIT_10 10
#define NMEA_SATE_NUM_BIT_11 11

#define NMEA_NINE   9
#define NMEA_TEN    10
#define NMEA_YEAR_MIN   2000
#define NMEA_YEAR_MAX   2090
#define NMEA_MONTH_MAX  12
#define NMEA_DAY_MAX    31
#define NMEA_NUM_10000  10000
#define NMEA_NUM_100  100
#define NMEA_NUM_90  90
#define NUMEA_YEAR_1900 1900
#define NUMEA_YEAR_2000 2000
#define NMEA_HOUR_MAX 24
#define NMEA_MIN_MAX 60
#define NMEA_SEC_MAX 60
#define NMEA_ELE_MIN    (-90)
#define NMEA_ELE_MAX    90
#define NMEA_AZ_MAX     360
#define NMEA_CNR_MAX    99
#define NMEA_CRC_MAX 3
#define NMEA_CRC_BIT_0 0
#define NMEA_CRC_BIT_1 1
#define NMEA_CRC_BIT_2 2
#define NMEA_HEX    16
#define NMEA_ONE_SENTENCE_FOUR_SAT  4
#define NMEA_GSV_OTHER_BIT  3
#define NMEA_ERROR  (4.0) // 估算误差

typedef enum {
    GNSS_SYSTEM_INVALID     = 0,
    GNSS_SYSTEM_GPS         = 1,
    GNSS_SYSTEM_GLONASS     = 2,
    GNSS_SYSTEM_GALILEO     = 3,
    GNSS_SYSTEM_BDS         = 4,
    GNSS_SYSTEM_QZSS        = 5,

    GNSS_SYSTEM_MAX
} GnssSystem;

typedef enum {
    GNSS_SIG_BAD            = 0,     // invaliad
    GNSS_SIG_LOW            = 1,     // fix
    GNSS_SIG_MID            = 2,     // differential
    GNSS_SIG_HIGH           = 3,     // sensitive
    GNSS_SIG_RTK            = 4,     // real time kinematic
    GNSS_SIG_FLRTK          = 5,     // float rtk
    GNSS_SIG_ESTIM          = 6,     // estimated
    GNSS_SIG_MAN            = 7,     // manual input mode
    GNSS_SIG_SIM            = 8,     // simulation mode
} GnssSignal;

typedef enum {
    GNSS_OPER_BAD            = 1,
    GNSS_OPER_2D             = 2,
    GNSS_OPER_3D             = 3,
} GnssOperation;

typedef enum {
    GNSS_NMEA_FRAME_GGA        = 0,
    GNSS_NMEA_FRAME_GSA        = 1,
    GNSS_NMEA_FRAME_GSV        = 2,
    GNSS_NMEA_FRAME_RMC        = 3,
    GNSS_NMEA_FRAME_VTG        = 4,
    GNSS_NMEA_FRAME_GLL        = 5,
    GNSS_NMEA_FRAME_ZDA        = 6,
    GNSS_NMEA_FRAME_TH000      = 7,
    GNSS_NMEA_FRAME_TH001      = 8,
    GNSS_NMEA_FRAME_TH002      = 9,
    GNSS_NMEA_FRAME_MAX
} GnssNmeaFrameType;

typedef enum {
    GNSS_NMEA_FIELD_TYPE_SMASK          = 0x1,
    GNSS_NMEA_FIELD_TYPE_SIGNAL         = 0x2,
    GNSS_NMEA_FIELD_TYPE_LAT            = 0x4,
    GNSS_NMEA_FIELD_TYPE_LON            = 0x8,
    GNSS_NMEA_FIELD_TYPE_ANTALT         = 0x10,
    GNSS_NMEA_FIELD_TYPE_OPERMODE       = 0x20,
    GNSS_NMEA_FIELD_TYPE_PDOP           = 0x40,
    GNSS_NMEA_FIELD_TYPE_HDOP           = 0x80,
    GNSS_NMEA_FIELD_TYPE_VDOP           = 0x100,
    GNSS_NMEA_FIELD_TYPE_SPEED          = 0x200,
    GNSS_NMEA_FIELD_TYPE_TRACK          = 0x400,
    GNSS_NMEA_FIELD_TYPE_MTRACK         = 0x800,
    GNSS_NMEA_FIELD_TYPE_MAGVAR         = 0x1000,
    GNSS_NMEA_FIELD_TYPE_UTCDATE        = 0x2000,
    GNSS_NMEA_FIELD_TYPE_UTCTIME        = 0x4000,
    GNSS_NMEA_FIELD_TYPE_SATUSEDNUM     = 0x8000,
    GNSS_NMEA_FIELD_TYPE_SATINUSE       = 0x10000,
    GNSS_NMEA_FIELD_TYPE_SATINVIEW      = 0x20000,
    GNSS_NMEA_FIELD_TYPE_HORIZONTAL_ACCURACY        = 0x40000,
    GNSS_NMEA_FIELD_TYPE_VERTICAL_ACCURACY          = 0x80000,
    GNSS_NMEA_FIELD_TYPE_SPEED_ACCURACY             = 0x100000,
    GNSS_NMEA_FIELD_TYPE_NORTH_VELOCITY             = 0x200000,
    GNSS_NMEA_FIELD_TYPE_EAST_VELOCITY              = 0x400000,
    GNSS_NMEA_FIELD_TYPE_DESCEND_VELOCITY           = 0x800000,
} GnssNmeaFieldType;

typedef struct {
    unsigned short year;                     // 年 1900开始
    unsigned char month;                      // 月 0-11
    unsigned char day;                        // 日 1-31
    unsigned char hour;                       // 时 0-23
    unsigned char min;                        // 分 0-59
    unsigned char sec;                        // 秒 0-59
    unsigned char centisec;                 // 百分秒 0-99
} GnssTime;

typedef struct {
    unsigned int svid;                      // 卫星编号
    short ele;                              // 卫星仰角 -90 ~ 90度
    unsigned short az;                    // 地平经度，0-360，从某点的指北方向依顺时针到目标方向之间的水平夹角
    unsigned short cnr;                   // 载躁比，信号强度 0-99dB
} GnssSatelliteStatus;

typedef struct {
    unsigned int satUsedCnt;                           // 当前使用的卫星数目
    unsigned int satUsedID[GNSS_SAT_NUM_MAX];      // 当前使用的卫星数目ID
    unsigned int satViewedCnt;                        // 可视的卫星数目
    GnssSatelliteStatus satelliteStatus[GNSS_SAT_NUM_MAX];     // 可视的卫星信息
} GnssSatelliteStatusInfo;

typedef struct {
    GnssSatelliteStatusInfo gps;
    GnssSatelliteStatusInfo bds;
    GnssSatelliteStatusInfo glonass;
    GnssSatelliteStatusInfo galileo;
    GnssSatelliteStatusInfo qzss;
} GnssSatelliteList;

typedef struct {
    unsigned int field;               // 标明结构体哪些元素可用
    unsigned int sentence;            // 标明当前帧类型
    unsigned char signal;                   // 信号质量
    unsigned char smode;                 // 模式
    unsigned char timevalid;                   // 时间有效性
    double  lat;                        // 纬度
    double  lon;                        // 经度
    double  altitude;                  // 天线海拔高度
    float   speed;                     // 速度   千米/小时
    double  pdop;                      // 三维位置精度因子：为纬度、经度和高程等误差平方和的开根号值
    double  hdop;                      // 水平分量精度因子：为纬度和经度等误差平方和的开根号值
    double  vdop;                      // 垂直分量精度因子
    double  trackAng;                  // 跟踪角
    double  magneticTrackAng;          // 磁轨角
    double  magVariation;              // 磁场变化度
    // Calculated Velocity Components
    // 计算后的速度分量
    float velocityNorth;    // Northward Velocity (m/s)
                              // 向北速度 (米/秒)
    float velocityEast;     // Eastward Velocity (m/s)
                              // 向东速度 (米/秒)
    float velocityDescend;  // Descent Velocity (m/s)
                              // 下降速度 (米/秒)

    float verticalAccuracy;    // Vertical accuracy estimate in mm
                                   // 垂直精度估计 单位：mm
    float horizontalAccuracy;  // Horizontal accuracy estimate in mm
                                   // 水平精度估计 单位：mm
    float speedAccuracy;       // Speed accuracy estimate in cm/s
                                   // 速度精度估计 单位：cm/s

    GnssTime utc;                      // 世界标准时间
    GnssSatelliteList satelist;            // 卫星信息
} GnssInfo;

typedef struct {
    unsigned int  fieldType;                       // 标明结构体哪些元素可用
    unsigned int  msgCnt;
    unsigned int  msgIdx;
    unsigned int  satViewedCnt;                       // 可视的卫星数目
    GnssSatelliteStatus satelliteStatus[GNSS_SAT_IN_MSG];
} GnssNmeaGSV;

typedef struct {
    unsigned int  fieldType;               //  标明结构体哪些元素可用
    double  latitude;                  // 纬度
    double  longitude;                 // 经度
    char    nSouth;                    // 'N' or 'S'
    char    eWest;                     // 'E' or 'W'
    unsigned int  signal;              // 信号质量
    unsigned int  satUsedCnt;          // 当前使用的卫星数目
    double  hdop;                      // 水平分量精度因子：为纬度和经度等误差平方和的开根号值
    double  antAltitude;               // 天线海拔高度
    char    antAltUnit;                // 'M'
    double  geoidalSep;                // 大地水平面至正常椭球面的垂直距离
    char    geoidalSepUnit;            // 'M'
    double  dGPSTimeSinceLast;         // DGPS上次更新后经过的时间
    unsigned int  dGPSId;           // DGPS站ID
    GnssTime utc;                   // 世界标准时间
} GnssNmeaGGA;

typedef struct {
    unsigned int  fieldType;                   // 标明结构体哪些元素可用
    unsigned int  operMode;
    unsigned char operType;                     // 1 = Fix not available; 2 = 2D; 3 = 3D
    unsigned int  satPrn[GNSS_SAT_NUM_MAX];    // PRNs of satellites
    double  pdop;                          // 三维位置精度因子：为纬度、经度和高程等误差平方和的开根号值
    double  hdop;                          // 水平分量精度因子：为纬度和经度等误差平方和的开根号值
    double  vdop;                          // 垂直分量精度因子
    int     systemId;                       // nmea 4.1版本新增
} GnssNmeaGSA;

typedef struct {
    unsigned int fieldType;               // 标明结构体哪些元素可用
    char    curStat;                   //  A = active or V = void
    double  latitude;                  // 纬度
    double  longitude;                 // 经度
    char    nSouth;                    // 'N' or 'S'
    char    eWest;                     // 'E' or 'W'
    double  speedN;                    // 速度   节 knots
    double  trackAng;                  // 跟踪角
    double  magVariation;              // 磁场变化度
    char    magVarEW;                  // 'E' or 'W'
    char    mode;                      // A=autonomous, D=differential, E=Estimated, N=not valid, S=Simulator
    char    navStatus;                  // nmea4.1及以上版本,导航状态标识符(V表示系统不输出导航状态信息)
    GnssTime utc;                   // 世界标准时间
} GnssNmeaRMC;

typedef struct {
    unsigned int fieldType;               // 标明结构体哪些元素可用
    double  trackAng;                  // 跟踪角
    double  magneticTrackAng;          // 磁轨角
    double  dSpeedN;                    // 速度   节 knots
    double  speed;                     // 速度   千米/小时
    char    trackT;                    // 'T'
    char    trackM;                   // 'M'
    char    cSpeedN;                    // 'N'
    char    speedK;                    // 'K'
    char    posModeA;                  // 'A'
} GnssNmeaVTG;

typedef struct {
    union {
        GnssNmeaGGA    nmeaGGA;
        GnssNmeaGSA    nmeaGSA;
        GnssNmeaGSV    nmeaGSV;
        GnssNmeaRMC    nmeaRMC;
        GnssNmeaVTG    nmeaVTG;
    } nmeaFrame;

    unsigned int length;
    char nmeaBuf[GNSS_NMEA_FRAME_MAX_SIZE];
} GnssNmeaFrameProcess;

void GnssNmeaInit();

GnssNmeaFrameType GnssNmeaGetFrameType(const char* nmeaBuf, unsigned int length);
    
int GnssNmeaDecodeFrame(const char* nmea, unsigned int nmeaLen, GnssInfo* gnssInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif