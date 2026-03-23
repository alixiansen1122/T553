#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOTDIR})
set(LibPngSrc ${PROJECT_ROOT_DIR}/open_source/openharmony/third_party/libpng)
set(LibZlibSrc ${PROJECT_ROOT_DIR}/open_source/openharmony/third_party/zlib)

#set src
set(COMPONENT_NAME "png_static")

return_if_not_build()

set(COMPONENT_SRC
    ${LibPngSrc}/png.c
    ${LibPngSrc}/pngerror.c
    ${LibPngSrc}/pngget.c
    ${LibPngSrc}/pngmem.c
    ${LibPngSrc}/pngpread.c
    ${LibPngSrc}/pngread.c
    ${LibPngSrc}/pngrio.c
    ${LibPngSrc}/pngrtran.c
    ${LibPngSrc}/pngrutil.c
    ${LibPngSrc}/pngset.c
    ${LibPngSrc}/pngtrans.c
    ${LibPngSrc}/pngwio.c
    ${LibPngSrc}/pngwrite.c
    ${LibPngSrc}/pngwtran.c
    ${LibPngSrc}/pngwutil.c
    ${LibPngSrc}/arm/arm_init.c
    ${LibPngSrc}/arm/filter_neon_intrinsics.c
    ${LibPngSrc}/arm/palette_neon_intrinsics.c
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${LibZlibSrc}
    ${LibPngSrc}
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
