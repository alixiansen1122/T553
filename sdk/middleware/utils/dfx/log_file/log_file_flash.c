/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: offline log file saved to the storage
 */

#include "log_file.h"
#include "errcode.h"
#include "securec.h"
#include "stdbool.h"
#include "soc_osal.h"
#include "common_def.h"
#include "dfx_adapt_layer.h"
#include "debug_print.h"
#include "log_file_common.h"
#include "log_file_flash.h"

#define INVAID_INDEX_ID       0xFFFFFFFF

#if (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES)
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)

STATIC dfx_flash_op_type_t logfile_get_flash_op_type(store_service_t type)
{
    dfx_flash_op_type_t flash_op_type;
    switch (type) {
        case STORE_DIAG:
            flash_op_type = FLASH_OP_TYPE_LOG_FILE;
            break;
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
        case STORE_DOTTING:
            flash_op_type = FLASH_OP_TYPE_DOTTING;
            break;
#endif
        default:
            flash_op_type = FLASH_OP_TYPE_MAX;
            break;
    }
    return flash_op_type;
}

STATIC uint32_t logfile_check_record_head_null(uint8_t *record_data, uint32_t record_len, uint32_t start_pos)
{
    uint32_t i;
    for (i = 0; i < record_len; i++) {
        if (record_data[i] != 0xFF) {
            break;
        }
    }

    /* 如当前位置数据为全FF, 直接返回记录头长度 */
    if (i == record_len) {
        return record_len;
    }

    uint32_t remain_len = FLASH_SECTOR_SIZE - (start_pos % FLASH_SECTOR_SIZE);

    /* 剩余长度大于记录头长度，即当前位置不在flash页的结尾部分，则直接返回0 */
    if (remain_len > record_len) {
        return 0;
    }

    /* 当前位置在flash页的结尾部分，则计算从当前位置至flash页的结尾是否全FF，如果是则返回长度，否则返回0 */
    for (i = 0; i < remain_len; i++) {
        if (record_data[i] != 0xFF) {
            break;
        }
    }

    if (i == remain_len) {
        return remain_len;
    }
    return 0;
}

STATIC void logfile_read_data_from_flash(store_file_info_t *file_info, uint8_t *record_info,
    uint32_t read_len, uint32_t start_addr)
{
    if (start_addr > file_info->file_cfg.file_size) {
        return;
    }

    (void)memset_s(record_info, read_len, 0, read_len);
    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    if (start_addr + read_len > file_info->file_cfg.file_size) {
        uint32_t tmp_len = file_info->file_cfg.file_size - start_addr;
        dfx_flash_read(flash_op_type, start_addr, record_info, tmp_len);
        dfx_flash_read(flash_op_type, RECORD_START_ADDRESS, record_info + tmp_len, (read_len - tmp_len));
    } else {
        dfx_flash_read(flash_op_type, start_addr, record_info, read_len);
    }
}

STATIC errcode_t flash_erase_older_records(store_file_info_t *file_info, uint32_t data_len)
{
    uint32_t space_left = file_info->flash_cur_pos % FLASH_SECTOR_SIZE == 0 ? 0 :
        FLASH_SECTOR_SIZE - file_info->flash_cur_pos % FLASH_SECTOR_SIZE;
    uint32_t next_sector_pos =  file_info->flash_cur_pos + space_left;
    uint32_t sector_remain_len;

    if (file_info->file_cfg.no_record_head == 0) {
        sector_remain_len = MAX_SECTOR_REMAIN_LEN;
    } else {
        sector_remain_len = file_info->file_cfg.record_max_size;
    }

    /* 提前MAX_SECTOR_REMAIN_LEN个字节，擦除下一个sector */
    while (space_left < (data_len + sector_remain_len)) {
        /* 获取下一个sector的位置，如果当前是最后一个，则翻转到前面 */
        if (next_sector_pos == file_info->file_cfg.file_size) {
            /* 简单模式下，翻转到第二个sector，否则翻转到第一个sector */
#if ((defined(CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE)) && (CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE == DFX_YES))
            next_sector_pos = FLASH_SECTOR_SIZE;
#else
            next_sector_pos = 0;
#endif
        }

        if (file_info->pos_erased != next_sector_pos) {
            /* 如果当前sector已经擦除，不再重复擦除 */
            dfx_log_debug("[logfile] erase old sector... 0x%x\r\n", next_sector_pos);
            dfx_flash_erase(logfile_get_flash_op_type(file_info->type), next_sector_pos, FLASH_SECTOR_SIZE);
            file_info->pos_erased = next_sector_pos;
        }

        next_sector_pos += FLASH_SECTOR_SIZE;
        space_left += FLASH_SECTOR_SIZE;
    }

    return ERRCODE_SUCC;
}

