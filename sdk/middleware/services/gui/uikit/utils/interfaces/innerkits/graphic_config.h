/*
 * Copyright (c) 2020-2021 Device Co., Ltd.
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

/**
 * @addtogroup Graphic
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file graphic_config.h
 *
 * @brief Provides configuration items required for graphics.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GRAPHIC_CONFIG_H
#define GRAPHIC_LITE_GRAPHIC_CONFIG_H

#include "gfx_utils/graphic_types.h"
#include "graphic_hardware_config.h"

namespace OHOS {
/**
 * Defines three graphics library versions: lightweight, standard, and extended versions.
 * The three versions have different requirements on the memory and hardware.
 * The standard version is enabled by default.
 *
 * The macros of the versions are defined as follows:
 * Name                | Version Description
 * ------------------- | ----------
 * VERSION_LITE        | Lightweight version
 * VERSION_STANDARD    | Standard version
 * VERSION_EXTENDED    | Extended version
 */
#if defined(__LITEOS_M__) || defined(__FREERTOS__)
#define VERSION_LITE
#define VERSION_IOT
#elif defined _LITEOS
#define VERSION_LITE
#elif defined _WIN32 || defined __APPLE__
#define VERSION_LITE
#else
#define VERSION_STANDARD
#endif

#define ENABLE_FIXED_POINT                    1
/* Defines some configurations and their default values on LiteOS and other system platforms. */
#ifdef VERSION_IOT
/**
 * @brief Dfx cmds, which is enabled by default.
 */
#define ENABLE_DFX_CMD                    1
#if ENABLE_DFX_CMD && defined(__LITEOS_M__)
/**
 * @brief Dfx traceui cmds, used for visualizing performance analysis
 */
#define ENABLE_DFX_TRACEUI                1
#endif
/**
 * @brief Font shaping, which is disabled by default.
 *        Cannot take effect without ENABLE_ICU.
 */
#define ENABLE_SHAPING                    1
/**
 * @brief Default animation for scroll bar/button etc.
 */
#define DEFAULT_ANIMATION                 0
/**
 * @brief Advanced algorithm for line breaks, which is disabled by default.
 */
#define ENABLE_ICU                        1
/**
 * @brief Multi-Font type, which is disabled by default.
 */
#define ENABLE_MULTI_FONT                 1

/**
 * @brief Multi-window, which does not take effect.
 */
#define ENABLE_WINDOW                     0
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default.
 */
#define ENABLE_FRAME_BUFFER               1
/**
 * @brief Vector type font, which is enabled by default.
 */
#define ENABLE_VECTOR_FONT               1
/**
 * @brief Bitmap type font, which is enabled by default.
 */
#define ENABLE_BITMAP_FONT                0
/**
 * @brief Static Bitmap type font, which is disabled by default.
 */
#define ENABLE_STATIC_FONT                0
/**
 * @brief Jpeg and png, which is enabled by default.
 */
#define ENABLE_JPEG_AND_PNG               1
/**
 * @brief Cache the decoded content for Jpeg and Png image
 */
#define CACHE_JPEG_AND_PNG                1

/**
 * @brief Customize the memory area when loading image resources
 */
#define IMG_CACHE_MEMORY_CUSTOM           0
/**
 * @brief Graphics rendering hardware acceleration, which is disabled by default.
 */
#define ENABLE_HARDWARE_ACCELERATION      0
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default.
 */
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default.
 */
#define ENABLE_GFX_ENGINES                0
/**
 * @brief Graphics rendering accelerated by VGUEngine, which is enabled by default.
 */
#define ENABLE_VGU_ENGINE                 1

/**
 * @brief ARM NEON ability, which is disabled by default.
 */
#define ENABLE_ARM_NEON                   0
/**
 * @brief vector font global ability, which is disabled by default on LiteOS.
 */
#define ENABLE_FONT_VECTOR_GLOBAL                   1
 /**
  * @brief image bin ability, which is disabled by default on LiteOS.
 */
#define ENABLE_IMAGE_PACKER                   1
 /**
  * @brief draw view on fb firstly, and then get its bitmap copied from fb
 */
#define ENABLE_GET_BITMAP_BASED_ON_FB        1
 /**
  * @brief use map buffer to support view transformation. if disable it to save memory,
  *        transformation will not be supported except UIImageView, UITextureMapper and UICanvasExt.
  */
#define ENABLE_MAP_BUFFER                    0
#elif defined _LITEOS
/**
 * @brief Font shaping, which is disabled by default on LiteOS.
 *        Cannot take effect without ENABLE_ICU.
 */
