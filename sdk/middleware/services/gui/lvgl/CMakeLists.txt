#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOT_DIR})
set(LVGL_DIR ${PROJECT_ROOT_DIR}/middleware/services/gui/lvgl)
set(GUI_DIR ${PROJECT_ROOT_DIR}/middleware/services/gui)

set(CFLAG ${ccflags})

set(COMPONENT_NAME "lvgl")

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${LVGL_DIR}/proprietary/src/dfx/*.c
    ${LVGL_DIR}/proprietary/src/misc/*.c
    ${LVGL_DIR}/proprietary/src/porting/*.c
)
set(SOURCES
    ${SOURCES}
)

set(PUBLIC_HEADER
    ${LVGL_DIR}/src
    ${LVGL_DIR}/proprietary/include/dfx
    ${LVGL_DIR}/proprietary/include/porting
    ${ROOT_DIR}/middleware/services/gpu/drv/include
)

set(PRIVATE_HEADER
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/core
    ${LVGL_DIR}/src/misc
    ${LVGL_DIR}/src/hal
    ${LVGL_DIR}/src/font
    ${LVGL_DIR}/src/widgets
    ${LVGL_DIR}/src/draw
    ${LVGL_DIR}/src/draw/sw
    ${LVGL_DIR}/src/extra/others/snapshot
    ${LVGL_DIR}/proprietary/src/dfx
    ${LVGL_DIR}/proprietary/src/decoder
    ${LVGL_DIR}/proprietary/src/draw
    ${LVGL_DIR}/proprietary/src/font
    ${LVGL_DIR}/proprietary/src/utils
    ${LVGL_DIR}/proprietary/src/misc
    ${LVGL_DIR}/config
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
    LV_CONF_INCLUDE_SIMPLE
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-sign-compare
    -Wno-unused-function
    -Wno-implicit-function-declaration
    -Wno-incompatible-function-pointer-types
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

if("MEMORY_MINI" IN_LIST DEFINES)
set(COMPONENT_NAME "lvgl_proprietary_mini")
else()
set(COMPONENT_NAME "lvgl_proprietary")
endif()

set(SOURCES
    ${LVGL_DIR}/proprietary/src/core/lv_scr_load_anim.c
    ${LVGL_DIR}/proprietary/src/utils/lv_hash_map.c
    ${LVGL_DIR}/proprietary/src/utils/lv_hw_draw_utils.c
    ${LVGL_DIR}/proprietary/src/utils/lv_matrix.c
    ${LVGL_DIR}/proprietary/src/utils/lv_mutex.c
    ${LVGL_DIR}/proprietary/src/utils/lv_transform.c
    ${LVGL_DIR}/proprietary/src/utils/lv_clip_img.c
    ${LVGL_DIR}/proprietary/src/utils/lv_kaleidoscope_utils.c
    ${LVGL_DIR}/proprietary/src/widgets/lv_canvas_ext.c
    ${LVGL_DIR}/proprietary/src/widgets/lv_img_ext.c
    ${LVGL_DIR}/proprietary/src/widgets/lv_surface_view.c
    ${LVGL_DIR}/proprietary/src/widgets/lv_cross_view.c
    ${LVGL_DIR}/proprietary/src/widgets/lv_kaleidoscope.c
    ${LVGL_DIR}/proprietary/src/widgets/lv_barcode.c
    ${LVGL_DIR}/proprietary/src/font/lv_font_vector.c
    ${LVGL_DIR}/proprietary/src/font/lv_font_loader_ext.c
    ${LVGL_DIR}/proprietary/src/font/lv_font_vector_cache.c
    ${LVGL_DIR}/proprietary/src/decoder/lv_hw_jpeg_decoder.c
    ${LVGL_DIR}/proprietary/src/decoder/lv_file_decoder.c
    ${LVGL_DIR}/proprietary/src/decoder/lv_img_cache_ext.c
    ${LVGL_DIR}/proprietary/src/decoder/lv_img_cache_manager.c
    ${LVGL_DIR}/proprietary/src/draw/lv_draw_vgu.c
)

set(PUBLIC_HEADER
    ${LVGL_DIR}/proprietary/include/decoder
    ${LVGL_DIR}/proprietary/include/font
    ${LVGL_DIR}/proprietary/include/utils
    ${LVGL_DIR}/proprietary/include/widgets
    ${LVGL_DIR}/proprietary/include/core
)

set(PRIVATE_HEADER
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/core
    ${LVGL_DIR}/src/misc
    ${LVGL_DIR}/src/hal
    ${LVGL_DIR}/src/font
    ${LVGL_DIR}/src/widgets
    ${LVGL_DIR}/src/draw
    ${LVGL_DIR}/src/draw/sw
    ${LVGL_DIR}/src/extra/others/snapshot
    ${LVGL_DIR}/demos
    ${LVGL_DIR}/sample
    ${LVGL_DIR}/config
    ${LVGL_DIR}/proprietary/src/dfx
    ${LVGL_DIR}/proprietary/src/decoder
    ${LVGL_DIR}/proprietary/src/draw
    ${LVGL_DIR}/proprietary/src/font
    ${LVGL_DIR}/proprietary/src/utils
    ${LVGL_DIR}/proprietary/src/misc
    ${ROOT_DIR}/middleware/services/gui/hal/input/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/open_source/openharmony/third_party/freetype/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-absolute-value
    -Wno-unused-function
    -Wno-int-conversion
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-missing-field-initializers
    -Wno-incompatible-function-pointer-types
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Wno-compare-distinct-pointer-types
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lvgl_service")

file(GLOB_RECURSE LVGL_PROPRIETARY_SRC ${LVGL_DIR}/proprietary/service/*.c)
set(COMPONENT_SRC
    ${LVGL_PROPRIETARY_SRC}
)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${LVGL_DIR}/proprietary/service
)

set(PRIVATE_HEADER
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/core
    ${LVGL_DIR}/src/misc
    ${LVGL_DIR}/src/hal
    ${LVGL_DIR}/src/font
    ${LVGL_DIR}/src/widgets
    ${LVGL_DIR}/src/draw
    ${LVGL_DIR}/src/draw/sw
    ${LVGL_DIR}/src/extra/others/snapshot
    ${LVGL_DIR}/demos
    ${LVGL_DIR}/sample
    ${LVGL_DIR}/config
    ${LVGL_DIR}/proprietary/src/dfx
    ${LVGL_DIR}/proprietary/src/decoder
    ${LVGL_DIR}/proprietary/src/draw
    ${LVGL_DIR}/proprietary/src/font
    ${LVGL_DIR}/proprietary/src/utils
    ${LVGL_DIR}/proprietary/src/misc
    ${ROOT_DIR}/middleware/services/gui/hal/input/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-absolute-value
    -Wno-unused-function
    -Wno-int-conversion
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-missing-field-initializers
    -Wno-incompatible-function-pointer-types
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Wno-pointer-integer-compare
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lvgl_sample")

file(GLOB_RECURSE LVGL_SAMPLE_SRC ${LVGL_DIR}/sample/*.c)
set(COMPONENT_SRC
    ${LVGL_SAMPLE_SRC}
)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sample
)

set(PRIVATE_HEADER
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/core
    ${LVGL_DIR}/src/misc
    ${LVGL_DIR}/src/hal
    ${LVGL_DIR}/src/font
    ${LVGL_DIR}/src/widgets
    ${LVGL_DIR}/src/draw
    ${LVGL_DIR}/src/draw/sw
    ${LVGL_DIR}/src/extra/others/snapshot
    ${LVGL_DIR}/demos
    ${LVGL_DIR}/sample
    ${LVGL_DIR}/sample/watch
    ${LVGL_DIR}/config
    ${LVGL_DIR}/proprietary/src/dfx
    ${LVGL_DIR}/proprietary/src/decoder
    ${LVGL_DIR}/proprietary/src/draw
    ${LVGL_DIR}/proprietary/src/font
    ${LVGL_DIR}/proprietary/src/utils
    ${LVGL_DIR}/proprietary/src/misc
    ${ROOT_DIR}/middleware/services/gui/hal/input/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-implicit-function-declaration
    -Wno-int-conversion
    -Wno-incompatible-pointer-types
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lvgl_demos")

file(GLOB_RECURSE DEMO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/demos/*.c)
set(SOURCES
    ${DEMO_SOURCES}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/demos
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LVGL_DIR}
    ${LVGL_DIR}/config
    ${LVGL_DIR}/proprietary/src/dfx
    ${LVGL_DIR}/proprietary/src/decoder
    ${LVGL_DIR}/proprietary/src/draw
    ${LVGL_DIR}/proprietary/src/font
    ${LVGL_DIR}/proprietary/src/utils
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${GUI_DIR}/hal/common/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-implicit-function-declaration
    -Wno-int-conversion
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