#if ((defined(CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE)) && (CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE == DFX_YES))
STATIC errcode_t flash_update_cur_pos(store_file_info_t *file_info, bool wrap)
{
    uint32_t cur_pos = file_info->flash_cur_pos;
    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);

    if (wrap) {
        /* 如果数据已翻转，将文件头中的first_record_pos清0 */
        file_info->file_head.first_record_pos = 0;
    }

    if (file_info->cur_pos_addr >= (FLASH_SECTOR_SIZE - sizeof(uint32_t))) {
        /* 位置区已写满，先擦除再写入 */
        uint32_t magic = OFFSET_REGION_MAGIC;

        file_info->cur_pos_addr = sizeof(store_file_head_t);

        dfx_flash_erase(flash_op_type, 0, FLASH_SECTOR_SIZE);
        /* 刷新文件头中的时间戳 */
        file_info->file_head.update_time = dfx_get_cur_second();
        dfx_flash_write(flash_op_type, 0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t), 0);

        /* 保存当前位置，到第一个sector开始（file_head之后） */
        dfx_flash_write(flash_op_type, file_info->cur_pos_addr, (uint8_t *)&cur_pos, sizeof(uint32_t), 0);
        /* 在最后4个字节，写入魔术字，表示该区域已经恢复完成 */
        dfx_flash_write(flash_op_type, FLASH_SECTOR_SIZE - sizeof(uint32_t), (uint8_t *)&magic, sizeof(uint32_t), 0);
    } else {
        /* 保存当前位置，追加到第一个sector的最后 */
        dfx_flash_write(flash_op_type, file_info->cur_pos_addr, (uint8_t *)&cur_pos, sizeof(uint32_t), 0);
        if (wrap) {
            /* 翻转的情况下，刷新文件头中的first_record_pos */
            uint32_t offset = offsetof(store_file_head_t, first_record_pos);
            dfx_flash_write(flash_op_type, offset, (uint8_t *)&(file_info->file_head.first_record_pos),
                sizeof(uint32_t), 0);
        }
    }

    dfx_log_debug("[logfile] flash_update_cur_pos: cur_pos_addr = 0x%x cur_pos = 0x%x\r\n",
        file_info->cur_pos_addr, file_info->flash_cur_pos);

    file_info->cur_pos_addr += sizeof(uint32_t);
    return ERRCODE_SUCC;
}

