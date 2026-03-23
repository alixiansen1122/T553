/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: svr dynload implementation.
 * Author:
 * Create: 2024-10-30
 */

#ifndef _SVR_DYNLOAD_H__
#define _SVR_DYNLOAD_H__
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
void dynload_load_library(const char* path);
void dynload_unload_library(void);
void* dynload_get_symbol(char* name);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
