/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: dtcf
* Author: Media Software Group
* Create: 2025-09-20
*/

#include "dtcf.h"
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "securec.h"
#include "dtcf_qsort.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MW_VERSION
#define MW_VERSION "0.0.0"
#endif

// default directory name
#define WAV_DIR          "wav"
#define MP3_DIR          "mp3"
#define SILK_DIR         "silk"
#define OGG_DIR          "ogg"

#define DTCF_DIR_MODE    (0760)

#define FILE_SUFFIX_LEN_MAX       (8)

#define DTCF_TIME_STR_LEN         (20)
#define FILE_NAME_LEN_MIN         (23) // e.g. YYYY_MM_DD_HHMMSS_XX.TS

#define DTCF_SCAN_FILE_AMOUNT_MAX (10 * 1024) // 10240

#define CHECK_DTCF_NULL_PTR(ptr) \
    do { \
        if ((ptr) == NULL) \
        { \
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF NULL pointer\n"); \
            return DTCF_ERR_NULL_PTR; \
        } \
    } while (0)

typedef enum {
    DTCF_STATUS_CREATED = 0,
    DTCF_STATUS_SCANED,
    DTCF_STATUS_DESTROY,
    DTCF_STATUS_BUTT
} DTCF_STATUS_E;


typedef struct {
    char azTmpDir[FILE_PATH_LEN_MAX];
    uint32_t u32DirNameLen;
    DTCF_DIR_E enDir;
    DTCF_FILE_TYPE_E enType;
    char azTimeStr[DTCF_TIME_STR_LEN + 1];
} DTCF_TEMP_DIR_S;

typedef struct {
    struct dirent *pdirent;
    DTCF_DIR_E enDir;
} DTCF_DIRENT_S;

typedef struct {
    DTCF_DIRENT_S *pstDirentList;
    uint32_t u32DirentlistCount;
    uint32_t u32listCount;
    DTCF_DIR_E enScanDirs[DTCF_DIR_BUTT];
    uint32_t u32ScanDirCount;
} DTCF_SCAN_FILE_LIST_S;

static DTCF_SCAN_FILE_LIST_S g_dtcfScanFileList = {NULL, 0, 0, {0}, 0};

static char g_azRootDir[FILE_PATH_LEN_MAX] = {0};
static DTCF_STATUS_E g_dtcfStatu = DTCF_STATUS_BUTT;
static MediaHalMutexHandle g_dtcfInterfaceLock = NULL;

static struct tm g_lastTime = {};
static int32_t g_num = -1;

static char g_azDirNames[DTCF_DIR_BUTT][DIR_LEN_MAX] = {
    WAV_DIR,
    MP3_DIR,
    SILK_DIR,
    OGG_DIR
};

static char g_szFileTypeSuffix[DTCF_FILE_TYPE_BUTT][FILE_SUFFIX_LEN_MAX] = {
    "wav",
    "mp3",
    "silk",
    "ogg",
};

static char g_szFileNameSuffix[DTCF_DIR_BUTT][FILE_SUFFIX_LEN_MAX] = {
    "",
};

static int32_t CheckDtcfStatusCreate(void)
{
    if ((g_dtcfStatu != DTCF_STATUS_CREATED) && (g_dtcfStatu != DTCF_STATUS_SCANED)) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF,
            "status is not DTCF_STATUS_CREATED|DTCF_STATUS_SCANED, current status:%d\n", g_dtcfStatu);
        return DTCF_ERR_STATUS_ERROR;
    }
    return MEDIA_HAL_OK;
}

static int32_t CheckDtcfStatusScan(void)
{
    if (g_dtcfStatu != DTCF_STATUS_SCANED) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "status is not DTCF_STATUS_SCANED, current status:%d\n", g_dtcfStatu);
        return DTCF_ERR_STATUS_ERROR;
    }
    return MEDIA_HAL_OK;
}

static int32_t CheckDtcfPathIsDir(const char *path)
{
    struct stat sBuf = {0};
    if (stat(path, &sBuf) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, " stat sys errno:%d\n", errno);
        return DTCF_ERR_SYSTEM_ERROR;
    }
    if (!S_ISDIR(sBuf.st_mode)) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "%s is not a directory\n", path);
        return DTCF_PATH_IS_NDIR_ERROR;
    }
    return MEDIA_HAL_OK;
}