#define ENABLE_SHAPING                    0
/**
 * @brief Advanced algorithm for line breaks, which is disabled by default on LiteOS.
 */
#define ENABLE_ICU                        0
/**
 * @brief Multi-Font type, which is disabled by default on LiteOS.
 */
#define ENABLE_MULTI_FONT                 0
/**
 * @brief Multi-window, which does not take effect on LiteOS.
 */
#define ENABLE_WINDOW                     0
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on LiteOS.
 */
#define ENABLE_FRAME_BUFFER               0
/**
 * @brief Vector type font, which is enabled by default on LiteOS.
 */
#define ENABLE_VECTOR_FONT                1
/**
 * @brief Bitmap type font, which is disabled by default on LiteOS.
 */
#define ENABLE_BITMAP_FONT                0
/**
 * @brief Static Bitmap type font, which is disabled by default on LiteOS.
 */
#define ENABLE_STATIC_FONT                0

#define ENABLE_JPEG_AND_PNG               0
/**
 * @brief Cache the decoded content for Jpeg and Png image
 */
#define CACHE_JPEG_AND_PNG                0
/**
 * @brief Customize the memory area when loading image resources
 */
#define IMG_CACHE_MEMORY_CUSTOM            0
/**
 * @brief Graphics rendering hardware acceleration, which is enabled by default on LiteOS.
 */
#define ENABLE_HARDWARE_ACCELERATION      1
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on LiteOS.
 */
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default on LiteOS.
 */
#define ENABLE_GFX_ENGINES                0
/**
 * @brief Graphics rendering accelerated by VGUEngine, which is disabled by default on LiteOS.
 */
#define ENABLE_VGU_ENGINE                 0
/**
 * @brief ARM NEON ability, which is disabled by default on LiteOS.
 */
#define ENABLE_ARM_NEON                   0
/**
 * @brief vector font global ability, which is disabled by default on LiteOS.
 */
#define ENABLE_FONT_VECTOR_GLOBAL                   0
/**
 * @brief image bin ability, which is disabled by default on LiteOS.
 */
#define ENABLE_IMAGE_PACKER                   0
#elif defined _WIN32 || defined __APPLE__
/**
 * @brief Font shaping, which is enabled by default on WIN32.
 *        Cannot take effect without ENABLE_ICU.
 */
#define ENABLE_SHAPING                    0
/**
 * @brief Advanced algorithm for line breaks, which is enabled by default on WIN32.
 */
#define ENABLE_ICU                        0
/**
 * @brief Multi-Font type, which is enabled by default on WIN32.
 */
#define ENABLE_MULTI_FONT                 0
/**
 * @brief Multi-window, which is disabled by default on WIN32.
 */
#define ENABLE_WINDOW                     0
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on WIN32.
 */
#define ENABLE_FRAME_BUFFER               0
/**
 * @brief Vector type font, which is enabled by default on WIN32.
 */
#define ENABLE_VECTOR_FONT                1
/**
 * @brief Bitmap type font, which is disabled by default on WIN32.
 */
#define ENABLE_BITMAP_FONT                0
/**
 * @brief Static Bitmap type font, enabled by default on WIN32.
 */
#define ENABLE_STATIC_FONT                0

#define ENABLE_JPEG_AND_PNG               1
/**
 * @brief Cache the decoded content for Jpeg and Png image
 */
#define CACHE_JPEG_AND_PNG                1
/**
 * @brief Customize the memory area when loading image resources
 */
#define IMG_CACHE_MEMORY_CUSTOM           0
/**
 * @brief ARM NEON ability, which is disabled by default on WIN32.
 */
#define ENABLE_ARM_NEON                   0
/**
 * @brief Graphics rendering hardware acceleration, which is disabled by default on WIN32.
 */
#define ENABLE_HARDWARE_ACCELERATION      0
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on WIN32.
 */
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default on WIN32.
 */
#define ENABLE_GFX_ENGINES                0
/**
 * @brief Graphics rendering accelerated by VGUEngine, which is disabled by default on WIN32.
 */
#define ENABLE_VGU_ENGINE                 0
/**
 * @brief vector font global ability, which is disabled by default on LiteOS.
 */
#define ENABLE_FONT_VECTOR_GLOBAL                   0
/**
 * @brief image bin ability, which is disabled by default on LiteOS.
 */
#define ENABLE_IMAGE_PACKER                   0
#else
/**
 * @brief Font shaping, which is enabled by default on other platforms.
 *        Cannot take effect without ENABLE_ICU.
 */
