#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

if(NOT "${OS_TYPE}" STREQUAL "Linux")
    set(COMPONENT_NAME "audio_capture")

    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_capturer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_capturer_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_source/audio_stream_source/audio_source.cpp
    )

    set(PUBLIC_HEADER
    )

    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/kits
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_source/include
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_source/include/audio_stream_source
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
    )

    set(CCDEFINES ${CCDEFINES} "UNSUPPORT_AUDIO_ENCODER" "_XOPEN_SOURCE=700")
    set(PRIVATE_DEFINES
        ${CCDEFINES}
    )

    set(PUBLIC_DEFINES
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    set(COMPONENT_FLAGS ${ccflags})
    list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
    list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)
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
    set(COMPONENT_NAME audio_capture)
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..)
    include(../../../../media/build/build_env.cmake)
    include(../../../../media/build/build_option.cmake)
    add_definitions(-DUNSUPPORT_AUDIO_ENCODER)
    aux_source_directory(. SOURCES)
    aux_source_directory(./audio_source/audio_stream_source SOURCES)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    add_library(${COMPONENT_NAME} STATIC ${SOURCES})
    target_include_directories(${COMPONENT_NAME} PRIVATE
        ${SECUREC_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/kits
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_source/include
        ${CMAKE_CURRENT_SOURCE_DIR}/audio_source/include/audio_stream_source
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/interfaces/kits/audiomanager
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audiostream/include/cpp/include
        ${PROJECT_ROOT_DIR}/middleware/services/media/foundation/audiomanager/frameworks/audiostream/include/cpp
        ${PROJECT_ROOT_DIR}/include/middleware/services/media/common
        ${PROJECT_ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager/cpp
        ${PROJECT_ROOT_DIR}/include
    )
endif()
