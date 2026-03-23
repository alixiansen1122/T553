#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "xts_adapt")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cjson_adapt.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/hals/file
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    UNITY_OUTPUT_CHAR=xts_test_output_char
    OHOS_DEBUG
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
