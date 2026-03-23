#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "plugin_demuxer_ffmpeg")

set(COMPONENT_CCFLAGS  -march=rv32imafc -mabi=ilp32f)
set(FFMPEG_ROOT_DIR ${ROOT_DIR}/middleware/services/media/component/ffmpeg_adp/ffmpeg-5.1.6)

# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ff_demuxer_plugin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ff_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ff_wrapper.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
    ${FFMPEG_ROOT_DIR}/libavformat
    ${FFMPEG_ROOT_DIR}/libavcodec
    ${FFMPEG_ROOT_DIR}/libavutil
    ${FFMPEG_ROOT_DIR}
)

set(CDEFINES ${CDEFINES} "ENABLE_FFMPEG_DEMUXER")
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
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS})

build_component()