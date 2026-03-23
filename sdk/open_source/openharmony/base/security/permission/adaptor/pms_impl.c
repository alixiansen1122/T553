/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pms.h"

#include <stdio.h>
#include "mem_util.h"
#include "securec.h"

#include "cJSON.h"

#include "error_define.h"
#include "perm_operate.h"

#include <fcntl.h>
#include <sys/stat.h>

#include "global.h"

#define P_DIR APP_BASE_DIR"user/ace/etc/permissions/"
#define P_NAME_MAXLEN 32
#define BUFF_SIZE 2048
#define FIELD_PERMISSION "permissions"
#define FIELD_NAME "name"
#define FIELD_DESC "desc"
#define FIELD_IS_GRANTED "isGranted"

// Supported permissions
static const PermissionDef g_permissions[] = {
    {"ohos.permission.LOCATION",                   SYSTEM_GRANT},
    {"ohos.permission.ACTIVITY_MOTION",            SYSTEM_GRANT},
    {"ohos.permission.READ_HEALTH_DATA",           SYSTEM_GRANT},
    {"ohos.permission.ACCESS_DISTRIBUTED_ABILITY", SYSTEM_GRANT},
    {"ohos.permission.MODIFY_AUDIO_SETTINGS",      SYSTEM_GRANT},
    {"ohos.permission.VIBRATE",                    SYSTEM_GRANT},
    {"ohos.permission.ACCELEROMETER",              SYSTEM_GRANT},
    {"ohos.permission.GYROSCOPE",                  SYSTEM_GRANT},
};

static unsigned int g_permissionSize = sizeof(g_permissions) / sizeof(PermissionDef);

// Permission matrix of run-time tasks
static struct TaskList g_taskList = {
    NULL
};

// don't forget free() afterwards
static char *ConcatString(const char *s1, const char *s2)
{
    unsigned int allocSize = strlen(s1) + strlen(s2) + 1;
    if (allocSize > BUFF_SIZE) {
        return NULL;
    }
    char *rst = (char *) PSRAM_Malloc(allocSize);
    if (rst == NULL) {
        return NULL;
    }
    if (memset_s(rst, allocSize, 0x0, allocSize) != EOK) {
        PSRAM_Free(rst);
        return NULL;
    }
    if (strcpy_s(rst, allocSize, s1) != EOK) {
        PSRAM_Free(rst);
        return NULL;
    }
    if (strcat_s(rst, allocSize, s2) != EOK) {
        PSRAM_Free(rst);
        return NULL;
    }
    return rst;
}

