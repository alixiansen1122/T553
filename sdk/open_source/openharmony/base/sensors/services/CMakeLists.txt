set(COMPONENT_NAME "sensor_service")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/base/sensors/interfaces/kits/native/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/sensor/interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if("__LITEOS__" IN_LIST DEFINES)
elseif ("__FREERTOS__" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${ROOT_DIR}/middleware/services/utils/fs/include/freertos_adapt
    )
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_service.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_service_impl.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "HAS_HDI_SENSOR_LITE_PRAT")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