STATIC void logfile_rebuild_file_head_sector(store_file_info_t *file_info, bool refresh_address, bool wrap)
{
    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    uint32_t magic_w = OFFSET_REGION_MAGIC;

    logfile_init_file_head(file_info);
    if (wrap) {
        /* 如果数据已翻转，将文件头中的first_record_pos清0 */
        file_info->file_head.first_record_pos = 0;
    }
    dfx_flash_erase(flash_op_type, 0, FLASH_SECTOR_SIZE);
    dfx_flash_write(flash_op_type, 0, (const uint8_t *)&file_info->file_head, sizeof(store_file_head_t), 0);

    if (refresh_address) {
        dfx_flash_write(flash_op_type, file_info->cur_pos_addr, (uint8_t *)&(file_info->flash_cur_pos),
            sizeof(uint32_t), 0);
        file_info->cur_pos_addr += sizeof(uint32_t);
    }

    dfx_flash_write(flash_op_type, FLASH_SECTOR_SIZE - sizeof(uint32_t), (uint8_t *)&magic_w, sizeof(uint32_t), 0);

    if (!refresh_address) {
        /* 第一次启动，事先擦除第二个sector */
        dfx_flash_erase(flash_op_type, FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
    }
}

STATIC void logfile_get_cur_pos_by_traversal(store_file_info_t *file_info)
{
    uint8_t record_info[RECORD_MAX_LEN] = {0};
    uint32_t i;
    bool found_null = false;
    bool found_warp = false;
    uint32_t read_len = 0;

    if (file_info->file_cfg.no_record_head == 0) {
        read_len = MAX_SECTOR_REMAIN_LEN;
    } else {
        read_len = file_info->file_cfg.record_max_size;
    }

    file_info->flash_cur_pos = RECORD_START_ADDRESS;

    /* 遍历flash区域，找到最新的写入地址 */
    for (i = (uint32_t)RECORD_START_ADDRESS; i < file_info->file_cfg.file_size;) {
        /* 从头开始遍历，从flash读取数据 */
        logfile_read_data_from_flash(file_info, record_info, read_len, i);
        /* 检查当前位置是否为全FF */
        uint32_t null_num = logfile_check_record_head_null(record_info, read_len, i);
        if (null_num == read_len) {
            /* 第一次找到全部为FF的数据，记录当前位置 */
            if (!found_null) {
                file_info->flash_cur_pos = i;
                found_null = true;
            }
            /* 直接移至下一个sector查找 */
            i = ((i / FLASH_SECTOR_SIZE) + 1) * FLASH_SECTOR_SIZE;
            continue;
        }

        /* 检查当前位置是否是一个有效的数据头 */
        uint32_t record_len = logfile_check_record_valid(file_info, record_info, read_len);
        if (record_len == 0) {
            i++; /* 如果不是有效记录头，移至下一个字节 */
            continue;
        }

        if (found_null) {
            /* 找到全FF数据之后再次找到有效数据头，说明数据已翻转，结束查找 */
            found_warp = true;
            break;
        }

        i += record_len;
    }

    dfx_log_info("[logfile] traversal flash_cur_pos = 0x%x. found_null = %d warp = %d\r\n",
        file_info->flash_cur_pos, found_null, found_warp);

    file_info->cur_pos_addr = sizeof(store_file_head_t);
    logfile_rebuild_file_head_sector(file_info, found_null, found_warp);
}

errcode_t logfile_flash_prepare(store_file_info_t *file_info)
{
    uint32_t magic = 0;
    uint32_t i;
    uint32_t cur_pos = FLASH_SECTOR_SIZE;
    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);

    dfx_log_info("[logfile] flash prepare ...\r\n");

    /* 读取文件头 */
    dfx_flash_read(flash_op_type, 0, (uint8_t *)&file_info->file_head, sizeof(store_file_head_t));

    /* 读取魔术字，判断该区域是否已经恢复完成 */
    dfx_flash_read(flash_op_type, FLASH_SECTOR_SIZE - sizeof(uint32_t), (uint8_t *)&magic, sizeof(uint32_t));

    /* 如果文件头魔术字错误，则认为是第一次启动logfile，初始化文件头 */
    if (file_info->file_head.start_flag != FILE_HEAD_START_FLAG) {
        dfx_log_info("[logfile] First boot ...\r\n");
        logfile_rebuild_file_head_sector(file_info, false, false);
        file_info->flash_cur_pos = FLASH_SECTOR_SIZE;
        file_info->cur_pos_addr = sizeof(store_file_head_t);
        return ERRCODE_SUCC;
    }

    if (magic != OFFSET_REGION_MAGIC) {
        dfx_log_info("[logfile] End Magic failed ...\r\n");
        /* 如果魔术字不匹配，则表示出现了在恢复过程中异常复位的情况，遍历所有记录查找flash_cur_pos */
        logfile_get_cur_pos_by_traversal(file_info);
    } else {
        /* 遍历 第一个sector，找到最新的写入地址 */
        for (i = sizeof(store_file_head_t); i < FLASH_SECTOR_SIZE - sizeof(uint32_t); i += sizeof(uint32_t)) {
            uint32_t tmp_pos = 0;
            dfx_flash_read(flash_op_type, i, (uint8_t *)&tmp_pos, sizeof(uint32_t));
            if (tmp_pos != INVAID_INDEX_ID) {
                cur_pos = tmp_pos;
            } else {
                break;
            }
        }

        file_info->cur_pos_addr = i;
        file_info->flash_cur_pos = cur_pos;

        dfx_log_info("[logfile] Found flash_cur_pos = 0x%x cur_pos_addr = 0x%x.\r\n",
            file_info->flash_cur_pos, file_info->cur_pos_addr);
    }
    return ERRCODE_SUCC;
}

