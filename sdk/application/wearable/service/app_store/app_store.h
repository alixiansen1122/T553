/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: app store api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef APP_STORE_H
#define APP_STORE_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PKG_NAME_MAX_LENGTH 128
#define VERSION_NAME_MAX_LENGTH 128
#define LABEL_NAME_MAX_LENGTH 256
#define FS_DIR_APP_STORE    "/user/app_store/"

typedef enum {
    PKG_OPERATION_TYPE_INSTALL        = 0x00,
    PKG_OPERATION_TYPE_UNINSTALL      = 0x01,
} PackageOperationType;

typedef enum {
    APP_STORE_OPERATION  = 0x01,
    APP_STORE_NOTIFY_PROGRESS = 0x02,
    APP_STORE_GET_PKG_LIST = 0x03,
    APP_STORE_GET_BUNDLEINFO = 0x04,
} AppStoreType;

typedef enum {
    INSTALL_CALLBACK = 0,
    UNINSTALL_CALLBACK = 1,
    INSTALLING_UNINSTALLING = 200,
} CallbackStatus;

typedef enum {
    INSTALL_DOING = 101,
    INSTALL_OK = 102,
    INSTALL_FAILED = 103,
    UNINSTALL_DOING = 104,
    UNINSTALL_OK = 105,
    UNINSTALL_FAILED = 106,
    INSTALL_OVER_LIMIT = 107,
} OperationResult;

typedef struct {
    uint8_t operationType;
    uint8_t pkgName[PKG_NAME_MAX_LENGTH];
} PkgOperationInfo;

typedef struct {
    uint8_t operationResult;
    uint8_t pkgName[PKG_NAME_MAX_LENGTH];
} PkgResultReport;

typedef struct {
    uint8_t pkgName[PKG_NAME_MAX_LENGTH];
    int32_t versionCode;
    uint8_t versionName[VERSION_NAME_MAX_LENGTH];
    uint8_t labelName[LABEL_NAME_MAX_LENGTH];
} PkgUnitInfo;

errcode_t AppStorePkgOperation(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t AppStoreGetPkgList(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t AppStoreGetBundleInfo(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* APP_STORE_H */
