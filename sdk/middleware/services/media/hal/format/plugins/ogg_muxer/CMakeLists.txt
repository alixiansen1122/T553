#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "plugin_demuxer_ogg")

# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ogg_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ogg_muxer.c
)

set(PUBLIC_HEADER
)

set(OPENMG_DIR ${ROOT_DIR}/open_source/openharmony)
set(OGG_INCLUDE ${OPENMG_DIR}/../libogg/libogg-1.3.5/include)
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
    ${OGG_INCLUDE}
    ${OGG_INCLUDE}/ogg/
)

set(PRIVATE_DEFINES
)

set(NEWCDEFINES "ENABLE_PLUGIN_MUXER_OGG")
set(PUBLIC_DEFINES
    ${NEWCDEFINES}
)

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