/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_HARDWARE_CONFIG_H
#define GRAPHIC_HARDWARE_CONFIG_H

#include "memory_config_common.h"
#ifdef CONFIG_PSRAM_SUPPORT
#include "psram.h"
#endif

/**
 * @brief Hardware acceleration for drawing path. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_PATH                   1
/**
 * @brief Hardware acceleration for drawing letter vector path. Prerequirement is that both ENABLE_VGU_ENGINE and
 *        CAPABILITY_HWDRAW_PATH is 1.
 */
#define CAPABILITY_HWDRAW_LETTER_VECTOR          1
/**
 * @brief Hardware acceleration for drawing alpha bitmap. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_ALPHA_BITMAP           1
/**
 * @brief 1 means outRect is supported.
 *        0 means outRect is not supported, and must be same as the buffer rect.
 */
#define CAPABILITY_HW_OUT_RECT                   1
/**
 * @brief Hardware acceleration for rotate and scale in one operation.
 */
#define CAPABILITY_HW_ROTATE_AND_SCALE           1
/**
 * @brief Enable JPEG preview which includes decoding jpeg file and cacheing decoded data.
 */
#ifdef SUPPORT_GPU_JPEG
#ifdef XTS_SUPPORT
#define CAPABILITY_HW_JPEG_DECODE                0
#else
#define CAPABILITY_HW_JPEG_DECODE                1
#endif
#else
#define CAPABILITY_HW_JPEG_DECODE                0
#endif
/**
 * @brief Hardware acceleration for gaussian blur.
 */
#define CAPABILITY_HW_GAUSSIAN_BLUR              1
/**
 * @brief ENABLE GMMU MEM.
 */
#ifdef SUPPORT_GPU_GMMU
#define ENABLE_GMMU                              1
#else
#define ENABLE_GMMU                              0
#endif
/**
 * @brief specify buffer mode when use single buffer.
 *        1 means compressed buffer.
 *        0 means uncompressed buffer.
 */
#define BUFFER_COMPRESS                          1
/**
 * @brief Some of hardware acceleration operations requires an aligned rect.
 *        If this prerequisite is not meet, it could cause extra data been handled.
 */
#define HW_RECT_ALIGN_RESTRICTION                0
/**
 * @brief Hardware acceleration for drawing ellipse. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_ELLIPSE         0
/**
 * @brief Hardware acceleration for drawing line. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_LINE            1
/**
 * @brief Hardware acceleration for drawing arc. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_ARC             1
/**
 * @brief Hardware acceleration for drawing cubic bezier curve. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_CURVE           1
/**
 * @brief Hardware acceleration for drawing rect. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_RECT           1
/**
 * @brief Hardware acceleration for filling small size. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_SMALL_SIZE    1
/**
 * @brief Whether open offline dial.
 */
#define USE_OFFLINE_DIAL           0

// if ENABLE_OPENVG is set to be 1, dislpay_vau.c should be compiled.
#define ENABLE_OPENVG 0
#if ENABLE_OPENVG
#define BLIT_COUNT_MAX 1
#else
#define BLIT_COUNT_MAX 20
#endif
#define STROKE_MAX_CNT 48

#define HARDWARE_ACC_SIZE_LIMIT (50 * 50)
/**
 * @brief Use double buffer for rendering process.
 */
 
#ifndef DOUBLE_BUFFER
#define DOUBLE_BUFFER                     1
#endif

#ifndef HW_ROTATE_ANGLE
#define HW_ROTATE_ANGLE 0
#endif

#if !defined(RESOLUTION_WIDTH) || !defined(RESOLUTION_HEIGHT)
#define RESOLUTION_WIDTH                  454
#define RESOLUTION_HEIGHT                 454
#endif

#ifdef MIPI_ULPS_SUPPORT
#define ENABLE_ULPS                       1
#else
#define ENABLE_ULPS                       0
#endif

