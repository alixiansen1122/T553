#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023-2023. All rights reserved.
Description: CMake pdr fusion
Create: 2023-08-07
]]

add_subdirectory_if_exist(hipdr)
#===================================================================================
set(COMPONENT_NAME "pdr_fusion")
set(SOURCES
    ${HIPDR_SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_boot/secure_boot_check.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

set(PRIVATE_HEADER
    ${HIPDR_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_boot
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