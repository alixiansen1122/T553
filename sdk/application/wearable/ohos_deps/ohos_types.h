/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: Types defination header file.
 * Author: CompanyName
 * Create:
 */
#ifndef OHOS_TYPES_H
#define OHOS_TYPES_H

#if !defined(_WIN32)
#include "los_typedef.h"
#else
#include <windows.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*
 * Defintion of basic data types.
 * The data types are applicable to both the application and kernel.
 */
typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef signed char             int8;
typedef short                   int16;
typedef int                     int32;

#ifndef _M_IX86
typedef unsigned long long      uint64;
typedef long long               int64;
#else
typedef unsigned __int64        uint64;
typedef __int64                 int64;
#endif

#if !defined(_WIN32)
typedef int                     boolean;
#endif

typedef void                    *pHandle;

#ifndef TRUE
#define TRUE             1L
#endif

#ifndef FALSE
#define FALSE            0L
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#endif

#define OHOS_SUCCESS          0
#define OHOS_FAILURE          (-1)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef INLINE
#define INLINE        inline
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OHOS_TYPES_H */
