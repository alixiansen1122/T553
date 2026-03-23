/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: hsensor mgr
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef HSENSOR_MGR_SERVICE_H
#define HSENSOR_MGR_SERVICE_H
#include "sensor_typedef.h"
#include "soc_osal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HSENSOR_LISTENER_NAME_MAX_LEN 64

typedef enum {
    SENSOR_LISTENER_REGISTER,
    SENSOR_LISTENER_UNREGISTER,
    SENSOR_LISTENER_ENABLE,
    SENSOR_LISTENER_DISABLE,
    SENSOR_LISTENER_WORKING,
} HsensorListenerStatus;

typedef struct {
    enum SensorTypeTag type;
    uint8_t *data;
    uint32_t dataLen;
    uint32_t option;
    uint64_t timestamp;
} SensorNotifyInfo;

typedef void (*NotifyCallback)(const SensorNotifyInfo *info, void *notifyArg);

typedef struct {
    struct osal_list_head list;
    char name[HSENSOR_LISTENER_NAME_MAX_LEN];
    enum SensorTypeTag type;
    uint16_t bufIndex;     /* listener的缓存索引 */
    uint16_t sampleCnt;    /* 传感器的采样次数 */
    uint16_t dispatchCnt;  /* 数据分发间隔次数 */
    uint32_t notifyPeriod; /* listener数据需求周期 ms */
    SensorPara openParam;
    NotifyCallback notifyCallback;
    void *notifyArg;
    RecordDataCallback resultDataCb;
    HsensorListenerStatus status;
} HsensorListener;

typedef enum {
    HSENSOR_SCB_UNINITED,
    HSENSOR_SCB_INITED,
} HsensorScbStatus;

typedef struct HsensorScb {
    struct osal_list_head list;         /* scb link point */
    struct osal_list_head listenerList; /* scb listener head */
    uint16_t listenerNum;
    enum SensorTypeTag type; /* scb sensor type */
    HsensorScbStatus status;
    uint8_t *dataBuf;          /* scb buffer */
    uint8_t *batchDataBuf;     /* scb batch buffer */
    uint32_t batchDataBufSize; /* scb batch buffer length */
    uint32_t dataBufSize;      /* scb buffer length */
    uint32_t dataPackSize;     /* scb data len for every batch */
    uint32_t dataBufIndex;     /* scb buffer current index */
    uint32_t reportPeriod;     /* 传感器采样的上报周期 */
    uint32_t batchCount;       /* scb buffer current index */
    uint64_t sysTick;
    SensorPara decisionPara; /* scb decision param */
    Hsensor *hsensor;
} HsensorScb;

typedef struct {
    struct osal_list_head hsensorScbHead;
    uint32_t hsensorScbNum;
    uint8_t *dataBuf;
    uint8_t *dataDispatchBuf;
    bool inited;
} HsensorMgr;

int32_t HsensorMgrInit(void);
int32_t HsensorMgrRegister(const Hsensor *hsensor);
int32_t HsensorMgrRegisterListener(HsensorListener *listener);
int32_t HsensorMgrUnRegisterListener(HsensorListener *listener);
int32_t HsensorMgrOpenListener(HsensorListener *listener);
int32_t HsensorMgrCloseListener(HsensorListener *listener);
HsensorScb *HsensorMgrHsensorScbFind(const enum SensorTypeTag type);

int32_t HsensorMgrIoctl(enum SensorTypeTag type, SensorIoctlCmd cmd, void *data, uint32_t len);
int32_t HsensorMgrReadData(const enum SensorTypeTag type, const uint64_t *ts);
void HsensorMgrPrintListener(enum SensorTypeTag type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif