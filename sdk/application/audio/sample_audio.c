/*
 * Copyright (c) CompanyNameMagicTag 2020-2021. All rights reserved.
 * Description: source file for module operation
 */
#include <stdlib.h>
#ifdef __LITEOS__
#include <los_task.h>
#endif
#include "cmsis_os.h"

#include "thread_os.h"
#include "securec.h"
#include "sample_audio_api.h"

#include "chip_type.h"
#include "audio_struct.h"
#include "soc_uapi_audio_sys.h"
#include "sample_audio.h"

#if (SAP_CHIP_TYPE == brandy)
#include "es8311_codec.h"
#include "pm_clocks.h"
#endif /* SAP_CHIP_TYPE */

#define TEST_SUITE_TASK_STACK_SIZE 0xc00
#define TEST_SUITE_NAME "audio"
#define TEST_SUITE_MAX_ARGV_LEN 128
#ifdef BOARD_CS
#define AUDIO_MEM_PROT_CFG   0x500006a0
#define AUDIO_MEM_PROT_LOCK  0x3
#endif

#define HIFI_FREQ_176M 176947200
#define HIFI_FREQ_147M 147456000
#define HIFI_FREQ_64M 64000000

#define HIFI_INVALID_PROFILE 0xF

static td_u8 g_dsp_profile = HIFI_INVALID_PROFILE;

typedef struct {
    td_s32 argc;
    td_char argv[TEST_SUITE_MAX_ARGC][TEST_SUITE_MAX_ARGV_LEN];
} cmd_msg_queue;

typedef struct {
    td_char *name;
    td_s32 (*func)(td_s32 argc, td_char *argv[]);
    td_bool running;
} functions;

typedef struct {
    athread_handle task;
    td_bool task_active;
} sample_audio_inst;

/* only one sample instance when set to true */
static td_bool g_sample_cfg_singleton = TD_TRUE;
static sample_audio_inst *g_sample_audio_inst;

typedef struct {
    td_char *sample;
    td_char *config;
    td_char *value;
} sample_set_config_arg;

static td_void sample_set_dump_mask(td_u8 dump_mask)
{
    td_s32 ret;
    uapi_audio_debug_cfg cfg = {0};

    ret = uapi_audio_get_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_get_debug_cfg, ret);
        return;
    }

    cfg.dump_mask = dump_mask;
    ret = uapi_audio_set_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_debug_cfg, ret);
        return;
    }

    sap_alert_log_info("dump mask status");
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_AI) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_SEA) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_AENC) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_ADEC) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_TRACK) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_AO) != 0);
}

static td_void sample_set_proc_mask(td_u8 proc_mask)
{
    td_s32 ret;
    uapi_audio_debug_cfg cfg = {0};

    ret = uapi_audio_get_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_get_debug_cfg, ret);
        return;
    }

    cfg.proc_mask = proc_mask;
    ret = uapi_audio_set_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_debug_cfg, ret);
        return;
    }

    sap_alert_log_info("proc mask status");
    sap_alert_log_bool((proc_mask & UAPI_AUDIO_PROC_MASK_UART) != 0);
    sap_alert_log_bool((proc_mask & UAPI_AUDIO_PROC_MASK_DIAG_SHELL) != 0);
}

static td_void sample_set_tws_role(td_u8 role)
{
    if (role != AUDIO_TWS_ROLE_MASTER && role != AUDIO_TWS_ROLE_SLAVE && role != AUDIO_TWS_ROLE_FREEMAN) {
        sap_err_log_info("invalid tws role, need to set tws role as 0/1/2(master/salve/freeman)\n");
        return;
    }
    uapi_audio_set_tws_role((uapi_audio_tws_role)role);
}

static td_void sample_set_dsp_profile(td_u8 profile)
{
    g_dsp_profile = profile;
    sap_alert_log_u32(g_dsp_profile);
}

static td_s32 sample_set_config(td_s32 argc, td_char *argv[])
{
    td_u8 tmp_mask;
    sample_set_config_arg *arg = (sample_set_config_arg *)argv;

    if (argc == 0x2 && (strcmp("q", argv[1]) == 0)) {
        return EXT_SUCCESS;
    }

    if (argc != sizeof(sample_set_config_arg) / sizeof(td_char *)) {
        sap_err_log_info("nothing changed!");
        return EXT_SUCCESS;
    }

    tmp_mask = (td_u8)strtoul(arg->value, TD_NULL, 0);
    if (strcmp(arg->config, "singleton") == 0) {
        g_sample_cfg_singleton = ((strtoul(arg->value, TD_NULL, 0)) == 0) ? TD_FALSE : TD_TRUE;
        sap_alert_log_u32(g_sample_cfg_singleton);
    } else if (strcmp(arg->config, "dump_mask") == 0) {
        sample_set_dump_mask(tmp_mask);
    } else if (strcmp(arg->config, "proc_mask") == 0) {
        sample_set_proc_mask(tmp_mask);
    } else if (strcmp(arg->config, "tws_role") == 0) {
        sample_set_tws_role(tmp_mask);
    }  else if (strcmp(arg->config, "profile") == 0) {
        sample_set_dsp_profile(tmp_mask);
    } else {
        sap_err_log_info("nothing changed!");
    }

    return EXT_SUCCESS;
}

