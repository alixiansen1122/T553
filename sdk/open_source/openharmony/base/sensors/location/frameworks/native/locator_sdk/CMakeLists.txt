set(COMPONENT_NAME "locator_sdk")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/location/utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../services/location_locator/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../services/utils/include
)

if("__LITEOS__" IN_LIST DEFINES)
elseif ("__FREERTOS__" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${ROOT_DIR}/middleware/services/utils/fs/include/freertos_adapt
    )
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/locator_impl.cpp
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

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "SW_UART_DEBUG")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
