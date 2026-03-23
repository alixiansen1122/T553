# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(proprietary/src/service)
add_subdirectory_if_exist(sample)

set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)
# set public headers
set(public_headers
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/surface/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits/hals
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${PROJECT_ROOT_DIR}/hal/input/include
)

# set public configs
set(public_configs
    -std=c++11
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-missing-field-initializers
    -Wno-writable-strings
    -Wno-reorder-ctor
    -Wno-tautological-pointer-compare
    -Wno-unused-function
    -Werror=return-type
)

# build graphic utils
set(COMPONENT_NAME "graphic_utils")
file(GLOB_RECURSE framewoks ${CMAKE_CURRENT_SOURCE_DIR}/utils/frameworks/*.cpp)
file(GLOB_RECURSE properity ${CMAKE_CURRENT_SOURCE_DIR}/proprietary/src/utils/*/*.cpp)
set(COMPONENT_SRC
    ${framewoks}
    ${properity}
)
list(REMOVE_ITEM COMPONENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/utils/frameworks/sys_info.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/utils/frameworks/hals/gfx_engines.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/utils/frameworks/hals/hi_fbdev.cpp)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${public_headers}
)

set(PRIVATE_DEFINES
)

list(REMOVE_ITEM CCDEFINES VERSION_STANDARD)
set(PUBLIC_DEFINES
    ${CCDEFINES}
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    ${public_configs}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

# build graphic ui
set(COMPONENT_NAME "graphic_ui")
set(PROP_UI_SRC ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks)
file(GLOB_RECURSE framewoks ${UIKIT_ROOT_DIR}/ui/frameworks/*/*.cpp)
file(GLOB_RECURSE common_src ${PROP_UI_SRC}/common/*.cpp)
file(GLOB_RECURSE components_src ${PROP_UI_SRC}/components/*.cpp)
file(GLOB_RECURSE core_src ${PROP_UI_SRC}/core/*.cpp)
file(GLOB_RECURSE dfx_src ${PROP_UI_SRC}/dfx/*.cpp)
file(GLOB_RECURSE dock_src ${PROP_UI_SRC}/dock/*.cpp)
file(GLOB_RECURSE draw_src ${PROP_UI_SRC}/draw/*.cpp)
file(GLOB_RECURSE imgdecode_src ${PROP_UI_SRC}/imgdecode/*.cpp)
file(GLOB_RECURSE effect_src ${PROP_UI_SRC}/swipe_effect/*.cpp)
set(COMPONENT_SRC
    ${framewoks}
    ${common_src}
    ${components_src}
    ${core_src}
    ${dfx_src}
    ${dock_src}
    ${draw_src}
    ${imgdecode_src}
    ${effect_src}
)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/components/image_adapter.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/components/ui_dialog.cppp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/components/ui_repeat_button.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/components/ui_surface_view.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/components/ui_video.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/dfx/performance_task.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/dfx/ui_screenshot.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/dock/lite_input_distributer.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/dock/ohos/ohos_input_device.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/dock/screen_device_proxy.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/window/window.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/window/window_impl.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/ui/frameworks/engines/gfx/hi3516/hi3516_engine.cpp)
list(REMOVE_ITEM COMPONENT_SRC ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks/components/ui_image_picker.cpp)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
)

set(PRIVATE_HEADER
    ${public_headers}
    ${ROOTDIR}/open_source/openharmony/third_party/cJSON
    ${ROOTDIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOTDIR}/open_source/openharmony/third_party/freetype/include
    ${ROOTDIR}/open_source/openharmony/third_party/icu/icu4c/source/common
    ${ROOTDIR}/open_source/openharmony/third_party/harfbuzz/src
    ${ROOTDIR}/open_source/openharmony/third_party/libjpeg
    ${ROOTDIR}/open_source/openharmony/third_party/libpng
    ${ROOTDIR}/open_source/openharmony/foundation/graphic/ui/frameworks/imgdecode
    ${ROOTDIR}/application/wearable/nativeapp/nativeui/main/include/dial
    ${ROOTDIR}/application/wearable/service/dial_market
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/format/interfaces/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    ${public_configs}
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

# build uikit ext
set(COMPONENT_NAME "uikit_ext")
set(PROP_UI_SRC ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks)
file(GLOB_RECURSE lottie ${PROP_UI_SRC}/lottie/*.cpp)
file(GLOB_RECURSE components_ext ${PROP_UI_SRC}/components_ext/*.cpp)
set(COMPONENT_SRC
    ${components_ext}
    ${lottie}
    ${PROP_UI_SRC}/font/ui_font_hw_cache.cpp
    ${PROP_UI_SRC}/font/ui_font_hw_vector.cpp
    ${PROP_UI_SRC}/engines/gfx/lite_m_gfx_engine.cpp
    ${PROP_UI_SRC}/font/ui_shaping_cache.cpp
    ${PROP_UI_SRC}/font/ui_text_shaping_ext.cpp
    ${PROP_UI_SRC}/font/font_global_manager.cpp
)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
)

set(PRIVATE_HEADER
    ${public_headers}
    ${ROOTDIR}/open_source/openharmony/third_party/freetype/include
    ${ROOTDIR}/open_source/openharmony/third_party/icu/icu4c/source/common
    ${ROOTDIR}/open_source/openharmony/third_party/harfbuzz/src
    ${ROOTDIR}/open_source/flatbuffers/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    ${public_configs}
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

# build traceui
set(COMPONENT_NAME "graphics_ui_traceui")
set(TRACEUI_SRC_DIR ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks/dfx/traceui)

if (DEFINES MATCHES "__LITEOS__")
set(TRACEUI_SRC
    ${TRACEUI_SRC_DIR}/traceui_buf.c
    ${TRACEUI_SRC_DIR}/traceui_convert_perfetto_proto.c
    ${TRACEUI_SRC_DIR}/traceui_core.c
)
endif()

set(SOURCES
    ${TRACEUI_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
)

set(PRIVATE_HEADER
    ${public_headers}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
