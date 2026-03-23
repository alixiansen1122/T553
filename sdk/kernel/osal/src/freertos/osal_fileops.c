/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 */

#include <FreeRTOS.h>
#include <fcntl.h>
#include <fs/fs.h>
#include <fs/file.h>
#include <unistd.h>
#include <portable.h>

#include "soc_osal.h"
#include "osal_inner.h"

void *osal_klib_fopen(const char *filename, int flags, int mode)
{
    int *filp = NULL;
    int fd = open(filename, flags, mode);
    if (fd < 0) {
        osal_log("open failed! fd = %d, errno = %d\n", fd, get_errno());
        return NULL;
    } else {
        filp = (int *)pvPortMalloc(sizeof(int));
        if (filp == NULL) {
            osal_log("pvPortMalloc failed!\n");
            close(fd);
            return NULL;
        }
        *filp = fd;
    }
    return (void *)filp;
}

void osal_klib_fclose(void *filp)
{
    if (filp == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    close(*(int *)filp);
    vPortFree(filp);
}

int osal_klib_fwrite(const char *buf, unsigned long size, void *filp)
{
    if (filp == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    return write(*(int *)filp, buf, size);
}

int osal_klib_fread(char *buf, unsigned long size, void *filp)
{
    if (filp == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    return read(*(int *)filp, buf, size);
}

int osal_klib_fseek(long long offset, int whence, void *filp)
{
    int ret;
    off_t res;
    if (filp == NULL || offset > INT32_MAX) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    res = lseek(*(int *)filp, (off_t)offset, whence);
    ret = (int)res;
    if (res != (off_t)ret) {
        ret = OSAL_EOVERFLOW;
    }
    return ret;
}