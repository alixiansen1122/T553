/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay vendor file adapt.
 * Author:
 * Create:
 */

#include "alipay_common.h"
#include "vendor_file.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "securec.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "dirent.h"

#define ALIPAY_BASE_DIR  "/user/alipay/"
#define ALIPAY_MAX_FILE_PATH_LEN 256

EXTERNC void* alipay_open_rsvd_part(PARAM_IN char filename[128])
{
    if (access(ALIPAY_BASE_DIR, F_OK) != 0) {
        if (mkdir(ALIPAY_BASE_DIR, S_IREAD | S_IWRITE) != 0) {
            return NULL;
        }
    }
    char file_path[ALIPAY_MAX_FILE_PATH_LEN] = { 0 };
    int size = sprintf_s(file_path, sizeof(file_path), "%s%s", ALIPAY_BASE_DIR, filename);
    if (size < 0) {
        return NULL;
    }

    int fd = open(file_path, O_CREAT | O_RDWR, 0777);
    if (fd < 0) {
        return NULL;
    } else {
        return (void*)fd;
    }
}

EXTERNC int alipay_write_rsvd_part(PARAM_IN void* fd, PARAM_IN void *data,PARAM_IN uint32_t data_len)
{
    int ret = write((int)fd, data, data_len);
    if (ret < 0) {
        return -1;
    } else {
        return 0;
    }
}

EXTERNC int alipay_read_rsvd_part(PARAM_IN void* fd,PARAM_OUT void *buffer,PARAM_INOUT uint32_t* read_len)
{
    size_t len = read((int)fd, buffer, (size_t)(*read_len));
    if (len > 0) {
        *read_len = (uint32_t)len;
        return 0;
    } else {
        return -1;
    }
}

EXTERNC int alipay_close_rsvd_part(PARAM_IN void* fd)
{
    int ret = close((int)fd);
    if (ret < 0) {
        return -1;
    } else {
        return 0;
    }
}

EXTERNC int alipay_access_rsvd_part(PARAM_IN char filename[128])
{
    char file_path[ALIPAY_MAX_FILE_PATH_LEN] = { 0 };
    int size = sprintf_s(file_path, sizeof(file_path), "%s%s", ALIPAY_BASE_DIR, filename);
    if (size < 0) {
        return 0;
    }
    int ret = access(file_path, F_OK);
    if (ret < 0) {
        return 0; // 文件不存在
    } else {
        return 1; // 文件存在
    }
}

EXTERNC int alipay_remove_rsvd_part(PARAM_IN char filename[128])
{
    char file_path[ALIPAY_MAX_FILE_PATH_LEN] = { 0 };
    int size = sprintf_s(file_path, sizeof(file_path), "%s%s", ALIPAY_BASE_DIR, filename);
    if (size < 0) {
        return -1;
    }
    int ret = unlink(file_path);
    if (ret < 0) {
        return -1;
    } else {
        return 0;
    }
}

EXTERNC int alipay_clear_rsvd_part(void)
{
    struct dirent *direntp;
    // 打开目录
    DIR *dirp = opendir(ALIPAY_BASE_DIR);
    // 遍历文件
    if (dirp != NULL) {
        while ((direntp = readdir(dirp)) != NULL) {
            int ret = alipay_remove_rsvd_part(direntp->d_name);
            if (ret < 0) {
                closedir(dirp);
                return -1;
            }
        }
    }
    // 关闭目录
    closedir(dirp);
    return 0;
}