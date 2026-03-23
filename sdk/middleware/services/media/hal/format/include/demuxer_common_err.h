/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: demuxer module error header file
 * Author: Media Software Group
 * Create: 2017-12-22
 */

#ifndef DEMUXER_COMMON_ERR_H
#define DEMUXER_COMMON_ERR_H

#include "error_def.h"
#include "mod_defs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 0x40~0xFF */
typedef enum {
    APP_DEMUXER_ERR_OPEN_FILE = 0x40,      /* open media file failed */
    APP_DEMUXER_ERR_READ_PACKET = 0x41,    /* read one packet from media file failed */
    APP_DEMUXER_ERR_SEEK = 0x42,           /* seek in media file failed */
    APP_DEMUXER_ERR_MEM_MALLOC = 0x43,     /* memery alloc failed */
    APP_DEMUXER_ERR_PROBE = 0x44,          /* probe the media info failed */
    APP_DEMUXER_ERR_ACTION = 0x45,         /* should not exec this action here */
    APP_DEMUXER_ERR_INVALID_HANDLE = 0x46, /* handle is invalid */
    APP_DEMUXER_ERR_FREE_PACKET = 0x47,    /* free the packet failed */
    APP_DEMUXER_ERR_SET_ATTR = 0x48,       /* set demuxer attr failed */
    APP_DEMUXER_ERR_NOT_SUPPORT = 0x49,    /* current action not support */
    APP_DEMUXER_ERR_CLOSE_FILE = 0x50,     /* close media file failed */
    APP_DEMUXER_ERR_SKIP = 0x51, /* close media file failed */
    APP_DEMUXER_BUTT = 0xFF
} AppDemuxerErrCode;

/* general error code */
#define ERR_DEMUXER_NULL_PTR       APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_ERR_NULL_PTR)
#define ERR_DEMUXER_ILLEGAL_PARAM  APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_ERR_ILLEGAL_PARAM)
#define ERR_DEMUXER_OPEN_FILE      APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_OPEN_FILE)
#define ERR_DEMUXER_READ_PACKET    APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_READ_PACKET)
#define ERR_DEMUXER_SEEK           APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_SEEK)
#define ERR_DEMUXER_MEM_MALLOC     APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_MEM_MALLOC)
#define ERR_DEMUXER_PROBE          APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_PROBE)
#define ERR_DEMUXER_ACTION         APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_ACTION)
#define ERR_DEMUXER_INVALID_HANDLE \
    APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_INVALID_HANDLE)
#define ERR_DEMUXER_FREE_PACKET    APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_FREE_PACKET)
#define ERR_DEMUXER_SET_ATTR       APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_SET_ATTR)
#define ERR_DEMUXER_NOT_SUPPORT    APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_NOT_SUPPORT)
#define ERR_DEMUXER_CLOSE_FILE     APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_CLOSE_FILE)
#define ERR_DEMUXER_SKIP_FILE      APP_DEF_ERR(APPID_DEMUXER, APP_ERR_LEVEL_ERROR, APP_DEMUXER_ERR_SKIP)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DEMUXER_COMMON_ERR_H */
