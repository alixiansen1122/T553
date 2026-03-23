/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: dtcf
* Author: Media Software Group
* Create: 2025-09-20
*/

#ifndef DTCF_H_
#define DTCF_H_
#include <stdint.h>
#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \addtogroup     DTCF */
/* [DTCF] */

#define MODULE_NAME_DTCF  "DTCF"

#define FILE_PATH_LEN_MAX         (256)
#define DIR_LEN_MAX               (64)

/* DataTime Error number base */
#define DTCF_ERRNO_BASE 0x2A00
/* Parameter is invalid */
#define DTCF_ERR_EINVAL_PARAMETER    (DTCF_ERRNO_BASE + 1)
/* Null pointer */
#define DTCF_ERR_NULL_PTR            (DTCF_ERRNO_BASE + 2)
/* failure caused by malloc memory */
#define DTCF_ERR_NOMEM               (DTCF_ERRNO_BASE + 3)
/* not support file path */
#define DTCF_UNSUPPORT_PATH          (DTCF_ERRNO_BASE + 4)
/* the same directory */
#define DTCF_SAME_DIR_PATH           (DTCF_ERRNO_BASE + 5)
/* Status error, operation not allowed */
#define DTCF_ERR_STATUS_ERROR        (DTCF_ERRNO_BASE + 6)
/* Empty directory */
#define DTCF_ERR_EMPTEY_DIR          (DTCF_ERRNO_BASE + 7)
/* directory not define */
#define DTCF_ERR_UNDEFINE_DIR        (DTCF_ERRNO_BASE + 8)
/* System error */
#define DTCF_ERR_SYSTEM_ERROR        (DTCF_ERRNO_BASE + 9)
/* the same filename */
#define DTCF_SAME_FILENAME_PATH      (DTCF_ERRNO_BASE + 10)
/* the path is not a directory */
#define DTCF_PATH_IS_NDIR_ERROR   (DTCF_ERRNO_BASE + 11)

/**
 * File Directory Type
 */
typedef enum {
    DTCF_DIR_RECORD_WAV_FILE,
    DTCF_DIR_RECORD_MP3_FILE,
    DTCF_DIR_RECORD_SILK_FILE,
    DTCF_DIR_RECORD_OGG_FILE,
    DTCF_DIR_BUTT,
} DTCF_DIR_E;

/**
 * File Type.
 */
typedef enum {
    DTCF_FILE_TYPE_WAV,
    DTCF_FILE_TYPE_MP3,
    DTCF_FILE_TYPE_SILK,
    DTCF_FILE_TYPE_OGG,
    DTCF_FILE_TYPE_BUTT
} DTCF_FILE_TYPE_E;

/**
  *    @brief init DTCF
  *
  *    init DTCF
  *    @param[in]  pazRootDir:  root of DTCF
  *    @return     0 success, non-zero error code
  */
int32_t DtcfInit(const char *pazRootDir);

/**
  *    @brief DeInit DTCF
  *
  *    DeInit DTCF
  *    @return     0 success, non-zero error code
  */
int32_t DtcfDeInit(void);

/**
  *    @brief scan files
  *
  *    scan files in some directories.
  *    @param[in]    enDirs:  directories to scan.
  *    @param[in]    u32DirCount: count of directory.
  *    @param[out]   pu32FileAmount:files amount.
  *    @return       0 success, non-zero error code
  */
int32_t DtcfScanFiles(const DTCF_DIR_E enDirs[], uint32_t u32DirCount, uint32_t *pu32FileAmount);

/**
  *    @brief create file full path
  *    @param[in]    enFileType:  type of file.
  *    @param[in]    enDir: type of directory.
  *    @param[out]   pazFilePath: file full path.
  *    @param[in]   u32Length: file full path length.
  *    @return       0 success, non-zero error code
  */
int32_t DtcfCreateFilePath(DTCF_FILE_TYPE_E enFileType, DTCF_DIR_E enDir,
    char *pazFilePath, uint32_t u32Length);

/**
  *    @brief get related file full path.
  *    @param[in]    pazSrcFilePath:  source file full path.
  *    @param[in]    enDir: related file directory.
  *    @param[out]   pazDstFilePath : related file full path.
  *    @param[in]   u32Length: file full path length.
  *    @return       0 success, non-zero error code
  */
int32_t DtcfGetRelatedFilePath(const char *pazSrcFilePath, DTCF_DIR_E enDir,
    char *pazDstFilePath, uint32_t u32Length);

/**
  *    @brief get current file time.
  *    @param[in]    fileTime:  struct to get file time.
  *    @return       0 success, non-zero error code
  */
int32_t DtcfGetCurrentFileTime(struct tm *fileTime);

int32_t DtcfGetOldestFilePath(DTCF_DIR_E enDir, char *pazFilePath, uint32_t u32Length);

int32_t DtcfMakeFileDir(char *pazFilePath, uint32_t u32Length);

int32_t CheckToMkdirForInit(DTCF_DIR_E dirType);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DTCF_H_ */
