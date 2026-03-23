/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: dump tool
 * Author: audio
 * Create: 2022-03-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "securec.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sea.h"
#include "sample_audio_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ARGV_FILE0_NAME_INDEX 1
#define ARGV_FILE0_SIZE_INDEX 2
#define ARGV_MOD0_NAME_INDEX 3
#define ARGV_MOD0_HANDLE_INDEX 4
#define ARGV_MOD0_OUTPUT_TYPE_INDEX 5

#define ARGV_FILE1_NAME_INDEX 6
#define ARGV_FILE1_SIZE_INDEX 7
#define ARGV_MOD1_NAME_INDEX 8
#define ARGV_MOD1_HANDLE_INDEX 9
#define ARGV_MOD1_OUTPUT_TYPE_INDEX 10

#define BYTES_IN_1KB 1024

#define FILE_SIZE_KB_MIN 1 /* 1kB */
#define FILE_SIZE_KB_MAX (16 * BYTES_IN_1KB * BYTES_IN_1KB) /* 16GB */

#define MODULE_NAME_LEN 8

#define DUMP_INST_NUM_MAX 2

#if (SAP_CHIP_TYPE == brandy)
#define DUMP_ADP_BUF_SIZE_MAX 4096 /* adp buf size limited by soc */
#else
#define DUMP_ADP_BUF_SIZE_MAX 65536
#endif

#if (SAP_CHIP_TYPE == socmn1)
#define DUMP_RAM_ADDR0 0x5818c000
#else
#define DUMP_RAM_ADDR0 FPGA_DUMP_DATA_ADDR
#endif
#define DUMP_RAM_LEN0 (FPGA_DUMP_DATA_SIZE / 2)

#define DUMP_RAM_ADDR1 (FPGA_DUMP_DATA_ADDR + DUMP_RAM_LEN0)
#define DUMP_RAM_LEN1 DUMP_RAM_LEN0

typedef enum {
    DUMP_MOD_AI,
    DUMP_MOD_SEA,
    DUMP_MOD_ADEC,
    DUMP_MOD_AENC,
    DUMP_MOD_TRACK,
    DUMP_MOD_SOUND,
    DUMP_MOD_MAX
} dump_mod_type;

typedef enum {
    DUMP_SAVE_FILE,
    DUMP_SAVE_RAM,
    DUMP_SAVE_MAX
} dump_save_type;

typedef enum {
    DUMP_DATA_FRAME,
    DUMP_DATA_STREAM,
    DUMP_DATA_MAX
} dump_data_type;

typedef struct {
    dump_mod_type mod_type;
    const td_char *name;
} dump_mod_name_item;

static const dump_mod_name_item g_dump_mod_name_list[] = {
    {DUMP_MOD_AI, "ai"},
    {DUMP_MOD_SEA, "sea"},
    {DUMP_MOD_ADEC, "adec"},
    {DUMP_MOD_AENC, "aenc"},
    {DUMP_MOD_TRACK, "track"},
    {DUMP_MOD_SOUND, "sound"},
};

typedef struct {
    td_u32 file_size;
    td_char file_path[FILE_PATH_LEN];
    td_char mod_name[MODULE_NAME_LEN];
    td_handle h_mod;
    td_u32 mod_out_type;

    td_bool param_setted;

    dump_save_type save_type;
    td_u32 save_offset;
} sap_test_dump_param;

typedef struct {
    td_bool task_enable;
    athread_handle task;

    dump_save_type save_type;
    td_bool save_over;
    td_u64 save_cur_size; /* max 16GB, need to use 64bit */
    td_u64 save_limit_size;
    /* for save to ram */
    td_u8 *save_addr;
    td_u32 save_offset;
    /* for save to file */
    FILE *h_file;
    td_char file_path[FILE_PATH_LEN];

    td_handle h_adp;

    dump_mod_type mod_type;
    td_handle h_mod;
    td_u32 mod_out_type;

    td_bool to_be_used;
    td_bool run_state;
} sap_test_dump_inst;

