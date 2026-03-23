#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "button_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/button_porting.c
)

set(PUBLIC_HEADER
${ROOT_DIR}/vendor/sikey/drivers/led
${ROOT_DIR}/vendor/sikey/drivers/fg/sh366102
)

set(WHOLE_LINK
    true
)

build_component()
