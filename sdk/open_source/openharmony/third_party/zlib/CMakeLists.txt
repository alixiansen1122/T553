#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOTDIR})
set(LibZlibSrc ${PROJECT_ROOT_DIR}/open_source/openharmony/third_party/zlib)

#set src
set(COMPONENT_NAME "zlib_static")

return_if_not_build()

set(COMPONENT_SRC
    ${LibZlibSrc}/adler32.c
    ${LibZlibSrc}/compress.c
    ${LibZlibSrc}/contrib/minizip/ioapi.c
    ${LibZlibSrc}/contrib/minizip/unzip.c
    ${LibZlibSrc}/contrib/minizip/zip.c
    ${LibZlibSrc}/crc32.c
    ${LibZlibSrc}/deflate.c
    ${LibZlibSrc}/gzclose.c
    ${LibZlibSrc}/gzlib.c
    ${LibZlibSrc}/gzread.c
    ${LibZlibSrc}/gzwrite.c
    ${LibZlibSrc}/infback.c
    ${LibZlibSrc}/inffast.c
    ${LibZlibSrc}/inflate.c
    ${LibZlibSrc}/inftrees.c
    ${LibZlibSrc}/trees.c
    ${LibZlibSrc}/uncompr.c
    ${LibZlibSrc}/zutil.c
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${LibZlibSrc}
)
set(PRIVATE_DEFINES
)


set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags})
list(APPEND CFLAG -Wno-error -Wno-unused-parameter)
set(COMPONENT_CCFLAGS
    ${CFLAG}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
