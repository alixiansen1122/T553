/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : hardware capability
 * Author: @CompanyNameTag
 * Create: 2021-11-13
 */
#ifndef HDI_HARDWARE_CAPABILITY_H
#define HDI_HARDWARE_CAPABILITY_H

#include <stdbool.h>
#include <VG/openvg.h>

#define HARDWARE_MAX_SCISSOR_RECTS          1
#define HARDWARE_MAX_DASH_COUNT             2
#define HARDWARE_MAX_GAUSSIAN_STD_DEVIATION 32
#define HARDWARE_MAX_COLOR_RAMP_STOPS       7
#define HARDWARE_MAX_IMAGE_WIDTH            640
#define HARDWARE_MAX_IMAGE_HEIGHT           640
#define HARDWARE_MAX_IMAGE_PIXELS           (HARDWARE_MAX_IMAGE_WIDTH * HARDWARE_MAX_IMAGE_HEIGHT)
#define HARDWARE_MAX_IMAGE_BYTES            (HARDWARE_MAX_IMAGE_PIXELS * 4)
#define HARDWARE_MAX_FLOAT                  (3.4027717462407993e+38f) // float24: 0x7f7fff00

#define HARDWARE_PATH_CAPABILITY            (VG_PATH_CAPABILITY_APPEND_FROM | VG_PATH_CAPABILITY_APPEND_TO | \
    VG_PATH_CAPABILITY_MODIFY | VG_PATH_CAPABILITY_TRANSFORM_FROM | VG_PATH_CAPABILITY_TRANSFORM_TO)

bool is_data_type_supported(VGPathDatatype type);

#endif /* HDI_HARDWARE_CAPABILITY_H */
