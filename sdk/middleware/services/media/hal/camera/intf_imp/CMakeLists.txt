#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "hdi_camera_intf")

# use build env and option camke
include(../../../build/build_env.cmake)
include(../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_device_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_host_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/offline_stream_operator_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stream_operator_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../mm_platform/src/camera_device_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../mm_platform/src/camera_device_mng.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../mm_platform/src/stream_mng.cpp
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER_COM ${PRIVATE_HEADER})
list(APPEND PRIVATE_HEADER_COM
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mm_platform/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../hw_camera/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
)

set(PRIVATE_HEADER
    ${PRIVATE_HEADER_COM}
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_FLAGS ${ccflags})
list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)
set(COMPONENT_CCFLAGS
    ${COMPONENT_FLAGS}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS})

build_component()
