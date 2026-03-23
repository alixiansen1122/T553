set(COMPONENT_NAME "location_locator")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/location/utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../drivers/peripheral/location/interface/gnss
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../drivers/peripheral/location/interface/agnss
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../location_gnss/gnss/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/fusion_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/location_config_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator_ability.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator_event_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator_event_subscriber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator_skeleton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/permission_status_change_cb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/report_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/request_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/subability_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/work_record.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/work_record_statistic.cpp
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

set(AUTO_ADD_PATH
    false
)

set(PUBLIC_DEFINES
)

set(CDEFINES ${CDEFINES} "FEATURE_GNSS_SUPPORT" "SW_UART_DEBUG")

set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