#else /* CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE */

STATIC uint32_t get_circled_cur_pos(store_file_info_t *file_info, uint32_t pos_in)
{
    uint32_t pos_out;

    if (pos_in < file_info->file_cfg.file_size) {
        pos_out = pos_in;
    } else {
        pos_out = pos_in - file_info->file_cfg.file_size + (uint32_t)sizeof(store_file_head_t);
    }
    return pos_out;
}

STATIC bool logfile_is_index_continuous(uint32_t last_index, uint32_t cur_index)
{
    /*
     * 比较当前记录与上一条记录的index，下列三种情况说明index是连续的：
     * 1、当前记录是遍历的第一条记录
     * 2、当前记录index上一条记录大1
     * 3、当前记录的index发生翻转（当前是0，上一条是65535）
    */
    if ((last_index == 0) ||
        (cur_index == last_index + 1) ||
        (cur_index == 0 && last_index == MAX_INDEX_NUM)) {
        return true;
    }
    return false;
}

STATIC bool logfile_flash_without_head(store_file_info_t *file_info)
{
    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    dfx_flash_read(flash_op_type, 0, (uint8_t *)&file_info->file_head, sizeof(store_file_head_t));

    if (file_info->file_head.start_flag != FILE_HEAD_START_FLAG) {
        logfile_init_file_head(file_info);
        dfx_flash_erase(flash_op_type, 0, FLASH_SECTOR_SIZE);
        dfx_flash_write(flash_op_type, 0, (const uint8_t *)&file_info->file_head, sizeof(store_file_head_t), 0);
        file_info->index = 0;
        file_info->flash_cur_pos = (uint32_t)sizeof(store_file_head_t);
        return true;
    }

    return false;
}

errcode_t logfile_flash_prepare(store_file_info_t *file_info)
{
    store_record_info_t record_info = { 0 };
    uint32_t first_jump_pos = 0;
    uint32_t first_jump_index = INVAID_INDEX_ID;
    uint32_t last_index = 0;
    uint32_t i;

    /* 如果flash中没有有效的文件头，即第一次打开logfile，直接返回 */
    if (logfile_flash_without_head(file_info)) {
        return ERRCODE_SUCC;
    }

    file_info->index = INVAID_INDEX_ID;

    /* 遍历flash区域，找到最新的写入地址 */
    for (i = (uint32_t)sizeof(store_file_head_t); i < file_info->file_cfg.file_size;) {
        /* 从头开始遍历，从flash读取数据 */
        logfile_read_data_from_flash(file_info, (uint8_t *)&record_info, sizeof(store_record_info_t), i);

        /* 检查当前位置是否为全FF */
        uint32_t null_num = logfile_check_record_head_null((uint8_t *)&record_info, sizeof(store_record_info_t), i);
        if (null_num == sizeof(store_record_info_t)) {
            /* 找到记录头全部为FF的数据，结束遍历，此处即为最新记录的位置 */
            file_info->index = (last_index == 0) ? 0 : (last_index + 1);
            file_info->flash_cur_pos = i;
            break;
        } else if (null_num > 0 && null_num < sizeof(store_record_info_t)) {
            /* 找到页的结尾处有FF，但长度不够记录头的长度，记录下当前位置，继续查找 */
            file_info->index = (last_index == 0) ? 0 : (last_index + 1);
            file_info->flash_cur_pos = i;
            i += null_num;
            continue;
        }

        /* 检查当前位置是否是一个有效的数据头 */
        if (logfile_check_record_valid(file_info, (uint8_t *)&record_info, sizeof(store_record_info_t)) == 0) {
            i++; /* 如果不是有效记录头，移至下一个字节 */
            continue;
        }

        if (!logfile_is_index_continuous(last_index, record_info.index)) {
            if (file_info->index != INVAID_INDEX_ID) {
                /* 在flash页结尾找到FF的情况下，找到index不连续的情况，结束遍历，此处即为最新记录的位置 */
                break;
            } else if (first_jump_index == INVAID_INDEX_ID) {
                /* 在未找到全FF数据的情况下，记录第一个index不连续的位置 */
                first_jump_index = last_index;
                first_jump_pos = i;
            }
        }

        last_index = record_info.index;
        i += record_info.len;
    }

    if ((file_info->index == INVAID_INDEX_ID) && (first_jump_index != INVAID_INDEX_ID)) {
        /* 未找到全FF，以第一次index不连续位置为准 */
        file_info->index = (first_jump_index == 0) ? 0 : (first_jump_index + 1);
        file_info->flash_cur_pos = first_jump_pos;
    } else if ((file_info->index == INVAID_INDEX_ID) && (first_jump_index == INVAID_INDEX_ID)) {
        /* 未找到全FF，也未找到index不连续，以遍历的最后位置为准 */
        file_info->index = (last_index == 0) ? 0 : (last_index + 1);
        file_info->flash_cur_pos = get_circled_cur_pos(file_info, i);
    }

    dfx_log_info("[logfile] Found cur pos = 0x%x last index = 0x%x\r\n", file_info->flash_cur_pos, file_info->index);
    return ERRCODE_SUCC;
}

