#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "codec_hw")

add_subdirectory(plugins/brandy)

# use build env and option camke
include(../../build/build_env.cmake)
include(../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec_v2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer_channel/buffer_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec_config/codec_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/codec_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pluginManager/codec_plugin_manager.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer_channel
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pluginManager
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pluginManager/codec_plugin_base
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec_config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/codec/interfaces/include
)

set(CDEFINES ${CDEFINES} "NO_NEED_MPI_SYS_INIT")
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
