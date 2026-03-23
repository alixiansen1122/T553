#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "svr_dynload")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/svr_dynload.c
    ${CMAKE_CURRENT_SOURCE_DIR}/los_dynload_gsymbol.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/liteos/liteos_v207.1.0/Huawei_LiteOS/kernel/extended/dynload/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_DYNLOAD
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-incompatible-library-redeclaration
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
