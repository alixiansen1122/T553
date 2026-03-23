#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
enable_language(C)
set(COMPONENT_NAME "audio_recorder")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_recorder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/muxer/src/audio_recorder_muxer.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../audio_recorder/interfaces
)

set(OPENMG_DIR ${ROOT_DIR}/open_source/openharmony)
set(PRIVATE_HEADER_COM ${PRIVATE_HEADER})
list(APPEND PRIVATE_HEADER_COM
    ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/muxer/include
)

if(${CHIP} MATCHES "socmn2")
    set(CDEFINES ${CDEFINES} "SUPPORT_FEATURE_CONFIG" "SUPPORT_SEA_CONFIG" "SUPPORT_ERROR_CODE" "SUPPORT_AUDIO_STREAM_IN_C")
endif()
if ("${CHIP}" STREQUAL "brandy")
    list(APPEND PRIVATE_HEADER_COM
        ${OPENMG_DIR}/drivers/peripheral/format/interfaces/include
    )
endif()
if(${CHIP} MATCHES "socmn2|3322")
    set(CDEFINES ${CDEFINES} "SUPPORT_NEW_FORMAT")
endif()

set(PRIVATE_HEADER
   ${PRIVATE_HEADER_COM}
)


set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(NEWDEFINES "SUPPORT_AUDIO_RECORDER")
set(PUBLIC_DEFINES
    ${NEWDEFINES}
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${COMPONENT_NAME})

build_component()