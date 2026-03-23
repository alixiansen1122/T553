#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "audio_bluetooth_port")

# use build env and option camke
include(../../../../../build/build_env.cmake)
include(../../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth_sub_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/latm_simple_muxer.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin_include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/audio/interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${ROOT_DIR}/drivers/drivers/driver/audio/include
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/services/bts/common
)

set(CDEFINES ${CDEFINES} "AUDIO_SDK_SUPPORT" "BT_HAL_SUPPORT" "MINI_MEMORY_SUPPORT")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)


set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS})

build_component()