static sap_test_dump_inst g_test_dump_inst[DUMP_INST_NUM_MAX] = {0};
static td_bool g_dump_sys_init_flag = TD_FALSE;

static td_s32 check_mod_name(const td_char *mod_name, td_u32 name_size)
{
    td_u32 i;
    for (i = 0; i < (sizeof(g_dump_mod_name_list) / sizeof(g_dump_mod_name_list[0])); i++) {
        if (strcasecmp(mod_name, g_dump_mod_name_list[i].name) == 0) {
            return EXT_SUCCESS;
        }
    }

    sap_unused(name_size);
    return EXT_FAILURE;
}

static td_s32 get_mod_type(const td_char *mod_name, dump_mod_type *type)
{
    td_u32 i;
    for (i = 0; i < (sizeof(g_dump_mod_name_list) / sizeof(g_dump_mod_name_list[0])); i++) {
        if (strcasecmp(mod_name, g_dump_mod_name_list[i].name) == 0) {
            *type = g_dump_mod_name_list[i].mod_type;
            return EXT_SUCCESS;
        }
    }
    return EXT_FAILURE;
}

static const td_char *get_mod_name(dump_mod_type type)
{
    td_u32 i;
    for (i = 0; i < (sizeof(g_dump_mod_name_list) / sizeof(g_dump_mod_name_list[0])); i++) {
        if (type == g_dump_mod_name_list[i].mod_type) {
            return g_dump_mod_name_list[i].name;
        }
    }
    return "unknown";
}

