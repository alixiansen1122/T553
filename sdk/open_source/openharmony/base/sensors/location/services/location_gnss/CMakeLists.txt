set(COMPONENT_NAME "location_gnss")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/location/utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../drivers/peripheral/location/interface/gnss
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../drivers/peripheral/location/interface/agnss
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/include
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/include/ntp
    ${CMAKE_CURRENT_SOURCE_DIR}/../location_locator/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/agnss_event_callback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/agnss_ni_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/gnss_ability.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/gnss_common_event_subscriber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/gnss_event_callback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/ntp/elapsed_real_time_check.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/ntp/gps_time_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/ntp/net_conn_observer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/ntp/ntp_time_check.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/ntp/ntp_time_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/ntp/time_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss/source/string_utils.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)


set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
)

set(CDEFINES ${CDEFINES} "FEATURE_GNSS_SUPPORT" "SW_UART_DEBUG")

set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
