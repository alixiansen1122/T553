set(COMPONENT_NAME "location_utils")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/location_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/location_thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/location_dumper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/location_log.cpp
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

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "ENABLE_INFO_LEVEL_LOG")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