static void DtcfThreadMutexLock(void)
{
    MediaHalInitStaticMutexLock(&g_dtcfInterfaceLock);
    MediaHalMutexLock(g_dtcfInterfaceLock);
}

static void DtcfThreadMutexUnLock(void)
{
    if (g_dtcfInterfaceLock != NULL) {
        MediaHalMutexUnLock(g_dtcfInterfaceLock);
    }
}

static bool IsDigitChar(char c, const char min, const char max)
{
    if ((max < c) || (c < min)) {
        return false;
    }
    return true;
}

static bool IsSameChar(const char c, const char target)
{
    if (c != target) {
        return false;
    }
    return true;
}

static void DtcfFreeScanNameList(void)
{
    while (g_dtcfScanFileList.u32DirentlistCount > 0) {
        g_dtcfScanFileList.u32DirentlistCount--;
        if (g_dtcfScanFileList.pstDirentList[g_dtcfScanFileList.u32DirentlistCount].pdirent != NULL) {
            free(g_dtcfScanFileList.pstDirentList[g_dtcfScanFileList.u32DirentlistCount].pdirent);
            g_dtcfScanFileList.pstDirentList[g_dtcfScanFileList.u32DirentlistCount].pdirent = NULL;
        }
    }
    free(g_dtcfScanFileList.pstDirentList);
    g_dtcfScanFileList.pstDirentList = NULL;
    g_dtcfScanFileList.u32DirentlistCount = 0;
    g_dtcfScanFileList.u32ScanDirCount = 0;
    g_dtcfScanFileList.u32listCount = 0;
    int32_t ret = memset_s(&g_dtcfScanFileList.enScanDirs, sizeof(DTCF_DIR_E) * DTCF_DIR_BUTT,
        0, sizeof(DTCF_DIR_E) * DTCF_DIR_BUTT);
    if (ret != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "memset_s err: 0x%x\n", ret);
        return;
    }
}

static bool DtcfFileNameRuleCheckFileTypeSuffix(const char *pFileTypeSuffix, const uint32_t u32Len)
{
    uint32_t suffexLen, i;
    for (i = 0; i < DTCF_FILE_TYPE_BUTT; i++) {
        suffexLen = (uint32_t)strlen(g_szFileTypeSuffix[i]);
        if ((strncmp(g_szFileTypeSuffix[i], pFileTypeSuffix, suffexLen) == 0) && (u32Len == suffexLen)) {
            return true;
        }
    }
    return false;
}

static bool DtcfFileNameRuleCheckFileNameSuffix(const char *pFileNameSuffix, const uint32_t u32Len)
{
    if ((pFileNameSuffix[0] == 's') && (pFileNameSuffix[1] == '.')) {
        return DtcfFileNameRuleCheckFileTypeSuffix(&pFileNameSuffix[2], u32Len - 2); /* 2: file suffix begin */
    } else if (pFileNameSuffix[0] == 'b') {
        if (pFileNameSuffix[1] == '.') {
            return DtcfFileNameRuleCheckFileTypeSuffix(&pFileNameSuffix[2], u32Len - 2); /* 2: file suffix begin */
        } else if ((pFileNameSuffix[1] == '_') && /* 1: the first letter of the string before the file type */
            (pFileNameSuffix[2] == 's') && /* 2: the second letter of the string before the file type */
            (pFileNameSuffix[3] == '.')) { /* 3: the third letter of the string before the file type */
            return DtcfFileNameRuleCheckFileTypeSuffix(&pFileNameSuffix[4], u32Len - 4); /* 4: file suffix begin */
        }
    }
    return false;
}

/**
 * 1970_01_01_182605.MP4
 * 1970_01_01_182605_s.MP4
 * 1970_01_01_182605_b.MP4
 * 1970_01_01_182605_b_s.MP4
 */
