/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : hardware capability
 * Author: @CompanyNameTag
 * Create: 2021-11-13
 */

#include "hdi_hardware_capability.h"

static const VGPathDatatype g_hardware_support_list_data_type[] = { VG_PATH_DATATYPE_F };

bool is_data_type_supported(VGPathDatatype type)
{
    for (VGuint i = 0; i < sizeof(g_hardware_support_list_data_type) /
         sizeof(g_hardware_support_list_data_type[0]); i++) {
        if (type == g_hardware_support_list_data_type[i]) {
            return true;
        }
    }
    return false;
}
