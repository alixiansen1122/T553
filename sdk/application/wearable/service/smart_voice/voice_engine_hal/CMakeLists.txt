#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "ai_voice_engine_hal")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/voice_engine_hal.c

)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/native_c/include
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