#endif /* CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE */

STATIC errcode_t write_cache_to_flash(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    uint32_t flash_left = file_info->file_cfg.file_size - file_info->flash_cur_pos;
    flash_erase_older_records(file_info, data_len);

    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    if (data_len > flash_left) {
        uint32_t data_left_len = data_len - flash_left;
#if ((defined(CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE)) && (CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE == DFX_YES))
        /* 在简单模式下， 第二段数据写入第二个sector */
        dfx_flash_write(flash_op_type, file_info->flash_cur_pos, data, flash_left, 0);
        dfx_flash_write(flash_op_type, FLASH_SECTOR_SIZE, data + flash_left, data_left_len, 0);
        file_info->flash_cur_pos = FLASH_SECTOR_SIZE + data_left_len;

        /* 当前位置flash_cur_pos保存到到第一个sector中 */
        (void)flash_update_cur_pos(file_info, true);
#else
        /* 在非简单模式下， 第二段数据写入第一个sector中file_head之后 */
        dfx_flash_write(flash_op_type, file_info->flash_cur_pos, data, flash_left, 0);
        dfx_flash_write(flash_op_type, 0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t), 0);
        dfx_flash_write(flash_op_type, sizeof(store_file_head_t), data + flash_left, data_left_len, 0);
        file_info->flash_cur_pos = (uint32_t)sizeof(store_file_head_t) + data_left_len;
#endif
    } else {
        dfx_flash_write(flash_op_type, file_info->flash_cur_pos, data, data_len, 0);
        file_info->flash_cur_pos += data_len;
#if ((defined(CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE)) && (CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE == DFX_YES))
        /* 当前位置到第一个sector */
        (void)flash_update_cur_pos(file_info, false);
#endif
    }

    return ERRCODE_SUCC;
}

errcode_t logfile_write_cache_to_flash(store_file_info_t *file_info)
{
    store_cache_t *cache = file_info->cache;
    uint8_t *read_data;
    int32_t read_len;
    uint32_t tmp_pos = 0;

    /* 读取 cache_write_pos 的瞬时值 */
    tmp_pos = cache->cache_write_pos;

    /* cache中没有新数据，直接退出 */
    if (tmp_pos == cache->cache_read_pos) {
        return ERRCODE_SUCC;
    }

    read_data = (uint8_t *)cache->data + cache->cache_read_pos;
    osal_mutex *file_write_mutex = &(logfile_get_manage()->file_write_mutex);
    osal_mutex_lock(file_write_mutex);
    if (tmp_pos > cache->cache_read_pos) {
        read_len = (int32_t)(tmp_pos - cache->cache_read_pos);
        write_cache_to_flash(file_info, read_data, (uint32_t)read_len);
    } else {
        read_len = (int32_t)(cache->cache_size - cache->cache_read_pos);
        write_cache_to_flash(file_info, read_data, (uint32_t)read_len);
        write_cache_to_flash(file_info, (uint8_t *)cache->data, tmp_pos);
    }

    cache->cache_read_pos = tmp_pos;
    osal_mutex_unlock(file_write_mutex);
    return ERRCODE_SUCC;
}

errcode_t logfile_flash_erase(store_service_t service_type, const store_file_cfg_t *cfg)
{
    return dfx_flash_erase(logfile_get_flash_op_type(service_type), 0, cfg->file_size);
}

#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
#endif /* CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE */