#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(C)

if(NOT "${OS_TYPE}" STREQUAL "Linux")
    set(COMPONENT_NAME "audio_utils")
if ("${CHIP}" STREQUAL "socmn1")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/am_list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_utils.c
    )
elseif ("${CHIP}" STREQUAL "brandy")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/am_list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_utils.c
    )
endif()
    set(PUBLIC_HEADER
        ${ROOTDIR}/utils/dfx/log/include
        ${ROOTDIR}/middleware/chips/brandy/dfx/include
    )

    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/include
    )

    set(PRIVATE_DEFINES
        ${CDEFINES}
    )

    set(PUBLIC_DEFINES
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    set(COMPONENT_FLAGS ${ccflags})
    list(APPEND COMPONENT_FLAGS -nostdlibinc)

    set(COMPONENT_CCFLAGS
    )

    set(WHOLE_LINK
        true
    )

    set(MAIN_COMPONENT
        false
    )

    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION})

    build_component()
else()
    set(COMPONENT_NAME audio_utils)
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../..)
    include(../../../../../device/hisilicon/hardware/media/build/build_env.cmake)
    include(../../../../../device/hisilicon/hardware/media/build/build_option.cmake)
    aux_source_directory(src SOURCES)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    add_library(${COMPONENT_NAME} STATIC ${SOURCES})
    target_include_directories(${COMPONENT_NAME} PRIVATE
        ${SECUREC_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/include
    )
endif()
