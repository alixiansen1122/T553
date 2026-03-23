/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: gui aef sample
 * Author: audio
 * Create: 2023-03-09
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "sample_audio_api.h"
#include "audio_aef.h"
#include "sws_aef.h"
#include "sea_aef.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define GUI_AEF_INVALID_HANDLE 0XFF

#ifndef DIAG_CMD_AUDIO_SWS_EFFECT_REQ
#define DIAG_CMD_AUDIO_SWS_EFFECT_REQ 0x2505
#endif
#ifndef DIAG_CMD_AUDIO_SEE_EFFECT_REQ
#define DIAG_CMD_AUDIO_SEE_EFFECT_REQ 0x2605
#endif

static gui_aef_str gui_aef_list[GUI_AEF_MAX];

static errcode_t diag_aef_sws_param(td_u16 cmd_id, td_pvoid param, td_u16 param_size, diag_option_t *option);
static errcode_t diag_aef_sea_param(td_u16 cmd_id, td_pvoid param, td_u16 param_size, diag_option_t *option);

static diag_cmd_reg_obj_t g_diag_audio_effect_cmd_tbl[] = {
    {DIAG_CMD_AUDIO_SWS_EFFECT_REQ, DIAG_CMD_AUDIO_SWS_EFFECT_REQ, diag_aef_sws_param},
    {DIAG_CMD_AUDIO_SEE_EFFECT_REQ, DIAG_CMD_AUDIO_SEE_EFFECT_REQ, diag_aef_sea_param},
};

static td_s32 sample_gui_aef_entry(td_void)
{
    td_s32 ret;

    ret = (td_s32)uapi_diag_register_cmd(g_diag_audio_effect_cmd_tbl,
        sizeof(g_diag_audio_effect_cmd_tbl) / sizeof(g_diag_audio_effect_cmd_tbl[0]));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_diag_register_cmd, ret);
        return ret;
    }

    for (td_u8 i = 0; i < GUI_AEF_MAX; i++) {
        gui_aef_list[i].type = GUI_AEF_MAX;
        gui_aef_list[i].handle = GUI_AEF_INVALID_HANDLE;
    }

    return EXT_SUCCESS;
}

td_void sample_gui_aef_add_handle(gui_aef_type type, td_handle handle)
{
    for (td_u8 i = 0; i < GUI_AEF_MAX; i++) {
        if (gui_aef_list[i].handle == GUI_AEF_INVALID_HANDLE) {
            gui_aef_list[i].type = type;
            gui_aef_list[i].handle = handle;
        }
    }
}

td_void sample_gui_aef_delete_handle(td_handle handle)
{
    for (td_u8 i = 0; i < GUI_AEF_MAX; i++) {
        if (gui_aef_list[i].handle == handle) {
            gui_aef_list[i].type = GUI_AEF_MAX;
            gui_aef_list[i].handle = GUI_AEF_INVALID_HANDLE;
        }
    }
}

static td_handle sample_gui_aef_get_handle(gui_aef_type type)
{
    for (td_u8 i = 0; i < GUI_AEF_MAX; i++) {
        if (gui_aef_list[i].type == type) {
            return gui_aef_list[i].handle;
        }
    }

    return GUI_AEF_INVALID_HANDLE;
}

static errcode_t diag_aef_sws_param(td_u16 cmd_id, td_pvoid param, td_u16 param_size, diag_option_t *option)
{
    errcode_t ret;
    td_handle sound;
    audio_sws_mobile_para sws_param = { 0 };
    audio_sws_mobile_para *tmp_req = (audio_sws_mobile_para *)param;
    td_u32 size = (td_u32)sizeof(audio_sws_mobile_para);

    sound = sample_gui_aef_get_handle(GUI_AEF_AO);
    if (sound == GUI_AEF_INVALID_HANDLE) {
        sap_err_log_info("invalid handle");
        sap_err_log_h32(sound);
        return EXT_FAILURE;
    }

    if (param_size != 0) {
        if (param_size < size) {
            sap_err_log_info("invalid param");
            sap_err_log_u32(param_size);
            return EXT_FAILURE;
        }

        /* set param */
        (td_void)memcpy_s(&sws_param, sizeof(sws_param), tmp_req, size);
        ret = (errcode_t)uapi_snd_set_aef_param(sound, SND_OUT_PORT_DEFAULT,
            AEF_CMD_ALG_PARAM, (const td_void *)&sws_param, size);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_snd_set_aef_param, ret);
            return EXT_FAILURE;
        }
    } else {
        /* get param */
        ret = (errcode_t)uapi_snd_get_aef_param(sound, SND_OUT_PORT_DEFAULT,
            AEF_CMD_ALG_PARAM, (td_void *)&sws_param, size);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_snd_set_aef_param, ret);
            return EXT_FAILURE;
        }
    }

    ret = uapi_diag_report_packet(cmd_id, option, (td_u8 *)&sws_param, sizeof(audio_sws_mobile_para), TD_TRUE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_diag_report_packet, ret);
        return EXT_FAILURE;
    }
    return ret;
}

static errcode_t diag_aef_sea_param(td_u16 cmd_id, td_pvoid param, td_u16 param_size, diag_option_t *option)
{
    errcode_t ret;
    td_handle sea;
    audio_vqe_param_struct sea_param = { 0 };
    audio_vqe_param_struct *tmp_req = (audio_vqe_param_struct *)param;
    td_u32 size = (td_u32)sizeof(audio_vqe_param_struct);

    sea = sample_gui_aef_get_handle(GUI_AEF_SEA);
    if (sea == GUI_AEF_INVALID_HANDLE) {
        sap_err_log_info("invalid handle");
        sap_err_log_h32(sea);
        return EXT_FAILURE;
    }

    if (param_size != 0) {
        if (param_size < size) {
            sap_err_log_info("invalid param");
            sap_err_log_u32(param_size);
            return EXT_FAILURE;
        }

        /* set param */
        (td_void)memcpy_s(&sea_param, sizeof(sea_param), tmp_req, size);
        ret = (errcode_t)uapi_sea_set_param(sea, UAPI_SEA_AFE_SEE, (const td_void *)&sea_param, size);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_set_param, ret);
            return EXT_FAILURE;
        }
    } else {
        /* get param */
        ret = (errcode_t)uapi_sea_get_param(sea, UAPI_SEA_AFE_SEE, (td_void *)&sea_param, size);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_get_param, ret);
            return EXT_FAILURE;
        }
    }
    ret = uapi_diag_report_packet(cmd_id, option, (td_u8 *)&sea_param, sizeof(audio_vqe_param_struct), TD_TRUE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_diag_report_packet, ret);
        return EXT_FAILURE;
    }
    return ret;
}

static td_s32 sample_gui_aef_exit(td_void)
{
    uapi_diag_unregister_cmd(g_diag_audio_effect_cmd_tbl,
                             sizeof(g_diag_audio_effect_cmd_tbl) / sizeof(g_diag_audio_effect_cmd_tbl[0]));

    return EXT_SUCCESS;
}

td_s32 sample_gui_aef(td_s32 argc, td_char *argv[])
{
    if ((argc == 2) && (strcmp(argv[1], "q") == 0)) { /* 2:second singular */
        return sample_gui_aef_exit();
    } else {
        return sample_gui_aef_entry();
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
