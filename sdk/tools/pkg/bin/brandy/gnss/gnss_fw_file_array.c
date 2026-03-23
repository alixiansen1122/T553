/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:  gnss firmware file arrays.
 *
 * History:
 * 2024-07-20, Create file.
 */
#include "gnss_fw_file_array.h"

#define GNSS_FW_ATTR

#define GNSS_CFG_BIN  "gnss_cfg.bin"
GNSS_FW_ATTR const char g_gnss_cfg_file[] = {
#include GNSS_CFG_BIN
};

const tiot_file g_gnss_fw_files[] = {
    { "cfg", sizeof(g_gnss_cfg_file), g_gnss_cfg_file },
};

const tiot_file_path g_gnss_fw_file_path = {
    g_gnss_fw_files, sizeof(g_gnss_fw_files) / sizeof(tiot_file)
};

const tiot_file_path *gnss_fw_file_path_get(void)
{
    return &g_gnss_fw_file_path;
}
