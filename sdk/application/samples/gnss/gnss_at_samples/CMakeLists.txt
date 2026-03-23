#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "gnss_at_samples")

return_if_not_build()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_agnss.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_common_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_normal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_pgnss.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_factory_test.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
CONFIG_GNSS_SAMPLE
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
)

set(WHOLE_LINK
true
)

set(BUILD_AS_OBJ
false
)

set(MAIN_COMPONENT
false
)
build_component()
