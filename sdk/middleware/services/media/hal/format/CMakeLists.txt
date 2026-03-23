#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "format_hw")

set(HMF_DMX_DIR ${ROOT_DIR}/middleware/services/media/component/hmf_demuxer)
set(FFMPEG_DIR ${ROOT_DIR}/middleware/services/media/component/ffmpeg_adp)
if (EXISTS ${COMPONENT_DIR})
    add_subdirectory(plugins/hmf_demuxer)
    add_subdirectory(plugins/protocol_https)
    add_subdirectory(plugins/mjpeg_demuxer)
endif()
if (EXISTS ${FFMPEG_DIR})
    add_subdirectory(plugins/ffmpeg_demuxer)
endif()
add_subdirectory(plugins/raw_demuxer)
add_subdirectory(plugins/raw_muxer)
add_subdirectory(plugins/wav_muxer)
add_subdirectory(plugins/ogg_muxer)
add_subdirectory(plugins/common/dtcf)
add_subdirectory(plugins/common/dtcf_manager)
add_subdirectory(src/decrypt)
# use build env and option camke
include(../../build/build_env.cmake)
include(../../build/build_option.cmake)

if (NOT "JS_ENABLE" IN_LIST DEFINES)
    option(ENABLE_BUFFER_MANAGER "buffer manager" ON)
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_manager/src/demuxer_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_plugins_manager/src/demuxer_plugins_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format_common/src/format_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_manager/src/muxer_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_plugins_manager/src/muxer_plugins_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins_manager_common/src/plugins_manager_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol_plugins_manager/src/protocol_plugins_manager.c
)

if(ENABLE_BUFFER_MANAGER)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_manager/src/demuxer_buffer.c)
    set(CDEFINES ${CDEFINES} "ENABLE_BUFFER_MANAGER")
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format_common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins_manager_common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/hmf_demuxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/raw_demuxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/ffmpeg_demuxer
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/wav_muxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/raw_muxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/ogg_muxer
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/format/interfaces/include
)

 set(CDEFINES ${CDEFINES} "ENABLE_DEMUXER" "-DENABLE_MUXER" "ENABLE_PROTOCOL_PLUGIN")

 if("${CHIP}" STREQUAL "brandy")
    set(CDEFINES ${CDEFINES} "CONFIG_SUPPORT_FILESYSTEM" "PRE_ASIC")
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

set(AUTO_ADD_PATH
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS})

build_component()
