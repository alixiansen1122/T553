set(COMPONENT_NAME "location_gnss_hdi")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
	${CMAKE_CURRENT_SOURCE_DIR}/../../interface/gnss
	${CMAKE_CURRENT_SOURCE_DIR}/../../interface/agnss
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
	${CMAKE_CURRENT_SOURCE_DIR}/source
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/location/gnss/hdi_service/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gnss_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gnss_interface_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/location_vendor_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/string_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/source/location_utils.cpp
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

set(PRIVATE_DEFINES
)

build_component()
