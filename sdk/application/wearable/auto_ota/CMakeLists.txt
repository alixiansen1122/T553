#===============================================================================
# @brief    cmake make file
# Copyright (c) 2023 @CompanyNameMagicTag
#===============================================================================
set(COMPONENT_NAME "auto_ota")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/auto_ota_service.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/protocol/bt/host/bt/bts/sdk/inc
    ${ROOT_DIR}/protocol/bt/host/bt/bts/sdk/inc/stack_api
    ${ROOT_DIR}/protocol/bt/host/bt/bts/sdk/src/inc
    ${ROOT_DIR}/protocol/bt/host/bt/bts/host/inc
    ${ROOT_DIR}/protocol/bt/host/bt/bts/service/wear/gdk/inc
    ${ROOT_DIR}/protocol/bt/host/bt/bts/sdk/src/stack_api
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/log
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/sdk_common
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/host
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/hci
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/gdk_common
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/ahi_common
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bg_common/include/ahi_a
    ${ROOT_DIR}/protocol/bt/host/bt/bts/brandy_service/service/src
    ${ROOT_DIR}/protocol/bt/host/bt/bts/brandy_service/service/src/base
    ${ROOT_DIR}/protocol/bt/host/bt/bts/bt_sample/sample_common
    ${ROOT_DIR}/application/wearable/service/auto_ota_msg
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
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

build_component()
