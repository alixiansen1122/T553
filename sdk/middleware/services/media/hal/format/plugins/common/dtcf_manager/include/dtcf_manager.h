/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: dtcf manager interface
* Author: Media Software Group
* Create: 2025-03-10
*/

#ifndef DTCF_MANAGER_H
#define DTCF_MANAGER_H
#include <stdint.h>
#include "media_hal_common.h"
#include "dtcf.h"

#define MW_CHECK_RETURN(s32Ret) \
    do { \
        if ((s32Ret) != MEDIA_HAL_OK) { \
            return s32Ret; \
        } \
    } while (0)

#define REC_FILE_NAME_LEN (256)

typedef enum {
    MEMORY_REACH_THE_MAXIMUM_VALUE,  /* The memory reaches the maximum value. */
    DELETE_THE_OLDEST_FAILED,        /* Failed to delete the oldest file.. */
    MEMORY_MONITOR_ERROR,            /* Other memory monitoring errors */
} DtcfCallBackType;

typedef struct {
    void *handle;
    int32_t (*CallBack)(void *handle, DtcfCallBackType type, int32_t extra);
} DtcfCallBack;

typedef struct {
    uint32_t fileDateYear;
    uint32_t fileDateMonth;
    uint32_t fileDateDay;
    uint32_t fileDateHour;
    uint32_t fileDateMin;
    uint32_t fileDateSec;
} FileDate;

int32_t DtcfMgrFileOptInit(const char *path, uint32_t len, DtcfCallBack callBack);
int32_t DtcfMgrRequestFileNames(DTCF_FILE_TYPE_E fileType, DTCF_DIR_E dirType,
    char (*paszFileNames)[REC_FILE_NAME_LEN]);
int32_t DtcfMgrFileOptDeInit(void);
int32_t DtcfMgrGetStorageMaxSize(uint64_t *size);
int32_t DtcfMgrHandleRecordMaxSize(uint64_t currentSize, uint64_t recordMaxSize);
uint64_t DtcfMgrGetCurrentRecordedSize(void);
uint64_t DtcfMgrGetCurrentMemorySize(uint64_t maxMemory);
void DtcfMgrSetDeleteFileEnable(bool enable);
void DtcfMgrSetMaxStorageThreshold(uint32_t threshold);
int32_t DtcfMgrGetCurrentFileDate(FileDate *fileDate);
bool DtcfMgrIsFile(const char *path, uint32_t len);
int32_t DtcfMgrMakeDir(const char *path, uint32_t len);
#endif