#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "quickjs")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libbf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libregexp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libunicode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/quickjs-libc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/quickjs.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-pointer-to-int-cast
    -Wno-sign-compare
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-missing-braces
    -Wno-macro-redefined
    -Wno-implicit-const-int-float-conversion
    -Wno-typedef-redefinition
    -Wno-atomic-alignment
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