static td_s32 sea_check_output_type(uapi_sea_output_type sea_type)
{
    if (sea_type >= UAPI_SEA_OUTPUT_MAX) {
        sap_printf("check sea_type fail, sea_type = %u.\n", sea_type);
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 snd_check_output_port(uapi_snd_out_port ao_port)
{
    if (!(ao_port >= UAPI_SND_OUT_PORT_DAC0 && ao_port <= UAPI_SND_OUT_PORT_DAC3) &&
        !(ao_port >= UAPI_SND_OUT_PORT_I2S0 && ao_port <= UAPI_SND_OUT_PORT_I2S4) &&
        !(ao_port == UAPI_SND_OUT_PORT_SPDIF0) &&
        !(ao_port >= UAPI_SND_OUT_PORT_HDMI0 && ao_port <= UAPI_SND_OUT_PORT_HDMI1) &&
        !(ao_port >= UAPI_SND_OUT_PORT_ARC0 && ao_port <= UAPI_SND_OUT_PORT_ARC1) &&
        !(ao_port >= UAPI_SND_OUT_PORT_USB0 && ao_port <= UAPI_SND_OUT_PORT_USB1) &&
        !(ao_port >= UAPI_SND_OUT_PORT_BT0 && ao_port <= UAPI_SND_OUT_PORT_BT1) &&
        !(ao_port >= UAPI_SND_OUT_PORT_CAST0 && ao_port <= UAPI_SND_OUT_PORT_CAST1)) {
        sap_printf("check ao_port fail, ao_port = %u.\n", ao_port);
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 check_mod_output_type(const td_char *mod_name, td_u32 name_size, td_u32 output_type)
{
    td_s32 ret;
    dump_mod_type mod_type;

    ret = get_mod_type(mod_name, &mod_type);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(get_mod_type, ret);
        return ret;
    }

    switch (mod_type) {
        case DUMP_MOD_SEA: {
            uapi_sea_output_type sea_type = (uapi_sea_output_type)output_type;
            ret = sea_check_output_type(sea_type);
            break;
        }

        case DUMP_MOD_SOUND: {
            uapi_snd_out_port ao_port = (uapi_snd_out_port)output_type;
            ret = snd_check_output_port(ao_port);
            break;
        }

        default:
            ret = EXT_SUCCESS;
            break;
    }

    sap_unused(name_size);
    return ret;
}

static td_s32 dump_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    g_dump_sys_init_flag = TD_TRUE;
    return EXT_SUCCESS;
}

static td_void dump_sys_deinit(td_void)
{
    td_s32 ret;

    if (g_dump_sys_init_flag == TD_FALSE) {
        return;
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }

    g_dump_sys_init_flag = TD_FALSE;
}

static td_void sample_dump_usage(const td_char *name)
{
    sap_printf("\ndump usage: %s file1_path file1_size mod1_name mod1_handle mod1_output "
        "[file2_path file2_size mod2_name mod2_handle mod2_output]\n", name);
    sap_printf("\nfile path: should be complate path. It means save to ram when path is null.\n");
    sap_printf("file size: should be [1, 16777216], it means [1kB, 16GB]. "
        "The size limit will be tighter when saving to ram.\n");
    sap_printf("mod_name: ai, sea, adec, aenc, track, sound.\n");
    sap_printf("mod_handle: module handle is u32 in hex.\n");
    sap_printf("mod_output: module output type is u32, only valid for sea & sound.\n");
    sap_printf("\nexamples:\n");
    sap_printf("1. save 1024kB from ai handle(0x222) in /bin/vs/sd0p0/ai.pcm:\n"
        "    %s /bin/vs/sd0p0/ai.pcm 1024 ai 0x222 0\n", name);
    sap_printf("2. save 512kB from sea handle(0x333) output_type(3) in /bin/vs/sd0p0/sea.pcm:\n"
        "    %s /bin/vs/sd0p0/sea.pcm 512 sea 0x333 3\n", name);
    sap_printf("3. save 1024kB from ai handle(0x222) in /bin/vs/sd0p0/ai.pcm, "
        "save 512kB from sea handle(0x333) output_type(3) in /bin/vs/sd0p0/sea.pcm:\n"
        "    %s /bin/vs/sd0p0/ai.pcm 1024 ai 0x222 0 /bin/vs/sd0p0/sea.pcm 512 sea 0x333 3\n", name);
    sap_printf("4. save 256kB from ai handle(0x222) to ram:\n"
        "    %s null 256 ai 0x222 0\n", name);
    sap_printf("\n");
}

static td_s32 sample_dump_check_file_size(const sap_test_dump_param *param, td_u32 dump_file_size)
{
    if (dump_file_size < FILE_SIZE_KB_MIN || dump_file_size > FILE_SIZE_KB_MAX) {
        sap_printf("wrong file_size = %u kB, shoule be [%d, %d].", dump_file_size, FILE_SIZE_KB_MIN, FILE_SIZE_KB_MAX);
        return EXT_FAILURE;
    }

    if (param->save_type == DUMP_SAVE_RAM && (dump_file_size * BYTES_IN_1KB) > DUMP_RAM_LEN0) {
        sap_printf("wrong file_size = %u kB, shoule be <= %d.", dump_file_size, DUMP_RAM_LEN0 / BYTES_IN_1KB);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 dump_entry_prase_mod0_param(td_char *argv[], sap_test_dump_param *param0)
{
    td_u32 file_size;
    td_u32 output_type;
    td_s32 ret;
    td_handle h_mod;

    param0->param_setted = TD_FALSE;

    /* file path */
    ret = strncpy_s(param0->file_path, sizeof(param0->file_path) - 1,
        argv[ARGV_FILE0_NAME_INDEX], strlen(argv[ARGV_FILE0_NAME_INDEX]));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return EXT_FAILURE;
    }
    if (strcmp(param0->file_path, "null") == 0) {
        param0->save_type = DUMP_SAVE_RAM;
        param0->save_offset = 0;
    } else {
        param0->save_type = DUMP_SAVE_FILE;
    }

    /* file size */
    file_size = (td_handle)strtol(argv[ARGV_FILE0_SIZE_INDEX], TD_NULL, 0);
    ret = sample_dump_check_file_size(param0, file_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dump_check_file_size, ret);
        return EXT_FAILURE;
    }
    param0->file_size = file_size;

    /* module name */
    ret = strncpy_s(param0->mod_name, sizeof(param0->mod_name) - 1,
        argv[ARGV_MOD0_NAME_INDEX], strlen(argv[ARGV_MOD0_NAME_INDEX]));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return EXT_FAILURE;
    }
    if (check_mod_name(param0->mod_name, sizeof(param0->mod_name)) != EXT_SUCCESS) {
        sap_printf("mod check name fail, name = %s.\n", param0->mod_name);
        return EXT_FAILURE;
    }

    /* module handle */
    h_mod = (td_handle)strtoul(argv[ARGV_MOD0_HANDLE_INDEX], TD_NULL, 16); /* 16 represents Hexadecimal */
    if (h_mod == 0) {
        sap_err_log_ret(h_mod);
        return EXT_FAILURE;
    }
    param0->h_mod = h_mod;

    /* module output type */
    output_type = (td_u32)strtoul(argv[ARGV_MOD0_OUTPUT_TYPE_INDEX], TD_NULL, 0);
    if (check_mod_output_type(param0->mod_name, sizeof(param0->mod_name), output_type) != EXT_SUCCESS) {
        sap_printf("mod check output type fail, output_type = %u.\n", output_type);
        return EXT_FAILURE;
    }
    param0->mod_out_type = output_type;

    param0->param_setted = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 dump_entry_prase_mod1_param(td_char *argv[], sap_test_dump_param *param1)
{
    td_u32 file_size;
    td_u32 output_type;
    td_s32 ret;
    td_handle h_mod;

    param1->param_setted = TD_FALSE;

    /* file path */
    ret = strncpy_s(param1->file_path, sizeof(param1->file_path) - 1,
        argv[ARGV_FILE1_NAME_INDEX], strlen(argv[ARGV_FILE1_NAME_INDEX]));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return EXT_FAILURE;
    }
    if (strcmp(param1->file_path, "null") == 0) {
        param1->save_type = DUMP_SAVE_RAM;
        param1->save_offset = DUMP_RAM_LEN0;
    } else {
        param1->save_type = DUMP_SAVE_FILE;
    }

    /* file size */
    file_size = (td_u32)strtol(argv[ARGV_FILE1_SIZE_INDEX], TD_NULL, 0);
    ret = sample_dump_check_file_size(param1, file_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dump_check_file_size, ret);
        return EXT_FAILURE;
    }
    param1->file_size = file_size;

    /* module name */
    ret = strncpy_s(param1->mod_name, sizeof(param1->mod_name) - 1,
        argv[ARGV_MOD1_NAME_INDEX], strlen(argv[ARGV_MOD1_NAME_INDEX]));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return EXT_FAILURE;
    }
    if (check_mod_name(param1->mod_name, sizeof(param1->mod_name)) != EXT_SUCCESS) {
        sap_printf("mod1 check name fail, name = %s.\n", param1->mod_name);
        return EXT_FAILURE;
    }

    /* module handle */
    h_mod = (td_handle)strtoul(argv[ARGV_MOD1_HANDLE_INDEX], TD_NULL, 16); /* 16 represents Hexadecimal */
    if (h_mod == 0) {
        sap_err_log_ret(h_mod);
        return EXT_FAILURE;
    }
    param1->h_mod = h_mod;

    /* module output type */
    output_type = (td_u32)strtoul(argv[ARGV_MOD1_OUTPUT_TYPE_INDEX], TD_NULL, 0);
    if (check_mod_output_type(param1->mod_name, sizeof(param1->mod_name), output_type) != EXT_SUCCESS) {
        sap_printf("mod check output type fail, output_type = %u.\n", output_type);
        return EXT_FAILURE;
    }
    param1->mod_out_type = output_type;

    param1->param_setted = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 dump_set_mod_param(sap_test_dump_inst *inst, const sap_test_dump_param *param)
{
    td_s32 ret;

    /* save type */
    inst->save_type = param->save_type;
    inst->save_offset = param->save_offset;

    /* file path */
    ret = strncpy_s(inst->file_path, sizeof(inst->file_path) - 1,
        param->file_path, strlen(param->file_path));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return EXT_FAILURE;
    }

    /* file size */
    inst->save_limit_size = (td_u64)param->file_size * BYTES_IN_1KB;
    inst->save_cur_size = 0;

    /* module type */
    if (get_mod_type(param->mod_name, &(inst->mod_type)) != EXT_SUCCESS) {
        sap_printf("get mod type fail, name = %s.\n", param->mod_name);
        return EXT_FAILURE;
    }

    /* module handle */
    inst->h_mod = param->h_mod;

    /* module output type */
    inst->mod_out_type = param->mod_out_type;

    inst->to_be_used = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 dump_entry_prase_param(td_s32 argc, td_char *argv[])
{
    td_s32 ret = EXT_FAILURE;
    sap_test_dump_param param0 = {0};
    sap_test_dump_param param1 = {0};

    if (argc != (ARGV_MOD0_OUTPUT_TYPE_INDEX + 1) && argc != (ARGV_MOD1_OUTPUT_TYPE_INDEX + 1)) {
        sap_err_log_u32(argc);
        goto fail;
    }

    ret = dump_entry_prase_mod0_param(argv, &param0);
    if (ret != EXT_SUCCESS) {
        goto fail;
    }

    if (argc > (ARGV_MOD0_OUTPUT_TYPE_INDEX + 1)) {
        ret = dump_entry_prase_mod1_param(argv, &param1);
        if (ret != EXT_SUCCESS) {
            goto fail;
        }
    }

    if (param0.param_setted == TD_TRUE) {
        (td_void)memset_s(&g_test_dump_inst[0], sizeof(g_test_dump_inst[0]),
            0x00, sizeof(g_test_dump_inst[0]));
        ret = dump_set_mod_param(&(g_test_dump_inst[0]), &param0);
        if (ret != EXT_SUCCESS) {
            goto fail;
        }
    }

    if (param1.param_setted == TD_TRUE) {
        (td_void)memset_s(&g_test_dump_inst[1], sizeof(g_test_dump_inst[1]),
            0x00, sizeof(g_test_dump_inst[1]));
        ret = dump_set_mod_param(&(g_test_dump_inst[1]), &param1);
        if (ret != EXT_SUCCESS) {
            goto fail;
        }
    }

    return EXT_SUCCESS;

fail:
    sample_dump_usage(argv[0]);
    return ret;
}

