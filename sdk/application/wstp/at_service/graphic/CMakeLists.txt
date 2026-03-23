#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "graphic_at_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic_cmd_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic_at_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic_rx_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic_tx_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic_msg_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample/gpu_sample.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sample/
    ${ROOT_DIR}/middleware/services/gui/uikit/sample
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/lvgl/src
    ${ROOT_DIR}/middleware/services/gui/lvgl/config
    ${ROOT_DIR}/middleware/services/gui/lvgl/sample
    ${ROOT_DIR}/middleware/services/gui/lvgl/proprietary
    ${ROOT_DIR}/middleware/services/gui/lvgl/proprietary/include/dfx
    ${ROOT_DIR}/middleware/services/gpu/drv/include
    ${ROOT_DIR}/middleware/services/gpu/drv/dpu/dfx
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include
)

set(PRIVATE_DEFINES
    LV_CONF_INCLUDE_SIMPLE
)

set(PUBLIC_DEFINES
    SUPPORT_AT_GRAPHIC
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
