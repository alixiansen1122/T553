/**
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved.
 *
 * Description:  caxx firmware file arrays.
 * Author: @CompanyNameTag
 * History:
 * 2025-07-22, Create file.
 */
#include "caxx_fw_file_array.h"

#define CAXX_FW_ATTR

#define CAXX_CFG_BIN  "caxx_cfg.bin"
CAXX_FW_ATTR const char g_caxx_cfg_file[] = {
#include CAXX_CFG_BIN
};

const tiot_file g_caxx_fw_files[] = {
    { "cfg", sizeof(g_caxx_cfg_file), g_caxx_cfg_file },
};

const tiot_file_path g_caxx_fw_file_path = {
    g_caxx_fw_files, sizeof(g_caxx_fw_files) / sizeof(tiot_file)
};

const tiot_file_path *caxx_fw_file_path_get(void)
{
    return &g_caxx_fw_file_path;
}