static int WriteString(const char *path, const char *string)
{
    int fd = open(path, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
    if (fd < 0) {
        return fd;
    }
    if (write(fd, string, strlen(string)) != strlen(string)) {
        close(fd);
        return PERM_ERRORCODE_WRITEFD_FAIL;
    }
    close(fd);
    return PERM_ERRORCODE_SUCCESS;
}

// don't forget free() afterwards
static char *ReadString(const char *path, int *errcode)
{
    struct stat buf;
    if (stat(path, &buf)) {
        *errcode = PERM_ERRORCODE_STAT_FAIL;
        return NULL;
    }
    int readSize = buf.st_size;
    char *rst = (char *) PSRAM_Malloc(buf.st_size);
    if (rst == NULL) {
        *errcode = PERM_ERRORCODE_MALLOC_FAIL;
        return NULL;
    }
    errno_t err = memset_s(rst, readSize, 0x0, readSize);
    if (err != EOK) {
        PSRAM_Free(rst);
        *errcode = PERM_ERRORCODE_MEMSET_FAIL;
        return NULL;
    }

    int fd = open(path, O_RDONLY, S_IREAD | S_IWRITE);
    if (fd < 0) {
        PSRAM_Free(rst);
        *errcode = PERM_ERRORCODE_OPENFD_FAIL;
        return NULL;
    }

    if (read(fd, rst, readSize) < 0) {
        PSRAM_Free(rst);
        close(fd);
        *errcode = PERM_ERRORCODE_READFD_FAIL;
        return NULL;
    }

    close(fd);
    return rst;
}

static char *GetPath(const char *identifier)
{
    return ConcatString(P_DIR, identifier);
}

static int GetPermissionType(const char *permission)
{
    for (int i = 0; i < g_permissionSize; i++) {
        if (strcmp(permission, g_permissions[i].name) == 0) {
            return g_permissions[i].type;
        }
    }
    return PERM_ERRORCODE_INVALID_PERMNAME;
}

static int ParsePermissions(const char *jsonStr, PermissionSaved **perms, int *permNum)
{
    cJSON *root = NULL;
    cJSON *array = NULL;
    root = cJSON_Parse(jsonStr);
    if (root == NULL) {
        return PERM_ERRORCODE_JSONPARSE_FAIL;
    }
    array = cJSON_GetObjectItem(root, FIELD_PERMISSION);
    int pSize = cJSON_GetArraySize(array);
    int allocSize = sizeof(PermissionSaved) * pSize;
    if (allocSize == 0 || pSize > g_permissionSize) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_PERM_NUM_ERROR;
    }
    *perms = (PermissionSaved *) PSRAM_Malloc(allocSize);
    if (*perms == NULL) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < pSize; i++) {
        cJSON *object = NULL;
        cJSON *itemName = NULL;
        cJSON *itemDesc = NULL;
        cJSON *itemGranted = NULL;
        object = cJSON_GetArrayItem(array, i);
        itemName = cJSON_GetObjectItem(object, FIELD_NAME);
        itemDesc = cJSON_GetObjectItem(object, FIELD_DESC);
        itemGranted = cJSON_GetObjectItem(object, FIELD_IS_GRANTED);
        printf("pms_impl::ParsePermissions pmsName: %s\n", itemName->valuestring);
        printf("pms_impl::ParsePermissions pmsDesc: %s\n", itemDesc->valuestring);
        printf("pms_impl::ParsePermissions granted: %d\n", itemGranted->valueint);
        if (strcpy_s((*perms + i)->name, PERM_NAME_LEN, itemName->valuestring) != EOK) {
            cJSON_Delete(root);
            PSRAM_Free(*perms);
            *perms = NULL;
            return PERM_ERRORCODE_COPY_ERROR;
        }
        if (strcpy_s((*perms + i)->desc, PERM_DESC_LEN, itemDesc->valuestring) != EOK) {
            cJSON_Delete(root);
            PSRAM_Free(*perms);
            *perms = NULL;
            return PERM_ERRORCODE_COPY_ERROR;
        }
        (*perms + i)->granted = (enum IsGranted) itemGranted->valueint;
    }
    *permNum = pSize;
    cJSON_Delete(root);
    return PERM_ERRORCODE_SUCCESS;
}

