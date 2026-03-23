/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: dtcf manager
* Author: Media Software Group
* Create: 2025-03-10
*/
#include "dtcf_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include "media_hal_thread_adapt.h"
#include "dtcf.h"

#define MODULE_NAME "DtcfManager"
#define BYTE_UNIT 1024
#define FILE_NAME_SIZE 1024
#define NORM_DIR_NUM 4
#define EMR_DIR_NUM 4
#define RM_RECORDERED_START_RATIO 20 /* 20 / 100 */
#define RM_RECORDERED_END_RATIO 30 /* 20 / 100 */
#define PERSENTAGE 100.0
#define DEFALUT_REMAIN_SPACE_THESHOLD 0.9
#define MEMORY_CHECK_INTERVAL_MS 5000
#define FILE_PATH_LEN_MAX         (256)

typedef struct {
    bool fileOptInited;
    float remainSpaceThreshold;
    bool overwriteOldestFiles;
    uint32_t fileAmount;
    DtcfCallBack callBack;
    char path[FILE_PATH_LEN_MAX];
    MediaHalMutexHandle mutex;
    uint64_t currentRecordedSize;
    FileDate fileDate;
    DTCF_DIR_E dirType;
} FileOptContext;

const uint64_t MEMORY_STORAGE_DEFAULT_MAX_SIZE = 8ULL * 1024 * 1024 * 1024;
static FileOptContext g_fileOptContext = {};

int32_t DtcfMgrGetStorageMaxSize(uint64_t *size)
{
#if defined(ONLY_UT_TEST)
    *size = MEMORY_STORAGE_DEFAULT_MAX_SIZE;
    return MEDIA_HAL_OK;
#endif
    struct statfs sfs;
    int result;
    uint64_t freeSize;
    (void)memset_s(&sfs, sizeof(sfs), 0, sizeof(sfs));
    result = statfs("/user/", &sfs);
    if (result != 0 || sfs.f_type == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "[UPG] statfs failed! errno = %d, sfs.f_type = %d", errno, (sfs.f_type));
        MEDIA_HAL_LOGE(MODULE_NAME, "[UPG] statfs failed! Invalid argument! result = %d", result);
        return MEDIA_HAL_ERR;
    }
    freeSize = (uint64_t)sfs.f_bsize * sfs.f_bfree;
    if (freeSize <= MEMORY_STORAGE_DEFAULT_MAX_SIZE && freeSize > 0) {
        *size = freeSize;
    } else {
        *size = MEMORY_STORAGE_DEFAULT_MAX_SIZE;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "max memory size = %llu", *size);
    return MEDIA_HAL_OK;
}

static int32_t OverwriteOldestFile(FileOptContext *context)
{
#ifdef TIME_CONSUMING_PRINTING
    uint32_t startTime;
    uint32_t endTime;
    uint32_t totalTime;
    startTime = uapi_tcxo_get_ms();
#endif
    DTCF_DIR_E azScanDir[DTCF_DIR_BUTT] = {
        DTCF_DIR_RECORD_WAV_FILE,
        DTCF_DIR_RECORD_MP3_FILE,
        DTCF_DIR_RECORD_SILK_FILE,
        DTCF_DIR_RECORD_OGG_FILE,
    };
    int32_t ret = DtcfScanFiles(azScanDir, DTCF_DIR_BUTT, &context->fileAmount);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "DtcfScanFiles fail %d", ret);
        context->callBack.CallBack(context->callBack.handle, DELETE_THE_OLDEST_FAILED, 0);
        return MEDIA_HAL_ERR;
    }
    char path[FILE_PATH_LEN_MAX] = {0};
    ret = DtcfGetOldestFilePath(context->dirType, path, FILE_PATH_LEN_MAX);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SS_DTCF_GetOldestFilePath fail %d", ret);
        return MEDIA_HAL_ERR;
    }
