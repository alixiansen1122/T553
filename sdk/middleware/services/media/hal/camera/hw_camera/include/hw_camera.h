/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: virtual camera header
 * Author: media_develop team
 * Create: 20211012
 */

#ifndef HW_CAMERA_H
#define HW_CAMERA_H
#include <hw_camera_types.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

int32_t HwCameraInit(void);
int32_t HwCameraDeinit(void);
int32_t HwCameraGetDeviceNum(uint8_t *num);
int32_t HwCameraGetDeviceList(uint32_t *cameraList, uint8_t listNum);
int32_t HwCameraDeviceOpen(uint32_t cameraId);
int32_t HwCameraDeviceClose(uint32_t cameraId);
int32_t HwCameraStreamsCreate(uint32_t cameraId, const StreamAttr *streamList, uint32_t *streamIdList, uint8_t num);
int32_t HwCameraStreamsDestroy(uint32_t cameraId, const uint32_t *streamIdList, uint8_t num);
int32_t HwCameraStreamOn(uint32_t cameraId, uint32_t streamId);
int32_t HwCameraStreamOff(uint32_t cameraId, uint32_t streamId);
int32_t HwCameraQueueBuf(uint32_t cameraId, uint32_t streamId, HalBuffer *buffer, uint8_t num);
int32_t HwCameraSetBufferCallback(uint32_t cameraId, const BufferAvailable callback, const HwCameraVoidPtr userData);

/* 以下接口可支持返回（空实现） */
int32_t HwCameraStreamSetInfo(uint32_t cameraId, uint32_t streamId, const CameraStreamInfo *info);
int32_t HwCameraStreamGetInfo(uint32_t cameraId, uint32_t streamId, CameraStreamInfo *info);
int32_t HwCameraDeviceSetInfo(uint32_t cameraId, const DeviceInfo *info);
int32_t HwCameraDeviceGetInfo(uint32_t cameraId, DeviceInfo *info);
int32_t HwCameraGetAbility(uint32_t cameraId, AbilityInfo *ability);
int32_t HwCameraSetDeviceDetectCb(const CameraDetectCb cb);
int32_t HwCameraGetStreamCapNum(uint32_t cameraId, uint32_t *num);
int32_t HwCameraGetStreamCap(uint32_t cameraId, StreamCap *streamCap, uint32_t streamNum);
int32_t HwCameraGetDeviceId(uint32_t cameraId, uint32_t streamId, uint32_t *deviceId);
int32_t HwCameraGetModeNum(uint8_t *num);
int32_t HwCameraSetMode(uint8_t index);
int32_t HwCameraSetResultCb(uint32_t cameraId, CameraResultCb cb);
int32_t HwCameraDequeueBuf(uint32_t cameraId, uint32_t streamId, HalBuffer *buffer);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
