/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: virtual camera header
 * Author: media_develop team
 * Create: 20211012
 */

#ifndef HW_CAMERA_TYPES_H
#define HW_CAMERA_TYPES_H
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define CAMERA_FPS_MAX_NUM  16
#define CAMERA_DESC_MAX_LEN 32
#define INFO_MAX_LEN 1024
#define DESC_MAX_LEN 64
#define AUTO_MODE_MAX_NUM 16
#define PRIVATE_META_MAX_LEN 32

typedef void* HwCameraVoidPtr;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} RectInfo;

typedef enum {
    STREAM_INFO_ATTR = 0,
    STREAM_INFO_POS,
    STERAM_INFO_PRIVATE,
    STREAM_INFO_BUTT,
} StreamInfoType;

typedef enum {
    CAMERA_CONTROL_3A_MODE = 0,
    CAMERA_CONTROL_PRIVATE,
    CAMERA_CONTROL_BUTT,
} DeviceType;

typedef enum {
    FORMAT_YVU420 = 0,
    FORMAT_JPEG,
    FORMAT_AVC,
    FORMAT_HEVC,
    FORMAT_RGB_BAYER_12BPP,
    FORMAT_MJPEG,
    FORMAT_IMAGE_BUTT
} ImageFormat;

typedef enum {
    STREAM_PREVIEW = 0,
    STREAM_VIDEO,
    STREAM_CAPTURE,
    STREAM_CALLBACK,
    STREAM_TYPE_BUTT
} StreamType;

typedef struct {
    StreamType type;
    ImageFormat format;
    int32_t width;
    int32_t height;
    uint16_t fps;
    RectInfo crop;
    uint8_t invertMode;
} StreamAttr;

typedef enum {
    CAP_DESC_RANGE,
    CAP_DESC_ENUM,
    CAP_DESC_MAX,
} CapDescType;

typedef struct {
    ImageFormat format;  /**< the format of memory */
    int32_t width;        /**< the width of memory */
    int32_t height;       /**< the heigh of memory */
    int32_t stride0;       /**< the stride of memory */
    int32_t stride1;      /**< the stride1 of memory */
    int32_t size;         /* < size of memory */
    int32_t fd;           /**< buffer fd, -1 if not supported */
    uint32_t flag;         /**< flag */
    int64_t usage;        /**< the usage of memory */
    uint64_t phyAddr;      /**< Physical address */
    uint64_t pts;          /**< pts */
    uint64_t timeStamp;    /**< timestamp */
    void *virAddr;    /**< Virtual address of memory  */
    int32_t attrFd;
    void *attrVirAddr;
    int32_t attrSize;
    int32_t attrOffset;
} HalBuffer;

typedef struct {
    int32_t maxWidth;
    int32_t minWidth;
    int32_t maxHeight;
    int32_t minHeight;
    int32_t maxFps;
    int32_t minFps;
} FormatRange;

typedef struct Format {
    int32_t width;
    int32_t height;
    uint8_t frameRateNum;
    uint8_t frameRate[CAMERA_FPS_MAX_NUM];
} FormatEnum;

typedef struct {
    CapDescType type;
    char description[CAMERA_DESC_MAX_LEN]; /* like YUYV 4:2:2 (YUYV) */
    ImageFormat format;
    union {
        FormatRange range;
        FormatEnum formatEnum;
    } u;
} StreamCap;

typedef enum Status {
    CAMERA_STATUS_NOT_PRESENT,
    CAMERA_STATUS_PRESENT,
    CAMERA_STATUS_BUTT,
} CameraStatus;

typedef void (*BufferAvailable)(uint32_t streamId, const HalBuffer *halBuffer, uint32_t bufferNum, const void *usr);
typedef void (*CameraDetectCb)(uint32_t cameraId, CameraStatus status);

typedef struct {
    DeviceType type;
    union {
        uint8_t data[INFO_MAX_LEN];
        const void *privateData;
    } u;
} DeviceInfo;

typedef struct {
    int32_t x;
    int32_t y;
} PosInfo;

typedef struct {
    StreamInfoType type;
    union {
        uint8_t data[INFO_MAX_LEN];
        StreamAttr attr;
        PosInfo pos;
    } u;
} CameraStreamInfo;

typedef enum {
    ORIENTATION_FRONT,
    ORIENTATION_REAR,
    ORIENTATION_OTHER,
    ORIENTATION_BUTT,
} CameraOrientation;

typedef enum {
    AE_MODE_ON,
    AE_MODE_OFF,
    AE_BUTT,
} CameraAEMode;

typedef enum {
    AF_MODE_AUTO,
    AF_MODE_OFF,
    AF_BUTT,
} CameraAFMode;

typedef enum {
    AWB_MODE_AUTO,
    AWB_MODE_OFF,
    AWB_BUTT,
} CameraAWBMode;

typedef enum {
    CAM_TYPE_WIDE_ANGLE,
    CAM_TYPE_FISH_EYE,
    CAM_TYPE_TRUE_DEAPTH,
    CAM_TYPE_BUTT,
} CameraType;

typedef struct {
    CameraOrientation orientation;
    uint8_t aeModeNum;
    uint8_t afModeNum;
    uint8_t awbModeNum;
    CameraAEMode aeModes[AUTO_MODE_MAX_NUM];
    CameraAFMode afModes[AUTO_MODE_MAX_NUM];
    CameraAWBMode awbModes[AUTO_MODE_MAX_NUM];
    CameraType type;
    char cameraDesc[DESC_MAX_LEN];
} AbilityInfo;

typedef struct {
    CameraAEMode aeMode;
    CameraAFMode afMode;
    CameraAWBMode awbMode;
    uint32_t privateData[PRIVATE_META_MAX_LEN];
} CameraMetaResult;

typedef void (*CameraResultCb)(uint32_t cameraId, CameraMetaResult result);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
