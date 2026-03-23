#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "ai_voice_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/smart_voice_service.c
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/native_c/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../voice_engine_hal/interfaces/innerkits/native_c/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiocommon
)

set(PUBLIC_HEADER
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()


set(COMPONENT_NAME   "ai_voice_audio_stream_wrapper")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/audio_stream.cpp
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiocommon
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiomanager
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/frameworks/audiostream/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11 
    -Wno-unused-parameter 
    -fno-exceptions 
    -fno-unwind-tables 
    -fno-asynchronous-unwind-tables 
    -Wno-undef 
    -Wno-bitwise-op-parentheses 
    -Wno-shift-op-parentheses 
    -Wno-c++11-narrowing 
    -nostdinc
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
