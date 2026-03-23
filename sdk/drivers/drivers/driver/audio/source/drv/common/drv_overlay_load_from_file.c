/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: header file for audio input
 * Author: audio
 */

#include "securec.h"
#include "mem_addr.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_audio_osal.h"
#include "drv_overlay_load.h"
#include "core_overlay_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_PATH_LENGTH 64

static const td_char *g_dsp_main_file = "dsp_main.bin";
static const td_char *g_dsp_lib_file = "dsp_overlay.bin";
static td_char g_dsp_file_path[MAX_PATH_LENGTH] = {0};

td_s32 overlay_set_dsp_file_path(const char *file_path, td_u32 len)
{
    td_s32 ret;
    td_u32 dsp_main_file_length = strlen(g_dsp_main_file);
    td_u32 dsp_lib_file_length = strlen(g_dsp_lib_file);
    td_u32 max_file_length = (dsp_main_file_length > dsp_lib_file_length) ? dsp_main_file_length : dsp_lib_file_length;

    if (len + max_file_length >= MAX_PATH_LENGTH) {
        sap_err_log_u32(len);
        sap_err_log_info("file path is too long");
        return ERR_SYS_MEM_OVERLOAD;
    }

    ret = memcpy_s(g_dsp_file_path, MAX_PATH_LENGTH - 1, file_path, len);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ERR_SYS_MEM_COPY;
    }

    return AUDIO_SUCCESS;
}

static td_s32 overlay_load_elf(audio_core_id core_id, td_void *flip)
{
    td_u32 i, sec_cnt;
    td_bool tcm_flag;
    td_u32 phys_addr;
    td_u32 *virt_addr = TD_NULL;
    td_char *sec_buf = TD_NULL;
    struct {
        td_u32 addr;
        td_u32 size;
    } sec_hdr;

    if (drv_audio_osal_klib_fread((td_char *)&sec_cnt, sizeof(sec_cnt), flip) != sizeof(sec_cnt)) {
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < sec_cnt; i++) {
        if (drv_audio_osal_klib_fread((td_char *)&sec_hdr, sizeof(sec_hdr), flip) != sizeof(sec_hdr)) {
            return ERR_SYS_NOT_AVAILABLE;
        }

        if (is_4byte_aligned(sec_hdr.addr) != TD_TRUE || is_4byte_aligned(sec_hdr.size) != TD_TRUE) {
            audio_log_err("addr=%#x/size=%#x, not aligned to 4 bytes", sec_hdr.addr, sec_hdr.size);
            return ERR_SYS_VERIFY_FAIL;
        }

        phys_addr = drv_overlay_map_addr(core_id, sec_hdr.addr, &tcm_flag);
        virt_addr = audio_osal_address_map((td_ulong)phys_addr, sec_hdr.size);
        if (virt_addr == TD_NULL) {
            return ERR_SYS_MEM_EXCEPTION;
        }

        if (tcm_flag) {
            /* Load IRAM/DRAM with 32bit operation */
            sec_buf = audio_osal_malloc_aligned(sec_hdr.size, sizeof(td_u32));
            if (sec_buf == TD_NULL) {
                audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
                return ERR_SYS_MEM_ALLOC;
            }
            if (drv_audio_osal_klib_fread((td_char *)sec_buf, sec_hdr.size, flip) != sec_hdr.size) {
                audio_osal_free_aligned(sec_buf);
                audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
                return ERR_SYS_NOT_AVAILABLE;
            }
            drv_overlay_copy_section(virt_addr, (const td_u32 *)sec_buf, sec_hdr.size);
            audio_osal_free_aligned(sec_buf);
        } else {
            if (drv_audio_osal_klib_fread((td_char *)virt_addr, sec_hdr.size, flip) != sec_hdr.size) {
                audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
                return ERR_SYS_NOT_AVAILABLE;
            }
        }

        audio_osal_dcache_region_wb((td_ulong)phys_addr, virt_addr, sec_hdr.size);
        audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
    }

    return AUDIO_SUCCESS;
}

