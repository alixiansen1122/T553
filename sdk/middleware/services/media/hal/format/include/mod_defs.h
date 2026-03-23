/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: models define
* Author: Media Software Group
* Create: 2021-12-07
*/


#ifndef MOD_DEFS_H
#define MOD_DEFS_H

#include <sys/param.h>
#include <time.h>
#include <limits.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef __GNUC__
#endif

#define DO_NOTHING

/* use for parameter INPUT, *DO NOT Modify the value **/
#define IN
/* use for parameter OUTPUT, the value maybe change when return from the function
 * the init value is ingore in the function. */
#define OUT
/* use for parameter INPUT and OUTPUT */
#define IO

/* -------------------------------- */
#ifndef EXTERN
#define EXTERN extern
#endif

#define STATIC static

#define LOCALVAR  static
#define GLOBALVAR extern

/** for declaring global variable */
#define DECLARE_GLOBALVAR

/** for using global variable */
#define USE_GLOBALVAR extern

#define LOCALFUNC  static
#define EXTERNFUNC extern

/** conculate the aligned start address,according to base address and align byte counts
 * eg. 4 byte aligned,0x80001232 ALIGN_START(0x80001232,4) = 0x80001230
 */
#define ALIGN_START(v, a) (((v)) & (~((a)-1)))

/** conculate the aligned end address,according to base address and align byte counts
 * eg. 4 byte aligned,0x80001232 ALIGN_START(0x80001232,4) = 0x80001230
 */
#define ALIGN_END(v, a) (((v) & ~((a)-1)) + ((a)-1))

/** conculate the aligned next address,according to base address and align byte counts
 * eg. 4 byte aligned,0x80001232 ALIGN_START(0x80001232,4) = 0x80001230
 */
#define ALIGN_NEXT(v, a) ((((v) + ((a)-1)) & (~((a)-1))))

#define ALIGN_LENGTH(l, a) ALIGN_NEXT(l, a)

#define ALIGNTYPE_1BYTE 1
/* zsp */
#define ALIGNTYPE_2BYTE 2
/* x86... default */
#define ALIGNTYPE_4BYTE 4

#define ALIGNTYPE_8BYTE 8
/* 1 Page */
#define ALIGNTYPE_4K 4096

#define ALIGNTYPE_ZSP ALIGNTYPE_2BYTE

#define ALIGNTYPE_VIDEO ALIGNTYPE_8BYTE

#define PACK_ONE_BYTE __attribute__((__packed__))

#define MW_UNUSED(x) (void)(x)

#define SEC_TO_USEC 1000000
#define SEC_UNIT 1000

/** middleware module id */
typedef enum {
    APPID_REC = 0x0B,         /** < recoder */
    APPID_SNAP = 0x0C,        /** < snap */
    APPID_FILEMGR = 0x16,     /** < file manager */
    APPID_STORAGE = 0x18,     /** < storage */
    APPID_LOG = 0x1F,         /** < log */
    APPID_LOCALPLAYER = 0x20, /** < local player */
    APPID_RTSPSERVER = 0x22,  /** <rtsp server */
    APPID_HTTPSERVER = 0x24,  /** <http server */
    APPID_MBUF = 0x25,        /** < mbuffer manager */
    APPID_LIVESTREAM = 0x26,  /** < livestream */
    APPID_RTSPCLIENT = 0x27,  /** < rtsp client */
    APPID_DEMUXER = 0x28,     /** < demuxer */
    APPID_MP4 = 0x29,         /** < demuxer */
    APPID_DTCF = 0x2A,        /** < dtcf */
    APPID_TIMER = 0x2C,       /** < timer */
    APPID_XMP = 0x2B,         /** < xmp */
    APPID_DCF = 0x2D,         /** <dcf */
    APPID_DCF_WRAPPER = 0x2E, /** <dcf_wrapper */
    APPID_TS = 0x2F,          /** < ts */
    APPID_MSGHDL = 0x30,      /** < message handler */
    APPID_HFSM = 0x31,        /** < Hierarchical state machine */
    APPID_EVTHUB = 0x32,      /** < event hub */
    APPID_TAG_EDITOR = 0x36,  /** < tag editro */
    APPID_PICDEMUXER = 0x37,  /** < pic demuxer */
    APPID_DNG = 0x38,         /** < dng module */
    APPID_OSCSERVER = 0X3F,   /** < osc server */
    APPID_NETATE = 0x40,      /** netate */
    APPID_BUTT = 0xFF
} AppModIdE;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __MOD_DEFS_H__ */