static bool CheckFileNameOtherpart(const char *pazPath)
{
    /* To fix [Cmetrics] dolphin, each char for checking will make cyclomatic complexity greater than 20. */
    if ((IsSameChar(pazPath[10], '_')) &&        /* Check char array[10] of File name  */
        (IsDigitChar(pazPath[11], '0', '9')) &&  /* Check char array[11] of File name  */
        (IsDigitChar(pazPath[12], '0', '9')) &&  /* Check char array[12] of File name  */
        (IsDigitChar(pazPath[13], '0', '9')) &&  /* Check char array[13] of File name  */
        (IsDigitChar(pazPath[14], '0', '9')) &&  /* Check char array[14] of File name  */
        IsDigitChar(pazPath[15], '0', '9') &&  /* Check char array[15] of File name  */
        IsDigitChar(pazPath[16], '0', '9') &&  /* Check char array[16] of File name  */
        IsSameChar(pazPath[17], '_') &&        /* Check char array[17] of File name  */
        IsDigitChar(pazPath[18], '0', '9') &&  /* Check char array[18] of File name  */
        IsDigitChar(pazPath[19], '0', '9')) {  /* Check char array[19] of File name  */
            return true;
        }
        return false;
}

static bool IsFileNameRight(const char *pazPath)
{
    if (IsDigitChar(pazPath[0], '1', '9') &&   /* Check char array[0] of File name  */
        IsDigitChar(pazPath[1], '0', '9') &&   /* Check char array[1] of File name  */
        IsDigitChar(pazPath[2], '0', '9') &&   /* Check char array[2] of File name  */
        IsDigitChar(pazPath[3], '0', '9') &&   /* Check char array[3] of File name  */
        IsSameChar(pazPath[4], '_') &&         /* Check char array[4] of File name  */
        IsDigitChar(pazPath[5], '0', '9') &&   /* Check char array[5] of File name  */
        IsDigitChar(pazPath[6], '0', '9') &&   /* Check char array[6] of File name  */
        IsSameChar(pazPath[7], '_') &&         /* Check char array[7] of File name  */
        IsDigitChar(pazPath[8], '0', '9') &&   /* Check char array[8] of File name  */
        IsDigitChar(pazPath[9], '0', '9') &&   /* Check char array[9] of File name  */
        CheckFileNameOtherpart(pazPath)) {
        return true;
    }
    return false;
}

static bool DtcfFileNameRuleCheck(const char *pazPath, const uint32_t u32Len)
{
    if (u32Len < FILE_NAME_LEN_MIN) {
        return false;
    }
    // check string such as 1970_01_01_182605_00
    if (!IsFileNameRight(pazPath)) {
        return false;
    }
    switch (pazPath[20]) { /* Check char array[20] of File name  */
        case '.': /* 20: check string such as 1970_01_01_182605_00.MP4 */
            return DtcfFileNameRuleCheckFileTypeSuffix(&pazPath[21], u32Len - 21); /* 21: file suffix begin */
        case '_': /* 20: check string such as 1970_01_01_182605_00_*.MP4 */
            /* 21: file suffix with name rule tail */
            return DtcfFileNameRuleCheckFileNameSuffix(&pazPath[21], u32Len - 21);
        default:
            return false;
    }
}

static int32_t DtcfCheckRootDir(const char* pszRootDir)
{
    char aszPath[FILE_PATH_LEN_MAX] = {0};

    uint32_t u32Len = (uint32_t)strlen(pszRootDir);
    if ((u32Len >= FILE_PATH_LEN_MAX) || (u32Len == 0)) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "RootDir:%s too long", pszRootDir);
        return DTCF_UNSUPPORT_PATH;
    }

    int32_t ret = snprintf_s(aszPath, FILE_PATH_LEN_MAX,
        FILE_PATH_LEN_MAX - 1, "%s", pszRootDir);
    if (ret < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "RootDir:%s is invalid path", pszRootDir);
        return DTCF_UNSUPPORT_PATH;
    }

    for (uint32_t i = 0; i < u32Len; i++) {
        if ((i == 0) && (aszPath[i] != '/')) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "RootDir:%s is invalid path", pszRootDir);
            return DTCF_UNSUPPORT_PATH;
        }

        if (aszPath[i] == '/') {
            continue;
        }

        if (!(((aszPath[i] >= 'a') && (aszPath[i] <= 'z')) ||
            ((aszPath[i] >= 'A') && (aszPath[i] <= 'Z')) ||
            ((aszPath[i] >= '0') && (aszPath[i] <= '9')) ||
            (aszPath[i] == '_') || (aszPath[i] == '.'))) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "RootDir:%s is invalid path", pszRootDir);
            return DTCF_UNSUPPORT_PATH;
        }
    }

    return MEDIA_HAL_OK;
}