static td_s32 overlay_get_file_from_flash(const char *file, td_u32 flash_addr, td_u32 flash_len)
{
#ifdef SAP_FLASH_FIRMWARE_SUPPORT
    td_void *flip = TD_NULL;

    audio_log_warn("save %#x %#x to %s\n", flash_addr, flash_len, file);
    flip = drv_audio_osal_klib_fopen(file, DRV_AUDIO_OSAL_O_RDWR | DRV_AUDIO_OSAL_O_CREAT, 0777);  /* 0777: priv mode */
    if (flip == TD_NULL) {
        audio_log_err("open %s failed\n", file);
        return ERR_SYS_OPEN_FAILED;
    }

    if (drv_audio_osal_klib_fwrite((td_char *)flash_addr, flash_len, flip) != flash_len) {
        audio_log_err("Write %s failed\n", file);
        return ERR_SYS_MEM_COPY;
    }

    audio_log_warn("Save %s Success.\n", file);
    drv_audio_osal_klib_fclose(flip);
#endif
    audio_unused(file);
    audio_unused(flash_addr);
    audio_unused(flash_len);
    return AUDIO_SUCCESS;
}

static td_void *overlay_open_file(const char *file, td_u32 flash_addr, td_u32 flash_len)
{
    td_s32 ret;
    td_void *flip = TD_NULL;
    td_char full_path[MAX_PATH_LENGTH] = {0};

    ret = snprintf_s(full_path, MAX_PATH_LENGTH - 1, MAX_PATH_LENGTH - 1, "%s%s", g_dsp_file_path, file);
    if (ret < 0) {
        sap_err_log_s32(ret);
        return ERR_SYS_MEM_COPY;
    }

    flip = drv_audio_osal_klib_fopen(full_path, DRV_AUDIO_OSAL_O_RDONLY, 0);
    if (flip == TD_NULL) {
        ret = overlay_get_file_from_flash(full_path, flash_addr, flash_len);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(overlay_get_file_from_flash, ret);
            return TD_NULL;
        }

        flip = drv_audio_osal_klib_fopen(full_path, DRV_AUDIO_OSAL_O_RDONLY, 0);
        if (flip == TD_NULL) {
            sap_err_log_fun(drv_audio_osal_klib_fopen, ERR_SYS_OPEN_FAILED);
            return TD_NULL;
        }
    }

    return flip;
}

static td_s32 overlay_update_section_list(audio_core_id core_id, td_void *flip)
{
    td_u32 i;
    td_u32 sec_cnt;
    td_bool tcm_flag;
    struct {
        td_u32 addr;
        td_u32 size;
    } sec_hdr;

    drv_overlay_clear_section_list();

    drv_audio_osal_klib_fseek(0, SEEK_SET, flip);
    if (drv_audio_osal_klib_fread((td_char *)&sec_cnt, sizeof(sec_cnt), flip) != sizeof(sec_cnt)) {
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < sec_cnt; i++) {
        if (drv_audio_osal_klib_fread((td_char *)&sec_hdr, sizeof(sec_hdr), flip) != sizeof(sec_hdr)) {
            return ERR_SYS_NOT_AVAILABLE;
        }

        (td_void)drv_overlay_map_addr(core_id, sec_hdr.addr, &tcm_flag);
        drv_overlay_add_section_item(core_id, sec_hdr.addr, sec_hdr.size);

        drv_audio_osal_klib_fseek(sec_hdr.size, SEEK_CUR, flip);
    }

    return AUDIO_SUCCESS;
}

static td_s32 overlay_unpack_lib(const td_char *lib_name, td_void *flip, td_u32 *lib_size)
{
    td_u32 i, sec_cnt = 0;
    struct {
        td_char name[OVERLAY_NAME_LEN];
        td_u32 size;
    } sec_hdr;

    if (drv_audio_osal_klib_fread((td_char *)&sec_cnt, sizeof(td_u32), flip) != sizeof(td_u32)) {
        sap_err_log_fun(drv_audio_osal_klib_fread, AUDIO_FAILURE);
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < sec_cnt; i++) {
        if (drv_audio_osal_klib_fread((td_char *)&sec_hdr, sizeof(sec_hdr), flip) != sizeof(sec_hdr)) {
            sap_err_log_fun(drv_audio_osal_klib_fread, AUDIO_FAILURE);
            return ERR_SYS_NOT_AVAILABLE;
        }

        sap_trace_log_str(sec_hdr.name);
        if (audio_osal_strncmp(sec_hdr.name, sizeof(sec_hdr.name), lib_name, sizeof(sec_hdr.name)) == 0) {
            sap_warn_log_str(sec_hdr.name);
            if (lib_size == TD_NULL) {
                return ERR_SYS_NULL_PTR;
            }
            *lib_size = sec_hdr.size;
            return AUDIO_SUCCESS;
        }

        if (drv_audio_osal_klib_fseek(sec_hdr.size, OSAL_SEEK_CUR, flip) < 0) {
            sap_err_log_fun(drv_audio_osal_klib_fseek, AUDIO_FAILURE);
            return ERR_SYS_NOT_AVAILABLE;
        }
    }

    return ERR_SYS_NOT_SUPPORTED;
}

