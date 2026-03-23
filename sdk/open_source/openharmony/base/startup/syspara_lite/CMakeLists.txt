#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "syspara")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_sys_param.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parameter_common.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
    -Wno-error=pointer-sign
    -Wno-unused-const-variable
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