#if (SAP_CHIP_TYPE == socmn1)
FILE *fopen(const char *name, const char *type)
{
    sap_unused(name);
    sap_unused(type);

    return NULL;
}

int fclose(FILE *f)
{
    sap_unused(f);

    return 0;
}

size_t fread(void *v, size_t size, size_t n, FILE *f)
{
    sap_unused(v);
    sap_unused(size);
    sap_unused(n);
    sap_unused(f);

    return 0;
}

size_t fwrite(const void *v, size_t size, size_t n, FILE *f)
{
    sap_unused(v);
    sap_unused(size);
    sap_unused(n);
    sap_unused(f);

    return 0;
}

td_s32 fseek(FILE *f, td_slong offset, td_s32 fromwhere)
{
    unused(f);
    unused(offset);
    unused(fromwhere);
    return 0;
}

td_slong ftell(FILE *f)
{
    unused(f);
    return 0;
}

void rewind(FILE *f)
{
    sap_unused(f);
}
#endif

static functions g_audio_func[] = {
#ifndef CONFIG_PROT_CORE
    {"sample_ai", sample_ai, TD_FALSE},
    {"sample_ao", sample_ao, TD_FALSE},
    {"sample_encode", sample_encode, TD_FALSE},
    {"sample_cast_aenc_play", sample_cast_aenc_play, TD_FALSE},
    {"sample_encode_play", sample_encode_play, TD_FALSE},
    {"sample_decode", sample_decode, TD_FALSE},
    {"sample_ai_aenc", sample_ai_aenc, TD_FALSE},
    {"sample_ai_sea_aenc", sample_ai_sea_aenc, TD_FALSE},
    {"sample_ai_ao", sample_ai_ao, TD_FALSE},
    {"sample_sea", sample_sea, TD_FALSE},
	{"sample_gui_aef", sample_gui_aef, TD_FALSE},
#if defined(SAP_HAID_SUPPORT)
    {"sample_haid", sample_haid, TD_FALSE},
    {"sample_spk_calib", sample_spk_calib, TD_FALSE},
#endif /* SAP_HAID_SUPPORT */
#if defined(SAP_DPM_SUPPORT)
    {"sample_dpm", sample_dpm, TD_FALSE},
#endif /* SAP_DPM_SUPPORT */
    {"sample_lpwk", sample_lpwk, TD_FALSE},
    {"sample_dump", sample_dump, TD_FALSE},
    {"sample_phone", sample_phone_application, TD_FALSE},
    {"sample_proc", sample_proc, TD_FALSE},
#else
    {"sample_phone", sample_phone_protocol, TD_FALSE},
#endif
    {"sample_set_config", sample_set_config, TD_FALSE},
};

static osMessageQueueId_t g_msg_queue_id = TD_NULL;
static cmd_msg_queue g_cmd_msg;

static td_void audio_auto_exit_function(td_s32 argc, td_char *argv[], td_u32 fun_id)
{
    td_u32 i;
    td_char *q = "q";
    td_char *cmd[] = {argv[0], q};

    if (!g_sample_cfg_singleton) {
        return;
    }

    /* audio func q */
    if (argc == 0x2 && (strcmp("q", argv[1]) == 0)) {
        g_audio_func[fun_id].running = TD_FALSE;
        return;
    }

    for (i = 0; i < (sizeof(g_audio_func) / sizeof(g_audio_func[0])); i++) {
        if (g_audio_func[i].running == TD_TRUE) {
            (td_void)g_audio_func[i].func(0x2, cmd);
            g_audio_func[i].running = TD_FALSE;
        }
    }

    g_audio_func[fun_id].running = TD_TRUE;
}