#define ENABLE_SHAPING                    0
/**
 * @brief Advanced algorithm for line breaks, which is enabled by default on other platforms.
 */
#define ENABLE_ICU                        0
/**
 * @brief Multi-Font type, which is enabled by default on other platforms.
 */
#define ENABLE_MULTI_FONT                 0
/**
 * @brief Multi-window, which is enabled by default on other platforms.
 */
#define ENABLE_WINDOW                     1
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on other platforms.
 */
#define ENABLE_FRAME_BUFFER               0
/**
 * @brief Vector type font, which is enabled by default on other platforms.
 */
#define ENABLE_VECTOR_FONT                1
/**
 * @brief Bitmap type font, which is disabled by default on other platforms.
 */
#define ENABLE_BITMAP_FONT                0
/**
 * @brief Static Bitmap type font, which is disabled by default on LiteOS.
 */
#define ENABLE_STATIC_FONT                0

#define ENABLE_JPEG_AND_PNG               1
/**
 * @brief Cache the decoded content for Jpeg and Png image
 */
#define CACHE_JPEG_AND_PNG                1
/**
 * @brief Customize the memory area when loading image resources
 */
#define IMG_CACHE_MEMORY_CUSTOM            1
/**
 * @brief Graphics rendering hardware acceleration, which is disabled by default on other platforms.
 */
#define ENABLE_HARDWARE_ACCELERATION      0
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on other platforms.
 */
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is enabled by default on other platforms.
 */
#define ENABLE_GFX_ENGINES                1
/**
 * @brief Graphics rendering accelerated by VGUEngine, which is disabled by default on other platforms.
 */
#define ENABLE_VGU_ENGINE                 0
/**
 * @brief ARM NEON ability, which is enabled by default on other platforms.
 */
#define ENABLE_ARM_NEON                   1
/**
 * @brief vector font global ability, which is disabled by default on LiteOS.
 */
#define ENABLE_FONT_VECTOR_GLOBAL                   0
/**
 * @brief image bin ability, which is disabled by default on LiteOS.
 */
#define ENABLE_IMAGE_PACKER                   0
#endif

#define ENABLE_ROTATE_INPUT               1

#define ENABLE_VIBRATOR                   1

#define ENABLE_FOCUS_MANAGER              1
/**
 * @brief Graphics local rendering, which is enabled by default.
 */
#define LOCAL_RENDER                      0
/**
 * @brief Fully rectangle render everytime, which is disabled by default.
 */
#define FULLY_RENDER                      0

/**
 * @brief Enable SoftKeyboard, which is enabled by default.
 */
#define ENABLE_SOFT_KEYBOARD              1

/**
 * @brief Actually use ARM NEON optimization.
 *        __ARM_NEON__ and __ARM_NEON are set by the compiler according to the compilation option -mfpu=neon
 */
#if (defined(__ARM_NEON__) || defined(__ARM_NEON)) && ENABLE_ARM_NEON == 1
#define ARM_NEON_OPT
#endif
/**
 * @brief Enable UISweepClock widget, which is denabled by default.
 */
#define ENABLE_SWEEP_CLOCK                1

/**
 * @brief Do not support CacheManager, which is disabled by default.
 */
#define NO_CACHE_MANAGER                  0
/**
 * @brief Graphics bottom-layer RGBA, which is enabled by default.
 */
#define ENABLE_BUFFER_RGBA                1
/**
 * @brief Debug mode, which is disabled by default.
 */
#define ENABLE_DEBUG                      1
/**
 * @brief Memory check, which is enabled by default. Statistics system dynamic memory after it is enabled.
 */
#define ENABLE_MEMORY_CHECK               1
/**
 * @brief Memory hook, which is enabled by default. The system memory allocation is taken over after it is enabled.
 */
#define ENABLE_MEMORY_HOOKS               1
/**
 * @brief Function for monitoring the image refresh frame rate, which is disabled by default.
 */
#define ENABLE_FPS_SUPPORT                0
/**
 * @brief Anti-aliasing, which is enabled by default.
 */
#define ENABLE_ANTIALIAS                  1
/**
 * @brief Rectangle anti-aliasing, which is disabled by default.
 */
#define ENABLE_RECT_ANTIALIAS             0
/**
 * @brief Font color mode, which is disabled by default.
 * After it is enabled, the font color mode is set to <b>4</b> to accelerate font rendering.
 */
#define ENABLE_SPEC_FONT                  0
/**
 * @brief Log function of a graphics subsystem, which is enabled by default
 */
#define ENABLE_GRAPHIC_LOG                1
/**
 * @brief Performance tracking for debugging, which is disabled by default.
 */
