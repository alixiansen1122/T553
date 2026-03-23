/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_CONF_EXT_H
#define LV_CONF_EXT_H

#include "graphic_hardware_config.h"

#ifdef USE_DIAG_LOG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif

#ifdef SUPPORT_GPU_GMMU
#define ENABLE_GMMU                              1
#else
#define ENABLE_GMMU                              0
#endif

#if !defined(RESOLUTION_WIDTH) || !defined(RESOLUTION_HEIGHT)
#define RESOLUTION_WIDTH                  454
#define RESOLUTION_HEIGHT                 454
#endif

#if defined(QSPI_DISPLAY)
#define DEFAULT_PIX_FMT                   PIXEL_FMT_RGBA_8888
#define DEFAULT_PIX_BPP                   32
#define LV_COLOR_DEPTH                    32
#define LV_IMG_CF_DEFAULT                 LV_IMG_CF_RGBA8888
#define DEFAULT_VECTOR_FONT_FILENAME      "/user/res/HarmonyOS_Sans_SC_Regular.ttf"
#define LV_USE_DEMO_SCROLL_TEXT_CN        1
#define LV_HW_DRAW_PENDING_MAX_CNT        640
#elif defined(MEMORY_MINI)
#define DEFAULT_PIX_FMT                   PIXEL_FMT_RGB_565
#define DEFAULT_PIX_BPP                   16
#define LV_COLOR_DEPTH                    16
#define LV_IMG_CF_DEFAULT                 LV_IMG_CF_RGB565
#define DEFAULT_VECTOR_FONT_FILENAME      "/user/res/HarmonyOS_Sans_Regular.ttf"
#define LV_USE_DEMO_SCROLL_TEXT_EN        1
#define LV_HW_DRAW_PENDING_MAX_CNT        48
#else
#define DEFAULT_PIX_FMT                   PIXEL_FMT_RGB_888
#define DEFAULT_PIX_BPP                   24
#define LV_COLOR_DEPTH                    32
#define LV_IMG_CF_DEFAULT                 LV_IMG_CF_RGB888
#define DEFAULT_VECTOR_FONT_FILENAME      "/user/res/HarmonyOS_Sans_SC_Regular.ttf"
#define LV_USE_DEMO_SCROLL_TEXT_CN        1
#define LV_HW_DRAW_PENDING_MAX_CNT        640
#endif

#ifdef USE_DIAG_LOG
#define LV_LOGE(fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define LV_LOGW(fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define LV_LOGI(fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#else
#define LV_LOGE(fmt, arg...) printf(fmt, ##arg)
#define LV_LOGW(fmt, arg...) printf(fmt, ##arg)
#define LV_LOGI(fmt, arg...) printf(fmt, ##arg)
#endif

#define LV_CORE_TIMER_NO_PERIOD_LIMIT 1
#define LV_USE_DPU 1
#define LV_USE_LOCAL_REFRESH 1
#define LV_USE_VGU 1
#define LV_USE_HW_BLUR 1
#define LV_HW_DRAW_RECT 1
#define LV_BYTE_ALIGNMENT 16

#define LV_USE_DFX_CMD 1
#define LV_USE_DFX_TRACE 1
#define LV_USE_PERF_MONITOR_EXT 1
#define LV_USE_MUTEX 1
#define LV_USE_LOW_POWER_MANAGER 1
#define LV_USE_ASYNC_MODE 1

#define LV_USE_FONT_VECTOR 1
#if LV_USE_FONT_VECTOR
#define LV_USE_DEMO_WIDGETS_FONT_VECTOR 0
#endif

#define LV_USE_SNAPSHOT_EXT 1
#if LV_USE_SNAPSHOT_EXT
#define LV_USE_SNAPSHOT_EXT_REDRAW 0
#endif

#define LV_USE_IMG_EXT 1
#define LV_USE_TRANSFORM_EXT 1
#define LV_USE_IMG_CACHE_EXT 1
#define LV_USE_IMG_LRU_CACHE 1

#ifdef SUPPORT_GPU_JPEG
#define LV_USE_HW_JPEG_DECODER 1
#else
#define LV_USE_HW_JPEG_DECODER 0
#endif

#define LV_USE_CANVAS_EXT 1
#define LV_USR_CROSS_VIEW 1
#define LV_USE_SCR_LOAD_ANIM 1
#define LV_USE_KALEIDOSCOPE 1

#define LV_USE_SURFACE_VIEW 1
#if LV_USE_SURFACE_VIEW
#define SKIP_LINE 1
#define LV_USE_DEMO_SURFACE 0
#define LV_USE_DEMO_VIDEO_PLAY 0
#define LV_USE_DEMO_VIDEO_DIAL 0
#define LV_USE_DEMO_SURFACE_FULL_SIZE 0
#endif

#define LV_USE_DEMO_WATCH 0

#define LV_USE_DEMO_EXT 1
#if LV_USE_DEMO_EXT
#define LV_USE_DEMO_BARCODE 1
#define LV_USE_DEMO_IMG_CACHE_MANAGER 1
#define LV_USE_DEMO_FONT_VECTOR 1
#define LV_USE_DEMO_SCROLL_LIST 1
#define LV_USE_DEMO_ZOOM_IMG 1
#define LV_USE_DEMO_SCROLL_TEXT 1
#define LV_USE_DEMO_IMG_EXT 1
#define LV_USE_DEMO_IMG 1
#define LV_USE_DEMO_RECT 1

#define LV_USE_DEMO_CANVAS_EXT 1
#if LV_USE_DEMO_CANVAS_EXT
#define LV_USE_DEMO_PATTERN 0
#define LV_USE_DEMO_ROTATE_SCALE_MATRIX 0
#endif

#if defined(MEMORY_MINI)
#define LV_USE_DEMO_RAINBOW 0
#define LV_USE_DEMO_COVERFLOW 0
#define LV_USE_DEMO_LOAD_SCR 0
#define LV_USE_DEMO_KALEIDOSCOPE 0
#else
#define LV_USE_DEMO_RAINBOW 1
#define LV_USE_DEMO_COVERFLOW 1
#define LV_USE_DEMO_LOAD_SCR 1
#define LV_USE_DEMO_KALEIDOSCOPE 1
#endif

#if LV_USE_DEMO_KALEIDOSCOPE
#define DEMO_KALEIDOSCOPE_UTILS 0
#define DEMO_KALEIDOSCOPE_ANIM 1
#endif
#endif // LV_USE_DEMO_EXT

#endif