static td_void update_dsp_profile(td_void)
{
#if (SAP_CHIP_TYPE == brandy)
#ifndef CONFIG_PROT_CORE
    /* brandy默认进入低功耗模式，按最小频点工作，启动sample业务前需更新到所需频点 */
    pm_hifi_mode_t hifi_mode;
    if (SAP_DSP_CLK_HZ > HIFI_FREQ_176M) {
        hifi_mode = HIFI_LPM_CLK_HIGH_LEVEL;
    } else if (SAP_DSP_CLK_HZ > HIFI_FREQ_147M) {
        hifi_mode = HIFI_LPM_CLK_LEVEL2;
    }  else if (SAP_DSP_CLK_HZ > HIFI_FREQ_64M) {
        hifi_mode = HIFI_LPM_CLK_LEVEL1;
    } else {
        hifi_mode = HIFI_LPM_CLK_LOW_LEVEL;
    }

    if ((td_u32)g_dsp_profile < HIFI_LPM_CLK_MAX) {
        /* 对于需要指定DSP工作频点的场景，使用上层用户配置的频点 */
        hifi_mode = g_dsp_profile;
    }
    sap_printf("SAP_DSP_CLK_HZ = %d, hifi_mode = %d.\n", SAP_DSP_CLK_HZ, hifi_mode);

    uapi_set_hifi_mode(hifi_mode);
#endif
#endif
}

td_void audio_execute_function(td_s32 argc, td_char *argv[])
{
    td_u32 i;
    td_s32 ret;

    if (argc == 0) {
        return;
    }

    if (strcmp("help", *argv) == 0) {
        return;
    }

    for (i = 0; i < (sizeof(g_audio_func) / sizeof(g_audio_func[0])); i++) {
        if (strcmp(*argv, g_audio_func[i].name) != 0) {
            continue;
        }

        if (strcmp(*argv, "sample_proc") != 0) {
            /* do not need to quit when reading proc */
            update_dsp_profile();
            audio_auto_exit_function(argc, argv, i);
        }

        ret = g_audio_func[i].func(argc, argv);
        sap_alert_log_str(g_audio_func[i].name);
        if (ret != EXT_SUCCESS) {
            sap_err_log_info("sample execute failed.");
        } else {
            sap_alert_log_info("sample execute succeeded.");
        }
        return;
    }
}

static td_void audio_task_entry(const td_void *param)
{
    td_s32 ret;
    td_u32 i;
    td_char *msg_argv[TEST_SUITE_MAX_ARGC];

    sample_audio_inst *inst = (sample_audio_inst *)param;

    while (inst->task_active) {
        ret = osMessageQueueGet(g_msg_queue_id, (td_void *)&g_cmd_msg, NULL, osWaitForever);
        if (ret != osOK) {
            continue;
        }

        for (i = 0; i < TEST_SUITE_MAX_ARGC; i++) {
            msg_argv[i] = g_cmd_msg.argv[i];
        }
        audio_execute_function(g_cmd_msg.argc, msg_argv);
    }
    sap_alert_log_info("thread exit.");
    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 audio_create_task(sample_audio_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    g_msg_queue_id = osMessageQueueNew(1, sizeof(cmd_msg_queue), TD_NULL);

    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = TEST_SUITE_TASK_STACK_SIZE;
    attr.name = TEST_SUITE_NAME;
    ret = athread_create(&inst->task, (td_void (*)(td_void *))audio_task_entry, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }

    return EXT_SUCCESS;
}

td_s32 audio_init(td_void)
{
    if (g_sample_audio_inst != TD_NULL) {
        sap_printf("inst had already existed!");
        return EXT_FAILURE;
    }

    g_sample_audio_inst = (sample_audio_inst *)malloc(sizeof(sample_audio_inst));
    if (g_sample_audio_inst == TD_NULL) {
        sap_printf("sample audio inst alloc failed\n");
        return EXT_FAILURE;
    }

    sample_audio_inst *inst = g_sample_audio_inst;
    memset_s(inst, sizeof(*inst), 0, sizeof(sample_audio_inst));

    audio_create_task(inst);
#if (SAP_CHIP_TYPE == brandy)
#ifndef CONFIG_PROT_CORE
    return uapi_audio_init();
#else
    sap_alert_log_info("execute succeeded.");
    return EXT_SUCCESS;
#endif
#endif /* SAP_CHIP_TYPE */

#ifdef BOARD_CS
    uapi_reg_write32(AUDIO_MEM_PROT_CFG, AUDIO_MEM_PROT_LOCK); /* lock audio memory access */
#endif
    return EXT_SUCCESS;
}

static td_void audio_destroy_task(sample_audio_inst *inst)
{
    if (inst->task != TD_NULL) {
        inst->task_active = TD_FALSE;
        athread_exit(inst->task);
        inst->task = TD_NULL;
    }
}

td_s32 audio_deinit(td_void)
{
    audio_destroy_task(g_sample_audio_inst);
    free(g_sample_audio_inst);
    g_sample_audio_inst = TD_NULL;
    return EXT_SUCCESS;
}
