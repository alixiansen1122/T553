#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "media_sample_wrapper")
if ("${CHIP}" STREQUAL "brandy")
    if("MEMORY_MINI" IN_LIST DEFINES)
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/player_sample_wrapper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_manager_c_wrapper.cpp
        )
    else()
        set(SOURCE_COM ${SOURCES})
        list(APPEND SOURCE_COM 
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_capture_wrapper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_scene_interaction.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/player_sample_wrapper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/video_player_sample_wrapper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/voice_call_volte_sample_wrapper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_manager_c_wrapper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_player_service.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_player_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/audio_recorder_sample.cpp
        )
        if ("ENABLE_UIKIT" IN_LIST DEFINES)
            list(APPEND SOURCE_COM
                ${CMAKE_CURRENT_SOURCE_DIR}/video_play_wrapper.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/camera_preview_sample_wrapper.cpp
            )
        endif()
        set(SOURCES
            ${SOURCE_COM}  
        )
    endif()
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "tone_player_sample_wrapper.cpp")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/interfaces/kits/audiocommon
)

set(OPENMG_DIR ${ROOT_DIR}/open_source/openharmony)
set(OGG_INCLUDE ${OPENMG_DIR}/../libogg/libogg-1.3.5/include)
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/interfaces/kits/audiomanager
    ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audiostream/include
)

if ("${CHIP}" STREQUAL "brandy")
    set(PRIVATE_HEADER_COM ${PRIVATE_HEADER})
    list(APPEND PRIVATE_HEADER_COM ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../player/interfaces/kits
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audio_capturer/interfaces/kits
        ${ROOT_DIR}/middleware/services/media/hal/camera/include
        ${OPENMG_DIR}/drivers/peripheral/display/interfaces/include
        ${ROOT_DIR}/include/middleware/services/bts/br
        ${ROOT_DIR}/include/middleware/services/bts/common
        ${OGG_INCLUDE}
        ${OGG_INCLUDE}/ogg/
    )
    if ("ENABLE_LVGL" IN_LIST DEFINES)
        set(LVGL_DIR ${ROOT_DIR}/middleware/services/gui/lvgl)
        list(APPEND PRIVATE_HEADER_COM ${CMAKE_CURRENT_SOURCE_DIR}
            ${LVGL_DIR}
            ${LVGL_DIR}/src
            ${LVGL_DIR}/src/widgets
            ${LVGL_DIR}/config
            ${LVGL_DIR}/proprietary/service
            ${LVGL_DIR}/proprietary/src/utils
            ${LVGL_DIR}/proprietary/include/font
            ${LVGL_DIR}/proprietary/include/widgets
        )
    elseif ("ENABLE_UIKIT" IN_LIST DEFINES)
        set(UIKIT_DIR ${ROOT_DIR}/middleware/services/gui/uikit)
        list(APPEND PRIVATE_HEADER_COM ${CMAKE_CURRENT_SOURCE_DIR}
            ${UIKIT_DIR}/ui/interfaces/innerkits
            ${UIKIT_DIR}/utils/interfaces/kits
            ${UIKIT_DIR}/utils/interfaces/innerkits
            ${UIKIT_DIR}/surface/interfaces/kits
            ${UIKIT_DIR}/ui/interfaces/kits
            ${UIKIT_DIR}/proprietary/src/utils/innerkits
            ${UIKIT_DIR}/proprietary/include
        )
    endif()
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER_COM}
    )
endif()

set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700" "LOCAL_VOICE_CALL" "HMF_DECRYPT_DATA_ENABLE" "AUDIO_OGG_SUPPORT")
list(REMOVE_ITEM CCDEFINES VERSION_STANDARD)
set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
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
