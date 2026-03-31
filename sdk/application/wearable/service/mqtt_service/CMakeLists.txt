set(COMPONENT_NAME "mqtt_service")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_service.h
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/dfx/diag/
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/mqtt/src
    ${ROOT_DIR}/vendor/sikey/service/nv_service
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${ROOT_DIR}/open_source/openharmony/kernel/liteos_m/kal/cmsis
    ${ROOT_DIR}/vendor/sikey/service/modem_service
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_service.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c99
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
    WEAR_LITEOS_ADAPT
)

build_component()
