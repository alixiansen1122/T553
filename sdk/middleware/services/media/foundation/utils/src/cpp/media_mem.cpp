/*
* Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
* Description: thread adapt impl
* Author: Media Software Group
* Create: 2025-03-01
*/

#include "media_mem.h"
#include <malloc.h>
#include "securec.h"
#include "media_log.h"
#ifdef HI3322_PRODUCT_FPGA
#include "osal_addr.h"
#include "chip_init.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *MediaMemMalloc(const size_t bytesNum)
{
    if (bytesNum == 0) {
        return NULL;
    }
#ifdef HI3322_PRODUCT_FPGA
    return osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, bytesNum);
#else
    return malloc(bytesNum);
#endif
}

void *MediaMemCalloc(const size_t bytesNum)
{
#ifdef HI3322_PRODUCT_FPGA
    void *tmp = osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, bytesNum);
#else
    void *tmp = malloc(bytesNum);
#endif
    if (tmp == NULL) {
        MEDIA_ERR_LOG("%s", "malloc failed!");
        return NULL;
    }

    if (memset_s(tmp, bytesNum, 0, bytesNum) != EOK) {
        MEDIA_ERR_LOG("%s", "memset_s failed!");
        MediaMemFree(tmp);
        return NULL;
    } else {
        return tmp;
    }
}

void MediaMemFree(const void* src)
{
    if (src == NULL) {
        MEDIA_ERR_LOG("%s", "don't need to free!");
        return;
    }
#ifdef HI3322_PRODUCT_FPGA
    osal_pool_mem_free(OS_EXTEND_MEM_ADDR, src);
#else
    free((void *)src);
#endif
}
#ifdef __cplusplus
};
#endif
