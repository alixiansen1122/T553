#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

if(NOT "${OS_TYPE}" STREQUAL "Linux")
    set(COMPONENT_NAME "audio_manager")
if ("${CHIP}" STREQUAL "socmn1")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/c/audio_callback_dispatcher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/c/audio_manager.c
    )

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiomanager/c
    )
    set(CDEFINES ${CDEFINES} "_XOPEN_SOURCE=700")
    set(PRIVATE_DEFINES
        ${CDEFINES}
    )
    set(PUBLIC_DEFINES
    )
elseif ("${CHIP}" STREQUAL "brandy")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/audio_callback_dispatcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/audio_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/audio_manager_impl.cpp
    )

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiomanager/cpp
    )
    set(CCDEFINES ${CCDEFINES} "_XOPEN_SOURCE=700")
    set(PRIVATE_DEFINES
        ${CCDEFINES}
    )
    set(PUBLIC_DEFINES
        "SUPPORT_MEDIA_AUDIO_MANAGER"
    )
endif()
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../frameworks/audioutils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../frameworks/audioservice/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/include
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    if ("${CHIP}" STREQUAL "brandy")
        set(COMPONENT_FLAGS)
        list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
        list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)
    endif()
    set(COMPONENT_CCFLAGS
        ${COMPONENT_FLAGS}
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
    set(COMPONENT_NAME audio_manager)
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../..)
    include(../../../../../device/hisilicon/hardware/media/build/build_env.cmake)
    include(../../../../../device/hisilicon/hardware/media/build/build_option.cmake)
    aux_source_directory(. SOURCES)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    add_library(${COMPONENT_NAME} STATIC ${SOURCES})
    target_include_directories(${COMPONENT_NAME} PRIVATE
        ${SECUREC_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../audioutils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../audioservice/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiomanager
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/include
    )
endif()
