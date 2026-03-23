#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "media_at_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/media_cmd_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/media_at_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/media_rx_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/media_tx_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/media_msg_manager.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_AT_MEDIA
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
    -Wno-incompatible-pointer-types-discards-qualifiers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
