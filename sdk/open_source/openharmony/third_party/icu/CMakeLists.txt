#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOTDIR})
set(COMPONENT_ROOT_DIR ${PROJECT_ROOT_DIR}/open_source/openharmony/third_party/icu/icu4c/source)

#set src
set(COMPONENT_NAME "icu4c")

return_if_not_build()

set(COMPONENT_SRC
    ${COMPONENT_ROOT_DIR}/common/cmemory.cpp
    ${COMPONENT_ROOT_DIR}/common/cstring.cpp
    ${COMPONENT_ROOT_DIR}/common/ubidi.cpp
    ${COMPONENT_ROOT_DIR}/common/ubidi_props.cpp
    ${COMPONENT_ROOT_DIR}/common/ubidiln.cpp
    ${COMPONENT_ROOT_DIR}/common/ubidiwrt.cpp
    ${COMPONENT_ROOT_DIR}/common/uchar.cpp
    ${COMPONENT_ROOT_DIR}/common/udataswp.cpp
    ${COMPONENT_ROOT_DIR}/common/uinvchar.cpp
    ${COMPONENT_ROOT_DIR}/common/ustring.cpp
    ${COMPONENT_ROOT_DIR}/common/utf_impl.cpp
    ${COMPONENT_ROOT_DIR}/common/utrie2.cpp
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${COMPONENT_ROOT_DIR}/common
)
set(CCDEFINES ${defines})
set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
)

if(${ARCH} STREQUAL "cortex_m7")
    list(REMOVE_ITEM COMPONENT_CCFLAGS -nostdlibinc)
    list(APPEND COMPONENT_CCFLAGS -nostdinc)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
