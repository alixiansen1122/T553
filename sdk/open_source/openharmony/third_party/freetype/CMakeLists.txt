#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "freetype_static")

return_if_not_build()

set(COMPONENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftbase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftbbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftbitmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftdebug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftglyph.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftinit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base/ftsystem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gzip/ftgzip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sfnt/sfnt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/smooth/smooth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/truetype/truetype.c
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base
)

set(CCDEFINES ${defines} "VERSION_IOT" "_XOPEN_SOURCE=700" "FT2_BUILD_LIBRARY")
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

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