static td_s32 dump_mod_attach(sap_test_dump_inst *dump_inst)
{
    td_s32 ret;

    switch (dump_inst->mod_type) {
        case DUMP_MOD_AI:
            ret = uapi_ai_attach_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_SEA: {
            uapi_sea_output_type type = (uapi_sea_output_type)dump_inst->mod_out_type;
            ret = uapi_sea_attach_output(dump_inst->h_mod, type, dump_inst->h_adp);
            break;
        }

        case DUMP_MOD_ADEC:
            ret = uapi_adec_attach_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_AENC:
            ret = uapi_aenc_attach_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_TRACK:
            ret = uapi_snd_attach_track_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_SOUND: {
            uapi_snd_out_port port = (uapi_snd_out_port)dump_inst->mod_out_type;
            ret = uapi_snd_attach_output(dump_inst->h_mod, port, dump_inst->h_adp);
            break;
        }

        default:
            return EXT_FAILURE;
    }

    return ret;
}

static td_s32 dump_mod_detach(sap_test_dump_inst *dump_inst)
{
    td_s32 ret;

    switch (dump_inst->mod_type) {
        case DUMP_MOD_AI:
            ret = uapi_ai_detach_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_SEA: {
            uapi_sea_output_type type = (uapi_sea_output_type)dump_inst->mod_out_type;
            ret = uapi_sea_detach_output(dump_inst->h_mod, type, dump_inst->h_adp);
            break;
        }

        case DUMP_MOD_ADEC:
            ret = uapi_adec_detach_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_AENC:
            ret = uapi_aenc_detach_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_TRACK:
            ret = uapi_snd_detach_track_output(dump_inst->h_mod, dump_inst->h_adp);
            break;

        case DUMP_MOD_SOUND: {
            uapi_snd_out_port port = (uapi_snd_out_port)dump_inst->mod_out_type;
            ret = uapi_snd_detach_output(dump_inst->h_mod, port, dump_inst->h_adp);
            break;
        }

        default:
            return EXT_FAILURE;
    }

    return ret;
}