static int32_t Selector(const struct dirent *dir)
{
    CHECK_DTCF_NULL_PTR(dir);
    uint32_t len = (uint32_t)strlen(dir->d_name);
    if (DtcfFileNameRuleCheck(dir->d_name, len)) {
        return 1;
    }
    MEDIA_HAL_LOGI(MODULE_NAME_DTCF, "dir->d_name:%s\n", dir->d_name);
    return MEDIA_HAL_OK;
}

static int32_t SortBydnameDesc(const void *a, const void *b)
{
    DTCF_DIRENT_S *aa = (DTCF_DIRENT_S *)a;
    DTCF_DIRENT_S *bb = (DTCF_DIRENT_S *)b;
    return strcmp(bb->pdirent->d_name, aa->pdirent->d_name);
}

static int32_t DtcfMkdir(const char *dir, mode_t mode)
{
    int32_t ret = mkdir(dir, mode);
    if (ret < 0) {
        if (errno == EEXIST) {
            return CheckDtcfPathIsDir(dir);
        } else {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "mkdir %s fail ret:%d\n", dir, ret);
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "[%d] \n", errno);
            return DTCF_ERR_EINVAL_PARAMETER;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t DtcfMakedirs(const char *muldir, mode_t mode)
{
    uint32_t i;
    uint32_t len;
    char str[FILE_PATH_LEN_MAX + 1] = { 0 };
    int32_t ret = strncpy_s(str, FILE_PATH_LEN_MAX + 1, muldir, FILE_PATH_LEN_MAX);
    if (ret != EOK) {
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    len = (uint32_t)strlen(str);
    for (i = 0; i < len; i++) {
        if ((i == 0) && (str[i] == '/')) {
            continue;
        }
        if (str[i] == '/') {
            str[i] = '\0';
            ret = mkdir(str, mode);
            if ((ret < 0) && (errno == EEXIST) && (CheckDtcfPathIsDir(str) == MEDIA_HAL_OK)) {
                str[i] = '/';
                continue;
            } else if (ret < 0) {
                MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "mkdir str:%s fail ret:%d\n", str, ret);
                return ret;
            }
            str[i] = '/';
        }
    }
    if (len <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "str len not enough\n");
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    ret = mkdir(str, mode);
    MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "str = %s", str);
    if (ret < 0) {
        if (errno == EEXIST) {
            return CheckDtcfPathIsDir(str);
        }
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, " stat sys errno:%d\n", errno);
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "mkdir str:%s fail ret:%d len:%u\n", str, ret, len);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static void DtcfResetDirNames(void)
{
    const char azDirNames[DTCF_DIR_BUTT][DIR_LEN_MAX] = {
        WAV_DIR,
    };
    if (memcpy_s(g_azDirNames, sizeof(g_azDirNames), azDirNames,
        sizeof(char) * DIR_LEN_MAX * DTCF_DIR_BUTT) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "reset dtcf dir name failed\n");
        return;
    }
    g_azRootDir[0] = '\0';
}

static int32_t DtcfExpandFileList(uint32_t u32size)
{
    uint32_t u32ChangePoit = 10;
    if ((g_dtcfScanFileList.u32DirentlistCount + u32size) > DTCF_SCAN_FILE_AMOUNT_MAX) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "u32DirentlistCount:%d +  u32size:%u > DTCF_SCAN_FILE_AMOUNT_MAX:%d\n",
            g_dtcfScanFileList.u32DirentlistCount, u32size, DTCF_SCAN_FILE_AMOUNT_MAX);
        return DTCF_ERR_EINVAL_PARAMETER;
    }

    if ((g_dtcfScanFileList.u32DirentlistCount + u32size) >= g_dtcfScanFileList.u32listCount) {
        while (u32ChangePoit < DTCF_SCAN_FILE_AMOUNT_MAX) {
            if (u32ChangePoit < (g_dtcfScanFileList.u32DirentlistCount + u32size)) {
                u32ChangePoit = u32ChangePoit * 2; /* 2: add fileList size */
            } else {
                break;
            }
        }
        DTCF_DIRENT_S *newDirEnt = (DTCF_DIRENT_S *) malloc ((u32ChangePoit) * sizeof(DTCF_DIRENT_S));
        if (newDirEnt == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "malloc mem error, errno:%d\n", errno);
            return DTCF_ERR_SYSTEM_ERROR;
        }
        if (memset_s(newDirEnt, u32ChangePoit * sizeof(DTCF_DIRENT_S), 0x0,
            u32ChangePoit * sizeof(DTCF_DIRENT_S)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "memset_s newDirEnt error, errno:%d\n", errno);
            free(newDirEnt);
            newDirEnt = NULL;
            return DTCF_ERR_EINVAL_PARAMETER;
        }
        if (g_dtcfScanFileList.pstDirentList != NULL) {
            if (memcpy_s(newDirEnt, (u32ChangePoit) * sizeof(DTCF_DIRENT_S), g_dtcfScanFileList.pstDirentList,
                (g_dtcfScanFileList.u32DirentlistCount * sizeof(DTCF_DIRENT_S))) != EOK) {
                free(newDirEnt);
                newDirEnt = NULL;
                return DTCF_ERR_EINVAL_PARAMETER;
            }
            free(g_dtcfScanFileList.pstDirentList);
            g_dtcfScanFileList.pstDirentList = NULL;
        }

        g_dtcfScanFileList.pstDirentList = newDirEnt;
        g_dtcfScanFileList.u32listCount = u32ChangePoit;
    }
    return MEDIA_HAL_OK;
}

