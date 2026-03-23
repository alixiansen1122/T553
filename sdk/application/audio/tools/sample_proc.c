/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: proc tool
 * Author: audio
 * Create: 2022-06-11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "securec.h"
#include "sample_audio_api.h"

#if (SAP_CHIP_TYPE == brandy) || (SAP_CHIP_TYPE == socmn1)
#include "drv_ai.h"
#include "drv_ao.h"
#include "drv_adec.h"
#include "drv_aenc.h"
#include "drv_sea.h"
#include "drv_ab.h"
#include "drv_sys.h"
#include "drv_adp.h"
#ifdef SAP_HAID_SUPPORT
#include "drv_haid.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ARGV_MOD_NAME_INDEX 1

typedef td_s32 (*audio_proc_exec_function)(td_void *file, td_void *private);

typedef enum {
    PROC_MOD_AI,
    PROC_MOD_SEA,
    PROC_MOD_ADEC,
    PROC_MOD_AENC,
    PROC_MOD_AO,
    PROC_MOD_AB,
    PROC_MOD_SYS,
    PROC_MOD_ADP,
#ifdef SAP_HAID_SUPPORT
    PROC_MOD_HAID,
#endif
    PROC_MOD_MAX
} proc_mod_type;

typedef struct {
    proc_mod_type mod_type;
    const td_char *name;
    audio_proc_exec_function exec_func;
} proc_mod_item;

static const proc_mod_item g_proc_mod_list[] = {
    {PROC_MOD_AI, "ai", ai_read_proc},
    {PROC_MOD_SEA, "sea", sea_read_proc},
    {PROC_MOD_ADEC, "adec", adec_read_proc},
    {PROC_MOD_AENC, "aenc", aenc_read_proc},
    {PROC_MOD_AO, "ao", ao_read_proc},
    {PROC_MOD_AB, "ab", ab_read_proc},
    {PROC_MOD_SYS, "sys", sys_read_proc},
    {PROC_MOD_ADP, "adp", adp_read_proc},
#ifdef SAP_HAID_SUPPORT
    {PROC_MOD_HAID, "haid", haid_read_proc},
#endif
};

static td_s32 check_mod_name(const td_char *mod_name)
{
    td_u32 i;

    for (i = 0; i < (sizeof(g_proc_mod_list) / sizeof(g_proc_mod_list[0])); i++) {
        if (strcasecmp(mod_name, g_proc_mod_list[i].name) == 0) {
            return EXT_SUCCESS;
        }
    }

    sap_printf("check_mod_name fail, name = %s.\n", mod_name);
    return EXT_FAILURE;
}

static td_s32 run_mod_proc(const td_char *mod_name)
{
    td_u32 i;
    td_bool offline_flag = TD_FALSE;

    for (i = 0; i < (sizeof(g_proc_mod_list) / sizeof(g_proc_mod_list[0])); i++) {
        if (strcasecmp(mod_name, g_proc_mod_list[i].name) == 0) {
            g_proc_mod_list[i].exec_func(TD_NULL, &offline_flag);
            return EXT_SUCCESS;
        }
    }

    sap_printf("run_mod_proc fail, name = %s.\n", mod_name);
    return EXT_FAILURE;
}

static td_void sample_proc_usage(const td_char *name)
{
    sap_printf("usage: %s mod_name\n", name);
    sap_printf("mod_name: ai, sea, adec, aenc, ao, ab, adp.\n");
    sap_printf("\nexamples:\n");
    sap_printf("%s ai\n", name);
    sap_printf("\n");
}

static td_s32 sample_proc_check_param(td_s32 argc, td_char *argv[])
{
    if (argc != (ARGV_MOD_NAME_INDEX + 1)) {
        sap_err_log_u32(argc);
        goto fail;
    }

    if (check_mod_name(argv[ARGV_MOD_NAME_INDEX]) != EXT_SUCCESS) {
        goto fail;
    }

    return EXT_SUCCESS;

fail:
    sample_proc_usage(argv[0]);
    return EXT_FAILURE;
}

static td_s32 sample_proc_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;

    ret = sample_proc_check_param(argc, argv);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = run_mod_proc(argv[ARGV_MOD_NAME_INDEX]);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 sample_proc(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_proc_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "-h") == 0) {
        sample_proc_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return EXT_SUCCESS;
    } else {
        return sample_proc_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