static td_void dump_close_inst(sap_test_dump_inst *dump_inst)
{
    td_s32 ret;

    if (dump_inst->task_enable) {
        dump_inst->task_enable = TD_FALSE;

        if (dump_inst->task != TD_NULL) {
            athread_exit(dump_inst->task);
            dump_inst->task = TD_NULL;
        }
    }

    ret = dump_mod_detach(dump_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dump_mod_detach, ret);
    }

    if (dump_inst->h_file != TD_NULL) {
        fclose(dump_inst->h_file);
        dump_inst->h_file = TD_NULL;
    }

    if (dump_inst->h_adp != 0) {
        uapi_adp_destroy(dump_inst->h_adp);
        dump_inst->h_adp = 0;
    }

    if (dump_inst->run_state) {
        dump_inst->run_state = TD_FALSE;
    }

    return;
}

static td_void get_data_info(td_void *data, dump_data_type data_type, td_u32 *data_size, td_u8 **data_buf)
{
    if (data_type == DUMP_DATA_FRAME) {
        uapi_audio_frame *frame = (uapi_audio_frame *)data;
        *data_size = frame->bits_bytes;
        *data_buf = (td_u8 *)frame->bits_buffer;
    } else {
        uapi_stream_buf *stream = (uapi_stream_buf *)data;
        *data_size = stream->size;
        *data_buf = (td_u8 *)stream->data;
    }
}

