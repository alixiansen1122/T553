/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: error define
* Author: Media Software Group
* Create: 2021-12-07
*/

#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** error level */
typedef enum {
    APP_ERR_LEVEL_DEBUG = 0, /* debug-level */
    APP_ERR_LEVEL_INFO,      /* informational */
    APP_ERR_LEVEL_NOTICE,    /* normal but significant condition */
    APP_ERR_LEVEL_WARNING,   /* warning conditions */
    APP_ERR_LEVEL_ERROR,     /* error conditions */
    APP_ERR_LEVEL_CRIT,      /* critical conditions */
    APP_ERR_LEVEL_ALERT,     /* action must be taken immediately */
    APP_ERR_LEVEL_FATAL,     /* just for compatibility with previous version */
    APP_ERR_LEVEL_BUTT
} AppErrLevelE;

/** common error code  0x01~0x11,module private error code define from 0x40 */
typedef enum {
    APP_ERR_ILLEGAL_PARAM = 0x01,   /* at lease one parameter is illagal
                               * eg, an illegal enumeration value */
    APP_ERR_NULL_PTR = 0x02,        /*  using a NULL point */
    APP_ERR_PARAM_OVERRANGE = 0x03, /* param out of range */
    APP_ERR_NOT_SUPPORT = 0x04,     /*  operation or type is not supported by NOW */
    APP_ERR_NOT_CONFIG = 0x05,      /*  try to enable or initialize system, device
                              ** or channel, before configing attribute */
    APP_ERR_NOT_PERM = 0x06,        /*  operation is not permitted
                              ** eg, try to change static attribute */
    APP_ERR_NOMEM = 0x07,           /*  failure caused by malloc memory */
    APP_ERR_SYS_NOTREADY = 0x08,        /*  System is not ready, maybe not initialed or
                              ** loaded. Returning the error code when opening
                              ** a device file failed. */
    APP_ERR_BADADDR = 0x09,         /*  bad address,
                              ** eg. used for copy_from_user & copy_to_user */
    APP_ERR_BUSY = 0x10,            /*  resource is busy,
                              ** eg. destroy a venc chn without unregister it */
    APP_ERR_BUTT = 0x11,            /*  maxium code, private error code of all modules
                              ** must be greater than it */
} AppErrCodeE;

#define ERR_APP_APPID     (0x80000000L + 0x21000000L)
#define ERR_EXT_APP_APPID (0x80000000L + 0x22000000L)

#define APP_DEF_ERR(module, level, errid) \
    ((int32_t)((ERR_APP_APPID) | ((module) << 16) | ((level) << 13) | (errid)))

#define EXT_APP_DEF_ERR(module, level, errid) \
    ((int32_t)((ERR_EXT_APP_APPID) | ((module) << 16) | ((level) << 13) | (errid)))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */
#endif  // __ERROR_DEF_H__