#ifdef TIME_CONSUMING_PRINTING
    endTime = uapi_tcxo_get_ms();
    totalTime = endTime - startTime;
    MEDIA_HAL_LOGI(MODULE_NAME, " DtcfScanFiles totalTime = %ums", totalTime);
    startTime = uapi_tcxo_get_ms();
#endif
    struct stat fileInfo = { 0 };
    ret = stat(path, &fileInfo);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "stat file: %s failed! errno = %d", path, errno);
        return MEDIA_HAL_ERR;
    }
    ret = remove(path);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "remove file %s failed! errno = %d", path, errno);
        return MEDIA_HAL_ERR;
    }
    context->currentRecordedSize -= fileInfo.st_size;
#ifdef TIME_CONSUMING_PRINTING
    endTime = uapi_tcxo_get_ms();
    totalTime = endTime - startTime;
    MEDIA_HAL_LOGI(MODULE_NAME, " remove totalTime = %ums", totalTime);
#endif
    return MEDIA_HAL_OK;
}

int32_t DtcfMgrHandleRecordMaxSize(uint64_t currentSize, uint64_t recordMaxSize)
{
    MediaHalMutexLock(g_fileOptContext.mutex);
    g_fileOptContext.currentRecordedSize += currentSize;
    if (g_fileOptContext.currentRecordedSize >= (recordMaxSize * g_fileOptContext.remainSpaceThreshold)) {
        if (g_fileOptContext.overwriteOldestFiles) {
            int32_t ret = OverwriteOldestFile(&g_fileOptContext);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "OverwriteOldestFile filed!");
                g_fileOptContext.callBack.CallBack(g_fileOptContext.callBack.handle, DELETE_THE_OLDEST_FAILED, 0);
                MediaHalMutexUnLock(g_fileOptContext.mutex);
                return MEDIA_HAL_ERR;
            }
        } else {
            MEDIA_HAL_LOGE(MODULE_NAME, "DtcfMgrHandleRecordMaxSize start");
            g_fileOptContext.callBack.CallBack(g_fileOptContext.callBack.handle, MEMORY_REACH_THE_MAXIMUM_VALUE, 0);
            MEDIA_HAL_LOGE(MODULE_NAME, "DtcfMgrHandleRecordMaxSize end");
        }
    }
    MediaHalMutexUnLock(g_fileOptContext.mutex);
    return MEDIA_HAL_OK;
}

uint64_t DtcfMgrGetCurrentRecordedSize(void)
{
    MediaHalMutexLock(g_fileOptContext.mutex);
    uint64_t currentRecordedSize = g_fileOptContext.currentRecordedSize;
    MediaHalMutexUnLock(g_fileOptContext.mutex);
    return currentRecordedSize;
}

uint64_t DtcfMgrGetCurrentMemorySize(uint64_t maxMemory)
{
    MediaHalMutexLock(g_fileOptContext.mutex);
    uint64_t currentMemorySize = maxMemory * g_fileOptContext.remainSpaceThreshold;
    MediaHalMutexUnLock(g_fileOptContext.mutex);
    return currentMemorySize;
}

void DtcfMgrSetDeleteFileEnable(bool enable)
{
    MediaHalMutexLock(g_fileOptContext.mutex);
    if (g_fileOptContext.fileOptInited) {
        g_fileOptContext.overwriteOldestFiles = enable;
        MEDIA_HAL_LOGI(MODULE_NAME, "set OverwriteOldestFile enable = %d!", enable);
    }
    MediaHalMutexUnLock(g_fileOptContext.mutex);
}

void DtcfMgrSetMaxStorageThreshold(uint32_t threshold)
{
    MediaHalMutexLock(g_fileOptContext.mutex);
    if (g_fileOptContext.fileOptInited) {
        g_fileOptContext.remainSpaceThreshold = threshold / PERSENTAGE;
        MEDIA_HAL_LOGI(MODULE_NAME, "set max storage threshold = %u!", threshold);
    }
    MediaHalMutexUnLock(g_fileOptContext.mutex);
}

