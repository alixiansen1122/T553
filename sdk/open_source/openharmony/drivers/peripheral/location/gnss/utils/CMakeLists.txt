set(COMPONENT_NAME "location_gnss_utils")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interface/gnss
    ${CMAKE_CURRENT_SOURCE_DIR}/../hdi_service/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gnss_nmea_parse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gnss_nmea_str_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gnss_utils.cpp
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

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "GNSS_DUMP_NMEA_SUPPORT")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