int32_t CheckToMkdirForInit(DTCF_DIR_E dirType)
{
    char azTmpDir[FILE_PATH_LEN_MAX] = { 0 };
    if (dirType >= DTCF_DIR_BUTT) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "not support dir[%d]", dirType);
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    int32_t ret = snprintf_s(azTmpDir, FILE_PATH_LEN_MAX, FILE_PATH_LEN_MAX - 1,
        "%s/%s", g_azRootDir, g_azDirNames[dirType]);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "snprintf_s stTmpDir.azTmpDir fail, ret:%d", ret);
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    MEDIA_HAL_LOGI(MODULE_NAME_DTCF, "azTmpDir:%s", azTmpDir);
    ret = DtcfMkdir(azTmpDir, DTCF_DIR_MODE);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "mkdir g_azDirNames[%d]:%s fail ret:%d", dirType, g_azDirNames[dirType], ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

int32_t DtcfInit(const char *pazRootDir)
{
    CHECK_DTCF_NULL_PTR(pazRootDir);
    DtcfThreadMutexLock();
    int32_t ret = DtcfCheckRootDir(pazRootDir);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF init failed:0x%x\n", ret);
        DtcfThreadMutexUnLock();
        return ret;
    }
    if (((g_dtcfStatu != DTCF_STATUS_BUTT) && (g_dtcfStatu != DTCF_STATUS_DESTROY)) ||
        (g_dtcfStatu == DTCF_STATUS_CREATED)) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "status error, current status:%d\n", g_dtcfStatu);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_STATUS_ERROR;
    }
    ret = DtcfMakedirs(pazRootDir, DTCF_DIR_MODE);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF_Mkdirs azRootDir:%s fail ret:%x\n", pazRootDir, ret);
        DtcfThreadMutexUnLock();
        return ret;
    }
    ret = snprintf_s(g_azRootDir, FILE_PATH_LEN_MAX, FILE_PATH_LEN_MAX - 1, "%s", pazRootDir);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "snprintf_s pazFilePath fail, ret:%d\n", ret);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    g_dtcfStatu = DTCF_STATUS_CREATED;
    DtcfThreadMutexUnLock();
    return MEDIA_HAL_OK;
}

int32_t DtcfDeInit(void)
{
    if (((g_dtcfStatu == DTCF_STATUS_BUTT) || (g_dtcfStatu == DTCF_STATUS_DESTROY)) &&
        (g_dtcfStatu != DTCF_STATUS_CREATED)) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "status error, current status:%d\n", g_dtcfStatu);
        return DTCF_ERR_STATUS_ERROR;
    }

    DtcfThreadMutexLock();
    if (g_dtcfScanFileList.pstDirentList != NULL) {
        DtcfFreeScanNameList();
    }
    DtcfResetDirNames();
    g_dtcfStatu = DTCF_STATUS_DESTROY;
    DtcfThreadMutexUnLock();

    return MEDIA_HAL_OK;
}

