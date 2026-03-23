#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hievent_lite")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/hiview_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/hiview_output_event.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/native/innerkits
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hiview/lite
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hilog/lite/interfaces/native/kits/hilog_lite
)

set(PRIVATE_DEFINES
    FAULT_EVENT_FILE_SIZE=1024
    UE_EVENT_FILE_SIZE=1024
    STAT_EVENT_FILE_SIZE=1024
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