static int WritePermissions(cJSON *root, const char *path)
{
    char *jsonStr = NULL;
    jsonStr = cJSON_PrintUnformatted(root);
    if (jsonStr == NULL) {
        PSRAM_Free(path);
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    int ret = WriteString(path, jsonStr);
    cJSON_free(jsonStr);
    PSRAM_Free(path);
    cJSON_Delete(root);
    return ret;
}

static int SavePermissions(const char *identifier, const PermissionSaved *permissions, int permNum)
{
    if (identifier == NULL || permissions == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    cJSON *root = NULL;
    cJSON *array = NULL;
    char *path = NULL;
    root = cJSON_CreateObject();
    if (root == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    array = cJSON_CreateArray();
    if (array == NULL) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    path = GetPath(identifier);
    if (path == NULL) {
        cJSON_Delete(array);
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < permNum; i++) {
        cJSON *object = cJSON_CreateObject();
        if (object == NULL) {
            PSRAM_Free(path);
            cJSON_Delete(array);
            cJSON_Delete(root);
            return PERM_ERRORCODE_MALLOC_FAIL;
        }
        cJSON_AddItemToObject(object, FIELD_NAME, cJSON_CreateString(permissions[i].name));
        cJSON_AddItemToObject(object, FIELD_DESC, cJSON_CreateString(permissions[i].desc));
        cJSON_AddItemToObject(object, FIELD_IS_GRANTED, cJSON_CreateBool(permissions[i].granted));
        cJSON_AddItemToArray(array, object);
    }
    cJSON_AddItemToObject(root, FIELD_PERMISSION, array);
    return WritePermissions(root, path);
}

int QueryPermission(const char *identifier, PermissionSaved **permissions, int *permNum)
{
    if (identifier == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    int errCode = 0;
    char *path = NULL;
    char *jsonStr = NULL;

    path = GetPath(identifier);
    if (path == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    int ret = open(path, O_RDONLY, S_IREAD | S_IWRITE);
    if (ret < 0) {
        PSRAM_Free(path);
        return PERM_ERRORCODE_FILE_NOT_EXIST;
    }
    close(ret);

    jsonStr = ReadString(path, &errCode);
    PSRAM_Free(path);
    if ((errCode != 0) || (jsonStr == NULL)) {
        return errCode;
    }

    ret = ParsePermissions(jsonStr, permissions, permNum);
    PSRAM_Free(jsonStr);
    return ret;
}

int UpdateAppPermission(const char *identifier, PermissionTrans newPerms[], int newPermNum)
{
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret == PERM_ERRORCODE_FILE_NOT_EXIST) {
        return SaveOrUpdatePermissions(identifier, newPerms, newPermNum, FIRST_INSTALL);
    }
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    int allocSize = sizeof(PermissionSaved) * newPermNum;
    PermissionSaved *updatePerms = (PermissionSaved *) PSRAM_Malloc(allocSize);
    if (updatePerms == NULL) {
        PSRAM_Free(permissions);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < newPermNum; i++) {
        if (strlen(newPerms[i].name) > PERM_NAME_LEN - 1 || strlen(newPerms[i].desc) > PERM_DESC_LEN - 1) {
            PSRAM_Free(updatePerms);
            PSRAM_Free(permissions);
            return PERM_ERRORCODE_FIELD_TOO_LONG;
        }
        if (strcpy_s((updatePerms + i)->name, PERM_NAME_LEN, newPerms[i].name) != EOK) {
            PSRAM_Free(updatePerms);
            PSRAM_Free(permissions);
            return PERM_ERRORCODE_COPY_ERROR;
        }
        if (strcpy_s((updatePerms + i)->desc, PERM_DESC_LEN, newPerms[i].desc) != EOK) {
            PSRAM_Free(updatePerms);
            PSRAM_Free(permissions);
            return PERM_ERRORCODE_COPY_ERROR;
        }
        int permType = GetPermissionType(newPerms[i].name);
        if (permType == PERM_ERRORCODE_INVALID_PERMNAME) {
            PSRAM_Free(updatePerms);
            PSRAM_Free(permissions);
            return PERM_ERRORCODE_INVALID_PERMNAME;
        }
        (updatePerms + i)->granted = (permType == SYSTEM_GRANT) ? GRANTED : NOT_GRANTED;
        for (int j = 0; j < permNum; j++) {
            if (strcmp(newPerms[i].name, (permissions + j)->name) == 0) {
                (updatePerms + i)->granted = permissions[j].granted;
            }
        }
    }
    int retCode = SavePermissions(identifier, updatePerms, newPermNum);
    PSRAM_Free(updatePerms);
    PSRAM_Free(permissions);
    return retCode;
}

static int FormPermissions(
    const PermissionTrans permissions[], int permNum, const char *path, cJSON *root, cJSON *array)
{
    for (int i = 0; i < permNum; i++) {
        if (strlen(permissions[i].name) > PERM_NAME_LEN - 1 || strlen(permissions[i].desc) > PERM_DESC_LEN - 1) {
            PSRAM_Free(path);
            cJSON_Delete(array);
            cJSON_Delete(root);
            return PERM_ERRORCODE_FIELD_TOO_LONG;
        }
        cJSON *object = cJSON_CreateObject();
        if (object == NULL) {
            PSRAM_Free(path);
            cJSON_Delete(array);
            cJSON_Delete(root);
            return PERM_ERRORCODE_MALLOC_FAIL;
        }
        cJSON_AddItemToObject(object, FIELD_NAME, cJSON_CreateString(permissions[i].name));
        cJSON_AddItemToObject(object, FIELD_DESC, cJSON_CreateString(permissions[i].desc));
        int permType = GetPermissionType(permissions[i].name);
        if (permType == PERM_ERRORCODE_INVALID_PERMNAME) {
            PSRAM_Free(path);
            cJSON_Delete(object);
            cJSON_Delete(array);
            cJSON_Delete(root);
            return PERM_ERRORCODE_INVALID_PERMNAME;
        }
        cJSON_AddItemToObject(object, FIELD_IS_GRANTED, cJSON_CreateBool(permType));
        cJSON_AddItemToArray(array, object);
    }
    cJSON_AddItemToObject(root, FIELD_PERMISSION, array);
    return WritePermissions(root, path);
}

int SaveOrUpdatePermissions(const char *identifier, PermissionTrans permissions[], int permNum, enum IsUpdate isUpdate)
{
    if (identifier == NULL || permissions == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    if (permNum > g_permissionSize) {
        return PERM_ERRORCODE_TOO_MUCH_PERM;
    }
    if (isUpdate == UPDATE) {
        return UpdateAppPermission(identifier, permissions, permNum);
    }
    cJSON *root = NULL;
    cJSON *array = NULL;
    char *path = NULL;
    root = cJSON_CreateObject();
    if (root == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    array = cJSON_CreateArray();
    if (array == NULL) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    path = GetPath(identifier);
    if (path == NULL) {
        cJSON_Delete(array);
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    return FormPermissions(permissions, permNum, path, root, array);
}

int DeletePermissions(const char *identifier)
{
    char *path = NULL;
    path = GetPath(identifier);
    if (path == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }

    int ret = open(path, O_RDONLY, S_IREAD | S_IWRITE);
    if (ret < 0) {
        PSRAM_Free(path);
        return PERM_ERRORCODE_SUCCESS;
    }
    close(ret);

    ret = unlink(path);
    if (ret != 0) {
        PSRAM_Free(path);
        return PERM_ERRORCODE_UNLINK_ERROR;
    }

    PSRAM_Free(path);
    return PERM_ERRORCODE_SUCCESS;
}

int IsPermissionValid(const char *permissionName)
{
    for (int i = 0; i < g_permissionSize; i++) {
        if (strcmp(permissionName, g_permissions[i].name) == 0) {
            return PERM_ERRORCODE_SUCCESS;
        }
    }
    return PERM_ERRORCODE_INVALID_PERMNAME;
}

int LoadPermissions(const char *identifier, int taskID)
{
    if (GetTask(&g_taskList, taskID) != NULL) {
        return PERM_ERRORCODE_SUCCESS;
    }
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }

    TNode *node = (TNode *) PSRAM_Malloc(sizeof(TNode));
    if (node == NULL) {
        PSRAM_Free(permissions);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    node->taskID = taskID;
    if (strcpy_s(node->pkgName, PKG_NAME_LEN, identifier) != EOK) {
        PSRAM_Free(permissions);
        PSRAM_Free(node);
        return PERM_ERRORCODE_COPY_ERROR;
    }
    node->permList = permissions;
    node->permNum = permNum;
    node->next = NULL;

    AddTask(&g_taskList, node);
    return PERM_ERRORCODE_SUCCESS;
}

int UnLoadPermissions(int taskID)
{
    DeleteTask(&g_taskList, taskID);
    return PERM_ERRORCODE_SUCCESS;
}

int CheckPermission(int taskID, const char *permissionName)
{
    if (taskID < 0 || permissionName == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    int ret = PermissionExists(&g_taskList, taskID, permissionName);
    return ret;
}

int GrantPermission(const char *identifier, const char *permName)
{
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    for (int i = 0; i < permNum; i++) {
        if (strcmp((permissions + i)->name, permName) == 0) {
            (permissions + i)->granted = GRANTED;
            break;
        }
    }
    int retCode = SavePermissions(identifier, permissions, permNum);
    PSRAM_Free(permissions);
    return retCode;
}

int RevokePermission(const char *identifier, const char *permName)
{
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    for (int i = 0; i < permNum; i++) {
        if (strcmp((permissions + i)->name, permName) == 0) {
            (permissions + i)->granted = NOT_GRANTED;
            break;
        }
    }
    int retCode = SavePermissions(identifier, permissions, permNum);
    PSRAM_Free(permissions);
    return retCode;
}

int GrantRuntimePermission(const char *identifier, const char *permName[], int permNum)
{
    PermissionSaved *permissions = NULL;
    int realPermNum = 0;
    int ret = QueryPermission(identifier, &permissions, &realPermNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    for (int i = 0; i < realPermNum; i++) {
        for (int j = 0; j < permNum; j++) {
            if (strcmp((permissions + i)->name, permName[j]) == 0) {
                (permissions + i)->granted = GRANTED;
                break;
            }
        }
    }
    int retCode = SavePermissions(identifier, permissions, realPermNum);
    PSRAM_Free(permissions);
    return retCode;
}