static int32_t DtcfScanDir(const DTCF_DIR_E enDirs[DTCF_DIR_BUTT], uint32_t u32DirCount)
{
    for (uint32_t i = 0; i < u32DirCount; i++) {
        struct dirent **dirNameList = NULL;
        if (enDirs[i] >= DTCF_DIR_BUTT) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF enDir:%d out of Range\n", enDirs[i]);
            return DTCF_ERR_EINVAL_PARAMETER;
        } else if (g_azDirNames[(int32_t)enDirs[i]][0] == '\0') {
            return DTCF_ERR_EINVAL_PARAMETER;
        }
        char dirBuf[FILE_PATH_LEN_MAX] = { 0 };
        int32_t ret = snprintf_s(dirBuf, FILE_PATH_LEN_MAX, FILE_PATH_LEN_MAX - 1, "%s/%s/",
            g_azRootDir, g_azDirNames[(int32_t)enDirs[i]]);
        if (ret <= 0) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "snprintf_s stTmpDir.dirBuf fail, ret:%d\n", ret);
            return DTCF_ERR_EINVAL_PARAMETER;
        }
        // in order to clean codeDEX warning
        const char *dirp = dirBuf;
        int32_t scanNamelistCount = scandir(dirp, &dirNameList, Selector, NULL);
        if (scanNamelistCount <= 0) {
            if (dirNameList != NULL) {
                free(dirNameList);
            }
            continue;
        }
        if (dirNameList == NULL) {
            g_dtcfScanFileList.enScanDirs[i] = enDirs[i];
            continue;
        }
        ret = DtcfExpandFileList(scanNamelistCount);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF_ExpandFileList fail, ret:%x\n", ret);
            int32_t node = 0;
            while (node < scanNamelistCount) {
                free(dirNameList[node]);
                node++;
            }
            free(dirNameList);
            return ret;
        }
        for (int32_t n = 0; n < scanNamelistCount; n++) {
            g_dtcfScanFileList.pstDirentList[g_dtcfScanFileList.u32DirentlistCount + (uint32_t)n].pdirent =
                dirNameList[n];
            g_dtcfScanFileList.pstDirentList[g_dtcfScanFileList.u32DirentlistCount + (uint32_t)n].enDir = enDirs[i];
        }
        g_dtcfScanFileList.u32DirentlistCount = g_dtcfScanFileList.u32DirentlistCount + (uint32_t)scanNamelistCount;
        free(dirNameList);
        g_dtcfScanFileList.enScanDirs[i] = enDirs[i];
    }
    return MEDIA_HAL_OK;
}

int32_t DtcfScanFiles(const DTCF_DIR_E enDirs[], uint32_t u32DirCount, uint32_t *pu32FileAmount)
{
    CHECK_DTCF_NULL_PTR(enDirs);
    int32_t ret = CheckDtcfStatusCreate();
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    DtcfThreadMutexLock();
    if ((u32DirCount == 0) || (u32DirCount > DTCF_DIR_BUTT)) {
        DtcfThreadMutexUnLock();
        return DTCF_ERR_EINVAL_PARAMETER;
    }

    if (g_dtcfScanFileList.pstDirentList != NULL) {
        DtcfFreeScanNameList();
    }

    ret = DtcfScanDir(enDirs, u32DirCount);
    if (ret != MEDIA_HAL_OK) {
        DtcfThreadMutexUnLock();
        return ret;
    }

    g_dtcfScanFileList.u32ScanDirCount = u32DirCount;
    *pu32FileAmount = g_dtcfScanFileList.u32DirentlistCount;
    if ((g_dtcfScanFileList.u32DirentlistCount > 0) && (g_dtcfScanFileList.pstDirentList != NULL)) {
        Qsort(g_dtcfScanFileList.pstDirentList, g_dtcfScanFileList.u32DirentlistCount,
            sizeof(DTCF_DIRENT_S), SortBydnameDesc);
    }
    g_dtcfStatu = DTCF_STATUS_SCANED;
    DtcfThreadMutexUnLock();
    return MEDIA_HAL_OK;
}