static td_void dump_save_data(sap_test_dump_inst *dump_inst, td_void *data, dump_data_type data_type)
{
    td_s32 ret;
    td_u32 data_size;
    td_u32 write_size;
    td_u8 *data_buf = TD_NULL;

    get_data_info(data, data_type, &data_size, &data_buf);

    dump_inst->save_cur_size += data_size;

    if (dump_inst->save_cur_size <= dump_inst->save_limit_size) {
        switch (dump_inst->save_type) {
            case DUMP_SAVE_RAM:
                if (dump_inst->save_addr == TD_NULL) {
                    break;
                }
                ret = memcpy_s(dump_inst->save_addr + dump_inst->save_cur_size - data_size, data_size,
                    data_buf, data_size);
                if (ret != EOK) {
                    sap_err_log_fun(memcpy_s, ret);
                }
                break;

            case DUMP_SAVE_FILE:
                if (dump_inst->h_file == TD_NULL) {
                    break;
                }
                write_size = fwrite((td_void *)data_buf, 1, data_size, dump_inst->h_file);
                if (write_size != data_size) {
                    sap_printf("fwrite failed with write size = %u, except len = %u.\n", write_size, data_size);
                }
                break;

            default:
                sap_printf("wrong save_type = %d.\n", dump_inst->save_type);
                break;
        }
    }

    if ((dump_inst->save_cur_size >= dump_inst->save_limit_size) && (dump_inst->save_over == TD_FALSE)) {
        if (dump_inst->save_type == DUMP_SAVE_FILE) {
            fclose(dump_inst->h_file);
            dump_inst->h_file = TD_NULL;
        }

        dump_inst->save_over = TD_TRUE;
        sap_printf("file saved ok, file path: %s.\n", dump_inst->file_path);
    }
}

static td_void dump_adp_frame_proc(td_void *args)
{
    td_s32 ret;
    uapi_audio_frame frame;
    sap_test_dump_inst *dump_inst = (sap_test_dump_inst *)args;

    while (dump_inst->task_enable) {
        ret = uapi_adp_acquire_frame(dump_inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS); /*   */
            continue;
        }

        dump_save_data(dump_inst, &frame, DUMP_DATA_FRAME);

        ret = uapi_adp_release_frame(dump_inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }

    athread_set_exit(dump_inst->task, TD_TRUE);
}

