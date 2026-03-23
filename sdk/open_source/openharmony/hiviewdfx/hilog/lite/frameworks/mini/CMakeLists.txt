#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hilog_lite")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_log_limit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_output_log.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hiview/lite
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hilog/lite/interfaces/native/kits/hilog_lite
)

set(PRIVATE_DEFINES
    HIVIEW_LOG_FILE_SIZE=8192
    LOG_LIMIT_DEFAULT=30
    OHOS_DEBUG
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
