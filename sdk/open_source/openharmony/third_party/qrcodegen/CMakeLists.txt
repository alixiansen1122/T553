#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOTDIR})
set(FREETYPE_ROOT_DIR ${PROJECT_ROOT_DIR}/open_source/openharmony/third_party/qrcodegen)

set(COMPONENT_NAME "qrcodegen")

return_if_not_build()

set(COMPONENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/QrCode.cpp
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
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
    -Wno-sometimes-uninitialized
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