static td_void dump_adp_stream_proc(td_void *args)
{
    td_s32 ret;
    uapi_stream_buf stream;
    sap_test_dump_inst *dump_inst = (sap_test_dump_inst *)args;

    while (dump_inst->task_enable) {
        ret = uapi_adp_acquire_stream(dump_inst->h_adp, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS); /*   */
            continue;
        }

        dump_save_data(dump_inst, &stream, DUMP_DATA_STREAM);

        ret = uapi_adp_release_stream(dump_inst->h_adp, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_stream, ret);
        }
    }

    athread_set_exit(dump_inst->task, TD_TRUE);
}

static athread_fun dump_get_thread_proc(dump_mod_type mod_type)
{
    switch (mod_type) {
        case DUMP_MOD_AENC:
            return dump_adp_stream_proc;

        default:
            return dump_adp_frame_proc;
    }
}

static td_s32 dump_open_inst(sap_test_dump_inst *dump_inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;
    athread_fun thread_func;
    athread_attr attr;

    if (dump_inst->save_type == DUMP_SAVE_RAM) {
        dump_inst->save_addr = (td_u8 *)DUMP_RAM_ADDR0 + dump_inst->save_offset;
        dump_inst->h_file = TD_NULL;
    } else {
        dump_inst->h_file = fopen(dump_inst->file_path, "wb");
        if (dump_inst->h_file == TD_NULL) {
            sap_printf("open %s failed", dump_inst->file_path);
            return EXT_FAILURE;
        }
    }

    /* Create ADP instance for mod instance output */
    uapi_adp_get_def_attr(&adp_attr);
    ret = uapi_adp_create(&dump_inst->h_adp, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        dump_close_inst(dump_inst);
        return EXT_FAILURE;
    }

    /* Attach ADP instance to mod instance */
    ret = dump_mod_attach(dump_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dump_mod_attach, ret);
        dump_close_inst(dump_inst);
        return EXT_FAILURE;
    }

    thread_func = dump_get_thread_proc(dump_inst->mod_type);

    /* Create SEA task for ADP instance to read pcm data */
    dump_inst->task_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "dump";
    ret = athread_create(&dump_inst->task, thread_func, (td_void *)dump_inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }
    sap_printf("file is saving.\n"
        "file_path = %s, file_size = %u kB, mod_name = %s, mod_handle = 0x%x, mod_output = %u.\n",
        dump_inst->file_path, (td_u32)(dump_inst->save_limit_size / BYTES_IN_1KB),
        get_mod_name(dump_inst->mod_type), dump_inst->h_mod, dump_inst->mod_out_type);

    dump_inst->run_state = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 sample_dump_exit(td_void)
{
    sap_test_dump_inst *dump_inst = TD_NULL;

    dump_inst = &g_test_dump_inst[1];
    if (dump_inst->to_be_used) {
        dump_close_inst(dump_inst);
    }

    dump_inst = &g_test_dump_inst[0];
    dump_close_inst(dump_inst);

    dump_sys_deinit();

    return EXT_SUCCESS;
}

static td_s32 sample_dump_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sap_test_dump_inst *dump_inst = TD_NULL;

    dump_inst = &(g_test_dump_inst[0]);
    if (dump_inst->run_state) {
        sample_dump_exit();
    }

    if (dump_entry_prase_param(argc, argv) != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = dump_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dump_sys_init, ret);
        return ret;
    }

    ret = dump_open_inst(dump_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dump_open_inst, ret);
        goto sys_deinit;
    }

    dump_inst = &(g_test_dump_inst[1]);
    if (dump_inst->to_be_used) {
        ret = dump_open_inst(dump_inst);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(dump_open_inst, ret);
            goto inst0_close;
        }
    }

    return EXT_SUCCESS;

inst0_close:
    dump_inst = &g_test_dump_inst[0];
    dump_close_inst(dump_inst);

sys_deinit:
    dump_sys_deinit();

    return EXT_FAILURE;
}

td_s32 sample_dump(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_dump_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "-h") == 0) {
        sample_dump_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_dump_exit();
    } else {
        return sample_dump_entry(argc, argv);
    }

    return EXT_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
