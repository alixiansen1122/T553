#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ohos_startup")

set(SOURCES
    # ${CMAKE_CURRENT_SOURCE_DIR}/ohosfwk_startup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_adapt_entropy.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/
    # ${ROOT_DIR}/application/wearable/service/device_msg/
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
