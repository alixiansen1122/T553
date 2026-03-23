#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "graphic_input")

set(SOURCES_LAYER ${CMAKE_CURRENT_SOURCE_DIR}/src/input_manager.c)
set(SOURCES
    ${SOURCES_LAYER}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags})
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-incompatible-function-pointer-types
    -Wno-implicit-function-declaration
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()
