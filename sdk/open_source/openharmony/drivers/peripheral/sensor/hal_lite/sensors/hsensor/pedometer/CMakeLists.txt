set(COMPONENT_NAME "sensor_hdi_pedometer")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../sensor_mgr/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pedometer.c
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

set(AUTO_ADD_PATH
    false
)

set(PUBLIC_DEFINES
)

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