#define ENABLE_DEBUG_PERFORMANCE_TRACE    0
/**
 * @brief Function for receiving input events in screen-off mode, which is disabled by default.
 */
#define ENABLE_AOD                        0
/**
 * @brief Defines the log level. A smaller value indicates a higher priority.
 * Logs whose priorities are higher than a specified level can be recorded.
 * Log levels:
 * FATAL: fatal level 0
 * ERROR: error level 1
 * WARN: warning level 2
 * INFO: info level 3
 * DEBUG: debugging level 4
 */
#define GRAPHIC_LOG_LEVEL                 2
/**
 * @brief Defines the color depth of graphics rendering. The default value is <b>32</b> bits.
 * The value can be <b>16</b> or <b>32</b>.
 */
#define COLOR_DEPTH 32

/**
 * @brief Represents the code number of the layer pixel format.
 * 0:     LAYER_PF_ARGB1555
 * 1:     LAYER_PF_ARGB8888
 * Other: LAYER_PF_ARGB8888
 */

#if LAYER_PF_CODE == 0
#define LAYER_PF_ARGB1555
#elif LAYER_PF_CODE == 1
#define LAYER_PF_ARGB8888
#elif LAYER_PF_CODE == 2
#define LAYER_PF_RGB888
#else
#define LAYER_PF_ARGB8888
#endif

/**
 * @brief Defines whether the C library math function is supported.
 * 0:    Unsupport
 * 1:    Support
  */
#define ENABLE_CMATH              1

/**
 * @brief Defines the file name of default vector font.
 */

#ifdef VERSION_IOT
#define DEFAULT_VECTOR_FONT_FILENAME      "HarmonyOS_Sans_SC_Regular.ttf"
#define BOLD_VECTOR_FONT_FILENAME         "HarmonyOS_Sans_SC_Bold.ttf"
#else
#if ENABLE_VECTOR_FONT
#define DEFAULT_VECTOR_FONT_FILENAME      "SourceHanSansSC-Regular.otf"
#else
#define DEFAULT_VECTOR_FONT_FILENAME      "SourceHanSansSC-Regular"
#endif
#endif

/* resources path. */
#ifdef VERSION_IOT
#define RES_PATH "/user/res/"
#define CMP_RES_PATH_CS "/user/res/CS_"
#define CMP_RES_PATH_ES "/user/res/ES_"
#endif
/* Default font size. The default value is <b>18</b>. */
static constexpr uint8_t DEFAULT_VECTOR_FONT_SIZE = 18;
/* Defines the file name of default line break rule. */
#define DEFAULT_LINE_BREAK_RULE_FILENAME   "line_cj.brk"
/* Defines some configurations and their default values on LiteOS and other system platforms. */
#ifdef VERSION_IOT
#if HW_ROTATE_ANGLE == 0 || HW_ROTATE_ANGLE == 2
/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
static constexpr int16_t HORIZONTAL_RESOLUTION = RESOLUTION_WIDTH;
/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
static constexpr int16_t VERTICAL_RESOLUTION = RESOLUTION_HEIGHT;
#else
/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
static constexpr int16_t HORIZONTAL_RESOLUTION = RESOLUTION_HEIGHT;
/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
static constexpr int16_t VERTICAL_RESOLUTION = RESOLUTION_WIDTH;
#endif
#elif defined _LITEOS
/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
static constexpr int16_t HORIZONTAL_RESOLUTION = RESOLUTION_WIDTH;
/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
static constexpr int16_t VERTICAL_RESOLUTION = RESOLUTION_HEIGHT;
#elif defined _WIN32 || defined __APPLE__
/* Resolution width of a graphics display screen. The default value is <b>960</b>. */
static constexpr int16_t HORIZONTAL_RESOLUTION = 960;
/* Resolution height of a graphics display screen. The default value is <b>480</b>. */
static constexpr int16_t VERTICAL_RESOLUTION = 480;
#else
/* Resolution width of a graphics display screen. The default value is <b>960</b>. */
static constexpr int16_t HORIZONTAL_RESOLUTION = 960;
/* Resolution height of a graphics display screen. The default value is <b>480</b>. */
static constexpr int16_t VERTICAL_RESOLUTION = 480;
#endif

#ifndef VERSION_LITE
static constexpr const char* MEDIA_IMAGE_PLAY_CENTER = "/user/data/videoplayer_play_center.png";
static constexpr const char* MEDIA_IMAGE_PLAY = "/user/data/videoplayer_play.png";
static constexpr const char* MEDIA_IMAGE_PAUSE = "/user/data/videoplayer_pause.png";
static constexpr const char* MEDIA_IMAGE_VOLUME = "/user/data/videoplayer_volume.png";
static constexpr const char* MEDIA_IMAGE_MUTE = "/user/data/videoplayer_mute.png";
#endif

