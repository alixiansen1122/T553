/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: header file for overlay_load module
 * Author: audio
 */

#ifndef __CORE_OVERLAY_H__
#define __CORE_OVERLAY_H__

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OVERLAY_NAME_SIZE  16

typedef struct {
#ifdef SAP_MEM_STATISTIC
    void *(*malloc)(size_t, const char *);
    void *(*calloc)(size_t, const char *);
    void *(*realloc)(void *, size_t, const char *);
#else
    void *(*malloc)(size_t);
    void *(*calloc)(size_t, size_t);
    void *(*realloc)(void *, size_t);
#endif
    void (*free)(void *);

    int (*printf)(const char *);
    int (*vsprintf)(char *str, const char *format, va_list arg);

    /* securec functions */
    int (*memset_s)(void *dest, size_t dest_max, int c, size_t count);
    int (*memcpy_s)(void *dest, size_t dest_max, const void *src, size_t count);
    int (*memmove_s)(void *dest, size_t dest_max, const void *src, size_t count);
    int (*vsnprintf_s)(char *str, size_t dest_max, size_t size, const char *format, va_list arg);

    int (*memcmp)(const void *, const void *, size_t);
    void *(*memcpy)(void *, const void *, size_t);
    void *(*memmove)(void *, const void *, size_t);
    void *(*memset)(void *, int, size_t);
    void (*qsort)(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));
    void *(*memchr)(const void *buf, int ch, size_t count);

    /* math functions */
    long (*lrintf)(float);
    long (*lrint)(double);
    double (*floor)(double);
    double (*log)(double);
    double (*log10)(double);
    double (*pow)(double, double);
    double (*sqrt)(double);
    int (*rand)(void);
} overlay_func;

typedef struct {
    td_void (*overlay_init)(overlay_func *func, td_char *lib);
    td_void *ops;
} overlay_contex;

typedef overlay_contex *(*overlay_start_call)(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CORE_OVERLAY_H__ */