static int32_t FindFilePath(DTCF_DIR_E enDir, char *pazFilePath, uint32_t u32Length, DTCF_TEMP_DIR_S stTmpDir)
{
    int32_t index = -1;
    MEDIA_HAL_LOGI(MODULE_NAME_DTCF, "enDir:%d u32DirentlistCount:%d\n", enDir, g_dtcfScanFileList.u32DirentlistCount);
    for (int32_t i = (int32_t)g_dtcfScanFileList.u32DirentlistCount - 1; i >= 0; i--) {
        MEDIA_HAL_LOGI(MODULE_NAME_DTCF, "i:%d g_dtcfScanFileList.pstDirentList[i].enDir:%d \n",
            i, g_dtcfScanFileList.pstDirentList[i].enDir);
        if (enDir == g_dtcfScanFileList.pstDirentList[i].enDir) {
            index = i;
            break;
        }
    }
    if (u32Length == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "error: pazFilePath lenth is 0\n");
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    if ((index < 0) || ((uint32_t)index >= g_dtcfScanFileList.u32DirentlistCount)) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "can't find enDir:%d file index:%d\n", enDir, index);
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    int32_t ret = snprintf_s(pazFilePath, u32Length, u32Length - 1, "%s%s", stTmpDir.azTmpDir,
        g_dtcfScanFileList.pstDirentList[index].pdirent->d_name);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "GetOldestFilePath snprintf_s pazFilePath fail, ret:%d\n", ret);
        return DTCF_ERR_EINVAL_PARAMETER;
    } else if ((uint32_t)ret >= u32Length) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF,
            "GetOldestFilePath snprintf_s u32Length:%d is not length enough, ret:%d\n", u32Length, ret);
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    return MEDIA_HAL_OK;
}

int32_t DtcfGetOldestFilePath(DTCF_DIR_E enDir, char *pazFilePath, uint32_t u32Length)
{
    CHECK_DTCF_NULL_PTR(pazFilePath);
    if (enDir >= DTCF_DIR_BUTT) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF enDir:%d out of Range\n", enDir);
        return DTCF_ERR_EINVAL_PARAMETER;
    } else if (CheckDtcfStatusScan() != MEDIA_HAL_OK) {
        return DTCF_ERR_STATUS_ERROR;
    } else if (u32Length == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF file path length is 0\n");
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    DtcfThreadMutexLock();
    if (strlen(g_azDirNames[enDir]) == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "g_azDirNames[%d] undefined\n", enDir);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_UNDEFINE_DIR;
    }
    DTCF_TEMP_DIR_S stTmpDir = {{0}, 0, DTCF_DIR_BUTT, DTCF_FILE_TYPE_BUTT, {0}};
    int32_t ret = snprintf_s(stTmpDir.azTmpDir, FILE_PATH_LEN_MAX, FILE_PATH_LEN_MAX - 1, "%s/%s/",
        g_azRootDir, g_azDirNames[enDir]);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "GetOldestFilePath snprintf_s stTmpDir.azTmpDir fail, ret:%d\n", ret);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_EINVAL_PARAMETER;
    } else if (ret >= FILE_PATH_LEN_MAX) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF,
            "GetOldestFilePath snprintf_s u32Length:%d is not length enough, ret:%d\n", u32Length, ret);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    ret = FindFilePath(enDir, pazFilePath, u32Length, stTmpDir);
    if (ret != MEDIA_HAL_OK) {
        DtcfThreadMutexUnLock();
        return ret;
    }
    DtcfThreadMutexUnLock();
    return MEDIA_HAL_OK;
}

int32_t DtcfGetCurrentFileTime(struct tm *fileTime)
{
    CHECK_DTCF_NULL_PTR(fileTime);
    fileTime->tm_year = g_lastTime.tm_year;
    fileTime->tm_mon = g_lastTime.tm_mon;
    fileTime->tm_mday = g_lastTime.tm_mday;
    fileTime->tm_hour = g_lastTime.tm_hour;
    fileTime->tm_min = g_lastTime.tm_min;
    fileTime->tm_sec = g_lastTime.tm_sec;
    return MEDIA_HAL_OK;
}

