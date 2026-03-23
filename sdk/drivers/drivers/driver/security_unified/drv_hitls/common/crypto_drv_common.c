/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#include "crypto_drv_common.h"

/* crypto dump functions. */
#define MAX_DUMP_LENGHT     (256)
#define BYTES_IN_ONE_LINE   (16)
#ifndef KERN_CONT
#define KERN_CONT
#endif
void crypto_dump_data(const char *name, const td_u8 *data, td_u32 data_len)
{
    td_u32 i;
    td_u32 dump_len = data_len;
    crypto_unused(name);
    crypto_unused(data);
    if (data == TD_NULL) {
        crypto_log_err("data is NULL\n");
        return;
    }

    crypto_print("%s:\n", name);
    dump_len = data_len > MAX_DUMP_LENGHT ? MAX_DUMP_LENGHT : data_len;
    for (i = 0; i < dump_len; i++) {
        crypto_print(KERN_CONT"%02x ", data[i]);
        if ((i + 1) % BYTES_IN_ONE_LINE == 0) {
            crypto_print("\n");
        }
    }
    if (dump_len % BYTES_IN_ONE_LINE != 0) {
        crypto_print("\n");
    }
}

void crypto_dump_phys_addr(const char *name, const td_u64 phys_addr, td_u32 data_len)
{
    td_u8 *buffer = crypto_ioremap_nocache(phys_addr, data_len);
    if (buffer == TD_NULL) {
        crypto_log_err("crypto_ioremap_nocache failed, buffer is NULL\n");
        return;
    }
    crypto_dump_data(name, buffer, data_len);
    crypto_iounmap(buffer, data_len);
    crypto_unused(phys_addr);
}

td_u32 crypto_get_value_by_index(const crypto_table_item *table, td_u32 table_size,
    td_u32 index, td_u32 *value)
{
    const crypto_table_item *item = TD_NULL;
    td_u32 i;

    crypto_chk_return(table == TD_NULL, TD_FAILURE, "table is NULL\n");
    crypto_chk_return(value == TD_NULL, TD_FAILURE, "value is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index == index) {
            *value = item->value;
            return TD_SUCCESS;
        }
    }
    crypto_log_err("Invalid Index!\n");
    return TD_FAILURE;
}

td_u32 crypto_get_index_by_value(const crypto_table_item *table, td_u32 table_size,
    td_u32 value, td_u32 *index)
{
    const crypto_table_item *item = TD_NULL;
    td_u32 i;

    crypto_chk_return(table == TD_NULL, TD_FAILURE, "table is NULL\n");
    crypto_chk_return(index == TD_NULL, TD_FAILURE, "index is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->value == value) {
            *index = item->index;
            return TD_SUCCESS;
        }
    }
    crypto_log_err("Invalid Value!\n");
    return TD_FAILURE;
}

void crypto_reg_item_dump(const td_char *name, const reg_item_t *reg_item_table, td_u32 item_size,
    reg_read_func read_func)
{
    td_u32 i;
    crypto_unused(name);
    crypto_unused(reg_item_table);
    crypto_unused(read_func);

    crypto_print("========== %s reg table:==========\n", name);
    for (i = 0; i < item_size; i++) {
        crypto_print("%s is 0x%x\n", reg_item_table[i].name, read_func(reg_item_table[i].offset));
    }
}