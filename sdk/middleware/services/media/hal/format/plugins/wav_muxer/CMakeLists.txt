#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "plugin_muxer_wav")

# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/wav_muxer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wav.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/dtcf/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/dtcf_manager/include
)

if ("${CHIP}" STREQUAL "brandy")
    set(CDEFINES ${CDEFINES} "MINI_MEMORY_SUPPORT")
endif()
set(PRIVATE_DEFINES
)

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "ENABLE_PLUGIN_MUXER_WAV")
set(PUBLIC_DEFINES
    ${CDEFINES}
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

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS}/${COMPONENT_NAME})

build_component()