#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

if(NOT "${OS_TYPE}" STREQUAL "Linux")
    set(COMPONENT_NAME "player")

    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/buffersource/src/buffer_source.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/player/liteplayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/player/liteplayer_comm.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/sink/player_sync.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/sink/player_audio_sink.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/sink/player_video_sink.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/sink/player_sink_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/source/player_demux.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/source/player_source.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/decoder/decoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player_impl.cpp
    )
if("${CHIP}" STREQUAL "3322")
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/player_control/player/liteplayer.cpp)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/player_control/player/vdec_liteplayer.cpp)
endif()
    set(PUBLIC_HEADER
    )

    set(OPENMG_DIR ${ROOTDIR}/open_source/openharmony)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../player/interfaces/kits
        ${CMAKE_CURRENT_SOURCE_DIR}/buffersource/include
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/include
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/decoder
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/player
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/sink
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/source
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/audioservice/include
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/frameworks/audiostream/include
        ${ROOTDIR}/middleware/services/media/foundation/audiomanager/frameworks/audioutils/include
        ${OPENMG_DIR}/drivers/peripheral/display/interfaces/include
        ${OPENMG_DIR}/drivers/peripheral/codec/interfaces/include
        ${OPENMG_DIR}/drivers/peripheral/format/interfaces/include
    )

    if("ENABLE_UIKIT" IN_LIST DEFINES)
        list(APPEND PRIVATE_HEADER
            ${ROOTDIR}/middleware/services/gui/uikit/utils/interfaces/kits
            ${ROOTDIR}/middleware/services/gui/uikit/surface/interfaces/kits
            ${ROOTDIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
            ${ROOTDIR}/middleware/services/gui/uikit/proprietary/include
        )
    elseif("ENABLE_LVGL" IN_LIST DEFINES)
        list(APPEND PRIVATE_HEADER
            ${ROOTDIR}/middleware/services/gui/lvgl/config
            ${ROOTDIR}/middleware/services/gui/lvgl/proprietary/src/utils
            ${ROOTDIR}/middleware/services/gui/lvgl/proprietary/include/widgets
        )
    endif()

    set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700" "UNSUPPORT_AUDIO_DECODER" "SUPPORT_AUDIO_VIDEO_SYNC")
    if ("${CHIP}" STREQUAL "brandy" OR "${CHIP}" STREQUAL "3322")
        if("MEMORY_MINI" IN_LIST DEFINES)
            set(CCDEFINES ${CCDEFINES} "UNSUPPORT_VIDEO_DECODER")
        endif()
        set(CCDEFINES ${CCDEFINES} "MINI_MEMORY_SUPPORT")
    endif()
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
    set(COMPONENT_NAME player)
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..)
    set(SECUREC_INCLUDE ${PROJECT_ROOT_DIR}/test/common/libs_ut/libc_sec/include)
    include(../../../../media/build/build_env.cmake)
    include(../../../../media/build/build_option.cmake)
    SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    add_definitions(${CCDEFINES} -DUNSUPPORT_AUDIO_DECODER)
    set(SOURCES
        player.cpp
        player_impl.cpp
        buffersource/src/buffer_source.cpp
        player_control/player/liteplayer.cpp
        player_control/player/liteplayer_comm.cpp
        player_control/sink/player_audio_sink.cpp
        player_control/sink/player_sink_manager.cpp
        player_control/source/player_demux.cpp
        player_control/source/player_source.cpp
        player_control/decoder/decoder.cpp
        player_control/sink/player_sync.cpp
        player_control/sink/player_video_sink.cpp
    )
    add_library(${COMPONENT_NAME} STATIC ${SOURCES})
    target_include_directories(${COMPONENT_NAME} PRIVATE
        ${SECUREC_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/kits
        ${CMAKE_CURRENT_SOURCE_DIR}/buffersource/include
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/include
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/decoder
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/player
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/sink
        ${CMAKE_CURRENT_SOURCE_DIR}/player_control/source
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audioservice/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audiostream/include/cpp/include
        ${PROJECT_ROOT_DIR}/middleware/services/media/foundation/audiomanager/frameworks/audiostream/include/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audioutils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../drivers/peripheral/format/interfaces/include
        ${PROJECT_ROOT_DIR}/open_source/openharmony/drivers/peripheral/codec/interfaces/include
        ${PROJECT_ROOT_DIR}/include/middleware/services/media/common
        ${PROJECT_ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager/cpp
        ${PROJECT_ROOT_DIR}/include #errcode.h
    )
endif()
