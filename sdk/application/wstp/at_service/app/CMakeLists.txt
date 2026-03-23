#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "app_at_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_cmd_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_at_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_rx_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_tx_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_msg_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_test_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_common_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_test_perf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_dfx_mem.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/vendor/sikey/drivers
)

set(PRIVATE_HEADER
)

if (DEFINED BUILD_TIME)
    set(PRIVATE_DEFINES
        TARGET_NAME="${TARGET_COMMAND}"
        BUILD_TIME="${BUILD_TIME}"
        SUPPORT_AT_APP
    )
else()
    set(PRIVATE_DEFINES
    )
endif()

set(PUBLIC_DEFINES
    SUPPORT_AT_APP
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
    -Wno-unused-parameter
    -Wno-int-conversion
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)

build_component()
