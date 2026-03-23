#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "audio_modem_port")

# use build env and option camke
include(../../../../../build/build_env.cmake)
include(../../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_sub_port.c
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
)

if ("${CHIP}" STREQUAL "brandy")
    set(CDEFINES ${CDEFINES} "AUDIO_SDK_SUPPORT")
endif()

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