/* Defines some configurations and their default values on LiteOS and other system platforms. */
#if defined QT_COMPILER
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = ".\\dump_dom_tree.json";
/* Default file path for font */
static constexpr const char* VECTOR_FONT_DIR = "..\\..\\simulator\\font\\";
#elif defined _WIN32
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = ".\\dump_dom_tree.json";
/* Default file path for font */
static constexpr const char* VECTOR_FONT_DIR = "..\\..\\tools\\font\\font_tool\\font_tool\\font\\";
#elif defined _LITEOS
/* Default file path for screenshots */
static constexpr const char* DEFAULT_SCREENSHOT_PATH = "user/log/screenshot.bin";
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = "user/log/dump_dom_tree.json";
#elif defined VERSION_IOT
/* Default file path for screenshots */
static constexpr const char* DEFAULT_SCREENSHOT_PATH = "/storage/screenshot.bin";
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = "/storage/dump_dom_tree.json";
/* Default file path for font */
static constexpr const char* VECTOR_FONT_DIR = "/user/res/";
#else
/* Default file path for screenshots */
static constexpr const char* DEFAULT_SCREENSHOT_PATH = "/storage/screenshot.bin";
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = "/storage/dump_dom_tree.json";
/* Default file path for font */
static constexpr const char* VECTOR_FONT_DIR = "/user/data/";
#endif
/* Default task execution period. The default value is <b>16</b> ms. */
static constexpr uint8_t DEFAULT_TASK_PERIOD = 16;
/* Window manager execution period. The default value is <b>16</b> ms. */
static constexpr uint8_t WMS_MAIN_TASK_PERIOD = 16;
static constexpr uint8_t IMG_CACHE_SIZE = 64; /* Maximum number of cached images. The default value is <b>64</b>. */
static constexpr uint8_t INDEV_READ_PERIOD = 10; /* Input event read cycle. The default value is <b>10</b> ms. */
/* Drag distance threshold of a drag event. The default value is <b>10px</b>. */
static constexpr uint8_t INDEV_DRAG_LIMIT = 10;
/* Maximum depth of view nesting. The default value is <b>64</b>. */
static constexpr uint8_t COMPONENT_NESTING_DEPTH = 64;
/* Long-press event threshold. The default value is <b>1000</b> ms. */
static constexpr uint16_t INDEV_LONG_PRESS_TIME = 1000;
/* Delay for reporting a press event on a draggable object. The default value is <b>100</b> ms. */
static constexpr uint16_t INDEV_PRESS_TIME_IN_DRAG = 100;
/* Maximum number of bytes in a text that can be continuously displayed. The default value is <b>4096</b> bytes. */
static constexpr uint16_t MAX_TEXT_LENGTH = 4096;
/* Maximum value of the graphic display range. The default value is <b>16383px</b>. */
static constexpr int32_t COORD_MAX = 16383;
/* Minimum value of the graphic display range. The default value is <b>-16384px</b>. */
static constexpr int32_t COORD_MIN = -16384;
static constexpr uint32_t HARFBUZ_CACHE_LENGTH = 0x19000; // 500K
static constexpr uint16_t MAX_LINE_WIDTH = 128;
/* Maximum length of a QR code string. The default value is <b>256</b>. */
static constexpr uint32_t QRCODE_VAL_MAX = 512;
/* Rotate sensitivity factor. The default value is <b>1.0<b> time. */
static constexpr float ROTATE_SENSITIVITY = 1.0;
/* Set the drag-back area. */
static constexpr int16_t MAX_X_IN_DRAG_REGION = HORIZONTAL_RESOLUTION;

static constexpr float DEFAULT_SWIPE_VIEW_ROTATE_FACTOR = 1.4;
static constexpr float DEFAULT_LIST_ROTATE_FACTOR = 0.6;
static constexpr float DEFAULT_PICKER_ROTATE_FACTOR = 0.6;
static constexpr float DEFAULT_SCROLL_VIEW_ROTATE_FACTOR = 1.0;
#ifdef _WIN32
static constexpr float DEFAULT_SLIDER_ROTATE_FACTOR = -1;
#else
static constexpr float DEFAULT_SLIDER_ROTATE_FACTOR = -0.05;
#endif
static constexpr uint8_t DEFAULT_SCROLL_VIEW_VIBRATION_LEN = 30;
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_CONFIG_H
