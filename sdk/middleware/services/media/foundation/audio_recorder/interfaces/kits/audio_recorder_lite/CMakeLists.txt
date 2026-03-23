#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

set(COMPONENT_NAME "js_audio_recorder")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_recorder_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_recorder_listener.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(OPENMG_DIR ${ROOTDIR}/open_source/openharmony)
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../utils/include
    ${OPENMG_DIR}/utils/native/lite/js/builtin/filekit/include
    ${OPENMG_DIR}/utils/native/lite/js/builtin/common/include
)

set(PRIVATE_DEFINES
    
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

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION})

build_component()