int32_t DtcfMgrGetCurrentFileDate(FileDate *fileDate)
{
    CHK_NULL_RETURN(fileDate, MEDIA_HAL_INVALID_PARAM, "fileDate is NULL!");
    struct tm t = {};
    int32_t ret = DtcfGetCurrentFileTime(&t);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get current file time failed!");
        return MEDIA_HAL_ERR;
    }
    fileDate->fileDateYear = t.tm_year + 1900;  /* the correct time need to plus 1900 */
    fileDate->fileDateMonth = t.tm_mon + 1;
    fileDate->fileDateDay = t.tm_mday;
    fileDate->fileDateHour = t.tm_hour;
    fileDate->fileDateMin = t.tm_min;
    fileDate->fileDateSec = t.tm_sec;
    return MEDIA_HAL_OK;
}

int32_t FileOptInitContext(const char *path, uint32_t len, DtcfCallBack callBack)
{
    g_fileOptContext.callBack = callBack;
    g_fileOptContext.fileOptInited = false;
    g_fileOptContext.overwriteOldestFiles = true;
    g_fileOptContext.remainSpaceThreshold = DEFALUT_REMAIN_SPACE_THESHOLD;
    g_fileOptContext.fileAmount = 0;
    g_fileOptContext.mutex = MediaHalMutexCreate(NULL);
    if (g_fileOptContext.mutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "MediaHalMutexCreate failed!");
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(g_fileOptContext.path, FILE_PATH_LEN_MAX, path, len) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t DtcfMgrFileOptInit(const char *path, uint32_t len, DtcfCallBack callBack)
{
    if (g_fileOptContext.fileOptInited) {
        return MEDIA_HAL_OK;
    }
    if (path == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input path is NULL");
        return MEDIA_HAL_ERR;
    }
    int32_t ret = FileOptInitContext(path, len, callBack);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "file init context failed!");
        return ret;
    }
    ret = DtcfInit(path);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SS_DTCF_Init fail %d", ret);
        return MEDIA_HAL_ERR;
    }
    g_fileOptContext.fileOptInited = true;
    return MEDIA_HAL_OK;
}

int32_t DtcfMgrFileOptDeInit(void)
{
    if (!g_fileOptContext.fileOptInited) {
        return MEDIA_HAL_OK;
    }
    g_fileOptContext.fileOptInited = false;
    MediaHalMutexDestroy(&g_fileOptContext.mutex);
    return DtcfDeInit();
}

int32_t DtcfMgrRequestFileNames(DTCF_FILE_TYPE_E fileType, DTCF_DIR_E dirType,
    char (*paszFileNames)[REC_FILE_NAME_LEN])
{
    int32_t s32Ret = CheckToMkdirForInit(dirType);
    if (s32Ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CheckToMkdirForInit failed!");
        return MEDIA_HAL_ERR;
    }
    s32Ret = DtcfCreateFilePath(fileType, dirType,
        paszFileNames[0], REC_FILE_NAME_LEN);
    if (s32Ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "DtcfCreateFilePath failed!");
        return MEDIA_HAL_ERR;
    }
    g_fileOptContext.dirType = dirType;
    return s32Ret;
}

bool DtcfMgrIsFile(const char *path, uint32_t len)
{
    MEDIA_HAL_UNUSED(len);
    char *lastSlash = strrchr(path, '/');
    if (lastSlash == NULL) {
        lastSlash = (char *)path;
    } else {
        if (lastSlash + 1 >= (path + len)) {
            return false;
        }
        lastSlash++;
    }
    char *dot = strchr(lastSlash, '.');
    if (dot != NULL && (dot - lastSlash) < strlen(lastSlash) - 1) {
        return true;
    }
    return false;
}

int32_t DtcfMgrMakeDir(const char *path, uint32_t len)
{
    return DtcfMakeFileDir(path, len);
}
