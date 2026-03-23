#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hiview_lite")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_util.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hilog/lite/interfaces/native/kits/hilog_lite
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hievent/lite/interfaces/native/innerkits
)

set(PRIVATE_DEFINES
    OUTPUT_OPTION=1
    HILOG_LITE_SWITCH=1
    DUMP_LITE_SWITCH=0
    HIEVENT_LITE_SWITCH=1
    LOG_OUTPUT_MODULE=-1
    LOG_STATIC_CACHE_SIZE=1024
    HIVIEW_HILOG_FILE_BUF_SIZE=512
    OUTPUT_LEVEL=1
    OHOS_DEBUG
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
