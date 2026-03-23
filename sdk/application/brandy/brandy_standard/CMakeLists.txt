#===============================================================================
# @brief    cmake make file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "brandy_app_standard")

if("liteos_207_1_0" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${ROOT_DIR}/open_source/openharmony/third_party/lwip/lwip_adapter/liteos)
elseif("liteos_207_0_0" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${ROOT_DIR}/open_source/openharmony/third_party/lwip/lwip_adapter/liteos)
elseif("freertos" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${ROOT_DIR}/open_source/openharmony/third_party/lwip/lwip_adapter/freertos)     
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/chip_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_pri_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/input_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.S
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
    ${CMAKE_SOURCE_DIR}/middleware/chips/brandy/update/include
    ${CMAKE_SOURCE_DIR}/middleware/chips/brandy/partition/include
    ${CMAKE_SOURCE_DIR}/middleware/utils/nv/nv_storage_app
    ${CMAKE_SOURCE_DIR}/middleware/utils/nv/nv_storage_lib
    ${CMAKE_SOURCE_DIR}/include/middleware/utils
    ${CMAKE_SOURCE_DIR}/open_source/openharmony/third_party/lwip/lwip_v2.1.3/src/include/
    ${CMAKE_SOURCE_DIR}/open_source/openharmony/third_party/lwip/lwip_v2.1.3/src/include/netif
    ${CMAKE_LWIP_ADAPTER_DIR}/src/include/
    ${CMAKE_LWIP_ADAPTER_DIR}/src/test/
    ${ROOT_DIR}/application/wearable/ohos_startup
    ${CMAKE_SOURCE_DIR}/vendor/sikey
    ${CMAKE_SOURCE_DIR}/vendor/sikey/service/gnss_service
    ${CMAKE_SOURCE_DIR}/vendor/sikey/service/paqichong
    ${CMAKE_SOURCE_DIR}/vendor/sikey/service/location_service
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
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
true
)
build_component()