static int32_t DtcfGetTime(char *pazTime, uint32_t u32BufSize)
{
    CHECK_DTCF_NULL_PTR(pazTime);
    struct tm t;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "gettimeofday error, errno:%d\n", errno);
        return DTCF_ERR_SYSTEM_ERROR;
    }

    if (tv.tv_usec > 500000) { /* 500000: Unit Conversion */
        tv.tv_sec++;
    }

    localtime_r((time_t *)&tv.tv_sec, &t);

    if (t.tm_year == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "localtime_r error, errno:%d\n", errno);
        return DTCF_ERR_SYSTEM_ERROR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME_DTCF, "%4d_%02d%02d_%02d%02d%02d  %lld\n",
        /* the correct time need to plus 1900 */
        t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, (int64_t)tv.tv_usec);
    if ((t.tm_year == g_lastTime.tm_year) &&
        (t.tm_mon == g_lastTime.tm_mon) &&
        (t.tm_mday == g_lastTime.tm_mday) &&
        (t.tm_hour == g_lastTime.tm_hour) &&
        (t.tm_min == g_lastTime.tm_min) &&
        (t.tm_sec == g_lastTime.tm_sec)) {
        g_num++;
    } else {
        g_num = 0;
    }

    // if this API was called 100 times or more IN THE SAME SECOND, will cause string length over DTCF_TIME_STR_LEN
    if (g_num >= 100) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "num=%d, file name over lenght error!\n", g_num);
        return DTCF_ERR_EINVAL_PARAMETER;
    }

    int32_t ret = memcpy_s(&g_lastTime, sizeof(g_lastTime), &t, sizeof(struct tm));
    if (ret != EOK) {
        return DTCF_ERR_SYSTEM_ERROR;
    }
    ret = snprintf_s(pazTime, u32BufSize, DTCF_TIME_STR_LEN, "%4d_%02d_%02d_%02d%02d%02d_%02d",
        t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, g_num); /* 1900: count begin */
    if (ret < 0 || ret >= DTCF_TIME_STR_LEN) {
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    return MEDIA_HAL_OK;
}

int32_t DtcfCreateFilePath(DTCF_FILE_TYPE_E enFileType, DTCF_DIR_E enDir,
    char *pazFilePath, uint32_t u32Length)
{
    CHECK_DTCF_NULL_PTR(pazFilePath);
    if (enDir >= DTCF_DIR_BUTT) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF enDir:%d out of Range\n", enDir);
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    if (enFileType >= DTCF_FILE_TYPE_BUTT) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF fileType out of Range\n");
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    int32_t ret = CheckDtcfStatusCreate();
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    if (u32Length == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF file path length is 0\n");
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    DtcfThreadMutexLock();
    if (strlen(g_azDirNames[enDir]) == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "g_azDirNames[%d] undefined\n", enDir);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_UNDEFINE_DIR;
    }

    char azTime[DTCF_TIME_STR_LEN + 1] = {0};
    ret = DtcfGetTime(azTime, DTCF_TIME_STR_LEN + 1);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "SS_DTCF_GetTime fail, ret:0x%x\n", ret);
        DtcfThreadMutexUnLock();
        return ret;
    }
    ret = snprintf_s(pazFilePath, u32Length, u32Length - 1, "%s/%s/%s%s.%s", g_azRootDir,
        g_azDirNames[enDir], azTime, g_szFileNameSuffix[enDir], g_szFileTypeSuffix[enFileType]);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "CreateFilePath snprintf_s pazFilePath fail, ret:%d\n", ret);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_EINVAL_PARAMETER;
    } else if ((uint32_t)ret >= u32Length) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF,
            "CreateFilePath snprintf_s u32Length:%d is not length enough, ret:%d\n", u32Length, ret);
        DtcfThreadMutexUnLock();
        return DTCF_ERR_EINVAL_PARAMETER;
    }
    DtcfThreadMutexUnLock();
    return MEDIA_HAL_OK;
}

int32_t DtcfMakeFileDir(char *pazFilePath, uint32_t u32Length)
{
    char dir[FILE_PATH_LEN_MAX] = {};
    char *lastSlash = strrchr(pazFilePath, '/');
    if (lastSlash != NULL) {
        int32_t dirLen = lastSlash - pazFilePath;
        if (dirLen >= FILE_PATH_LEN_MAX - 1) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "The file path is too long to create!");
            return -1;
        }
        if (strncpy_s(dir, FILE_PATH_LEN_MAX, pazFilePath, dirLen) != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "copy path error");
            return -1;
        }
        dir[dirLen] = '\0';
    }
    int32_t ret = DtcfMakedirs(dir, DTCF_DIR_MODE);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME_DTCF, "DTCF_Mkdirs azRootDir:%s fail ret:%x\n", dir, ret);
        return ret;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
