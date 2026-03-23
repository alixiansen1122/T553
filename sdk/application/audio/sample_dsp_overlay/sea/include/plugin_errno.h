/*
 * Copyright (c) CompanyNameMagicTag 2019-2019. All rights reserved.
 * Description: plugin errno
 * Author: Plugin
 */

#ifndef __PLUGIN_ERRNO_H__
#define __PLUGIN_ERRNO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PLUGIN_SUCCESS 0
#define PLUGIN_FAILURE (-1)

/* Plugin error no: [0x8000_F000,0x8000_F0FF) */
typedef enum {
    PLG_ERR_INVALID_PARAM = 0x8000F000,
    PLG_ERR_INVALID_ID,
    PLG_ERR_INVALID_PORT,
    PLG_ERR_INVALID_HANDLE,
    PLG_ERR_MEM_ALLOC,
    PLG_ERR_MEM_COPY,
    PLG_ERR_MEM_OVERLOAD,
    PLG_ERR_MEM_EXCEPTION,
    PLG_ERR_BUF_EMPTY,
    PLG_ERR_BUF_FULL,
    PLG_ERR_BUF_UNDERFLOW,
    PLG_ERR_BUF_OVERFLOW,
    PLG_ERR_INST_FULL,
    PLG_ERR_INST_BUSY,
    PLG_ERR_INST_STOP,
    PLG_ERR_INIT_FAILED,
    PLG_ERR_DEINIT_FAILED,
    PLG_ERR_OPEN_FAILED,
    PLG_ERR_CLOSE_FAILED,
    PLG_ERR_NOT_INIT,
    PLG_ERR_NOT_OPEN,
    PLG_ERR_NOT_SUPPORTED,
    PLG_ERR_NOT_AVAILABLE,
    PLG_ERR_NULL_PTR,
    PLG_ERR_TIME_OUT,
} plg_errno;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PLUGIN_ERRNO_H__ */
