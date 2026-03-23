#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "audio_port_common")

# use build env and option camke
include(../../../../../build/build_env.cmake)
include(../../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin_common.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin_include
    ${CMAKE_CURRENT_SOURCE_DIR}/../primary
    ${CMAKE_CURRENT_SOURCE_DIR}/../bluetooth
    ${CMAKE_CURRENT_SOURCE_DIR}/../modem
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/audio/interfaces/include
    ${ROOT_DIR}/drivers/drivers/driver/audio/include
)

if("${CHIP}" STREQUAL "socmn1")
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/middleware/utils/nv/nv_storage_lib
    )
elseif("${CHIP}" STREQUAL "brandy")
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/middleware/chips/brandy/nv/nv_config/include
    )
endif()

set(CDEFINES ${CDEFINES} "AUDIO_SDK_SUPPORT" "AUDIO_SEA_SUPPORT" "SUPPORT_NV")

if ("${CHIP}" STREQUAL "brandy" AND NOT "MEMORY_MINI" IN_LIST DEFINES)
    set(CDEFINES ${CDEFINES} "SUPPORT_MODEM")
endif()

if("${CHIP}" STREQUAL "socmn1")
    set(CDEFINES ${CDEFINES} "MELODY_AUDIO_SUPPORT")
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
