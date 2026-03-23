#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "sensor_sample")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sensor_sample.cpp
)

set(PUBLIC_HEADER
)


set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/open_source/openharmony/base/sensors/interfaces/kits/native/include
)

set(PUBLIC_DEFINES
    SUPPORT_AT_SENSOR
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_FLAGS)
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

build_component()