td_s32 drv_overlay_get_lib(const td_char *lib_name, td_u8 *lib_buf, td_u32 *lib_buf_size)
{
    td_s32 ret;
    td_u32 lib_size;
    td_u8 *lib = TD_NULL;
    td_void *flip = TD_NULL;

    sys_check_null_ptr_return(lib_name);
    sys_check_null_ptr_return(lib_buf);
    sys_check_null_ptr_return(lib_buf_size);

    flip = overlay_open_file(g_dsp_lib_file, FLASH_DSP_OVERLAY_ADDR, FLASH_DSP_OVERLAY_SIZE);
    if (flip == TD_NULL) {
        sap_err_log_fun(overlay_open_file, flip);
        return ERR_SYS_OPEN_FAILED;
    }

    ret = overlay_unpack_lib(lib_name, flip, &lib_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_unpack_lib, ret);
        goto out0;
    }

    if (lib_size > *lib_buf_size) { /* input buf size check */
        sap_err_log_str(lib_size);
        ret = ERR_SYS_BUF_OVERFLOW;
        goto out0;
    }

    if (drv_audio_osal_klib_fread((td_char *)lib_buf, lib_size, flip) != lib_size) {
        ret = ERR_SYS_NOT_AVAILABLE;
        goto out0;
    }

    *lib_buf_size = lib_size; /* output buf actual data size */

out0:
    drv_audio_osal_klib_fclose(flip);
    return ret;
}

static td_s32 drv_overlay_clear_section(audio_lib_id type)
{
    td_s32 ret;

    if (type == AUDIO_LIB_DSP0_APP || type == AUDIO_LIB_DSP1_APP) {
        return AUDIO_SUCCESS;
    }

    ret = drv_msg_sys_clear_overlay_section(type);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sys_clear_overlay_section, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_load_main_program(audio_core_id core_id)
{
    td_s32 ret;
    td_void *flip = TD_NULL;

    flip = overlay_open_file(g_dsp_main_file, FLASH_DSP_MAIN_ADDR, FLASH_DSP_MAIN_SIZE);
    if (flip == TD_NULL) {
        return ERR_SYS_OPEN_FAILED;
    }

    ret = overlay_load_elf(core_id, flip);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_load_elf, ret);
        drv_audio_osal_klib_fclose(flip);
        return ret;
    }

    overlay_update_section_list(core_id, flip);
    drv_audio_osal_klib_fclose(flip);

    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_load_lib_program(audio_core_id core_id, audio_lib_id type, td_u32 index)
{
    td_s32 ret;
    td_u32 lib_size;
    const td_char *lib_name = TD_NULL;
    td_void *flip = TD_NULL;

    if (type >= AUDIO_LIB_MAX) {
        sap_err_log_s32(type);
        return ERR_SYS_INVALID_PARAM;
    }

    lib_name = drv_overlay_find_and_reset_lib(core_id, type, index);
    if (lib_name == TD_NULL) {
        sap_err_log_fun(drv_overlay_find_and_reset_lib, lib_name);
        return ERR_SYS_NOT_SUPPORTED;
    }

    flip = overlay_open_file(g_dsp_lib_file, FLASH_DSP_OVERLAY_ADDR, FLASH_DSP_OVERLAY_SIZE);
    if (flip == TD_NULL) {
        sap_err_log_fun(overlay_open_file, flip);
        return ERR_SYS_OPEN_FAILED;
    }

    ret = overlay_unpack_lib(lib_name, flip, &lib_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_unpack_lib, ret);
        drv_audio_osal_klib_fclose(flip);
        return ret;
    }

    ret = drv_overlay_clear_section(type);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_clear_section, ret);
        return ret;
    }

    ret = overlay_load_elf(core_id, flip);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_load_elf, ret);
        drv_audio_osal_klib_fclose(flip);
        return ret;
    }

    drv_audio_osal_klib_fclose(flip);
    audio_unused(core_id);
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