#if defined(QSPI_DISPLAY)
#define LAYER_PF_CODE                     1
#define LAYER_BPP                         32
#elif defined(MEMORY_MINI)
#define LAYER_PF_CODE                     0
#define LAYER_BPP                         16
#else
#define LAYER_PF_CODE                     2
#define LAYER_BPP                         24
#endif

/**
 * @brief Memory pool define.
 */
#define ALIGN_UP(x, a) ((((x) + ((a) - 1)) / (a)) * (a))

#ifndef CONFIG_PSRAM_SUPPORT // equal to MEMORY_MINI
#define MEM_POOL_UI_POOL_SIZE  0x13c000 // 1408 - 144 = 1264k
#define MEM_POOL_UI_FONT       L2M_GPU_RAM_GPU_SHARE_ORIGIN // 0x60240000
#define MEM_POOL_UI_FONT_SIZE  0x12000

#define MEM_POOL_UI_FB         (MEM_POOL_UI_FONT + MEM_POOL_UI_FONT_SIZE)
#define MEM_POOL_UI_FB_SIZE    ((ALIGN_UP(RESOLUTION_WIDTH, 16) * RESOLUTION_HEIGHT * LAYER_BPP / 8) * 2)

#define MEM_POOL_UI_OTHER      (MEM_POOL_UI_FB + MEM_POOL_UI_FB_SIZE)
#define MEM_POOL_UI_OTHER_SIZE (MEM_POOL_UI_POOL_SIZE - MEM_POOL_UI_FONT_SIZE - MEM_POOL_UI_FB_SIZE)

#define DRAW_MAX_CNT 48

#else
#define MEM_POOL_UI_POOL_SIZE  PSRAM_GUI_MEM_POOL_SIZE
#define MEM_POOL_UI_FONT       PSRAM_GUI_MEM_POOL_ADDR
#define MEM_POOL_UI_FONT_SIZE  0x132000

#ifdef SUPPORT_GPU_GMMU // FB(sram) is virtual addr, FB0:0x61240000 FB1:0x61a40000
#define MEM_POOL_UI_OTHER      (MEM_POOL_UI_FONT + MEM_POOL_UI_FONT_SIZE)
#define MEM_POOL_UI_OTHER_SIZE (MEM_POOL_UI_POOL_SIZE - MEM_POOL_UI_FONT_SIZE)
#else
#ifdef QSPI_DISPLAY // FB(sram) is physical addr, FB0:0x60280000 FB1:0x602fe000
#define MEM_POOL_UI_FB         L2M_GPU_RAM_GPU_SHARE_ORIGIN // 0x60280000
#define MEM_POOL_UI_FB_SIZE    0xfc000 // L2M_GPU_RAM_GPU_SHARE_LENGTH 0x120000 - GPU node 144*1024K
#define MEM_POOL_UI_OTHER      (MEM_POOL_UI_FONT + MEM_POOL_UI_FONT_SIZE)
#define MEM_POOL_UI_OTHER_SIZE (MEM_POOL_UI_POOL_SIZE - MEM_POOL_UI_FONT_SIZE)
#else // FB(psram) is physical addr, GUI dynamic allocation
#define MEM_POOL_UI_FB         0x60240000
#define MEM_POOL_UI_FB_SIZE    0x151800 // max:480*3*480*2
#define MEM_POOL_UI_OTHER      (MEM_POOL_UI_FONT + MEM_POOL_UI_FONT_SIZE)
#define MEM_POOL_UI_OTHER_SIZE (MEM_POOL_UI_POOL_SIZE - MEM_POOL_UI_FONT_SIZE)
#endif
#endif

#define DRAW_MAX_CNT 640
#endif

#define CAPABILITY_HW_SUPPORT_EMOJI 1
#define CAPABILITY_SW_DFX_FRAME_TRACE 1
#define ENABLE_DYNAMIC_FRAME_RATE 1
#endif // GRAPHIC_HARDWARE_CONFIG_H
