#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOTDIR})
set(LibjpegSrc ${PROJECT_ROOT_DIR}/open_source/openharmony/third_party/libjpeg)

#set src
set(COMPONENT_NAME "jpeg_static")

return_if_not_build()

set(COMPONENT_SRC
    ${LibjpegSrc}/cdjpeg.c
    ${LibjpegSrc}/jaricom.c
    ${LibjpegSrc}/jcapimin.c
    ${LibjpegSrc}/jcapistd.c
    ${LibjpegSrc}/jcarith.c
    ${LibjpegSrc}/jccoefct.c
    ${LibjpegSrc}/jccolor.c
    ${LibjpegSrc}/jcdctmgr.c
    ${LibjpegSrc}/jchuff.c
    ${LibjpegSrc}/jcinit.c
    ${LibjpegSrc}/jcmainct.c
    ${LibjpegSrc}/jcmarker.c
    ${LibjpegSrc}/jcmaster.c
    ${LibjpegSrc}/jcomapi.c
    ${LibjpegSrc}/jcparam.c
    ${LibjpegSrc}/jcprepct.c
    ${LibjpegSrc}/jcsample.c
    ${LibjpegSrc}/jctrans.c
    ${LibjpegSrc}/jdapimin.c
    ${LibjpegSrc}/jdapistd.c
    ${LibjpegSrc}/jdarith.c
    ${LibjpegSrc}/jdatadst.c
    ${LibjpegSrc}/jdatasrc.c
    ${LibjpegSrc}/jdcoefct.c
    ${LibjpegSrc}/jdcolor.c
    ${LibjpegSrc}/jddctmgr.c
    ${LibjpegSrc}/jdhuff.c
    ${LibjpegSrc}/jdinput.c
    ${LibjpegSrc}/jdmainct.c
    ${LibjpegSrc}/jdmarker.c
    ${LibjpegSrc}/jdmaster.c
    ${LibjpegSrc}/jdmerge.c
    ${LibjpegSrc}/jdpostct.c
    ${LibjpegSrc}/jdsample.c
    ${LibjpegSrc}/jdtrans.c
    ${LibjpegSrc}/jerror.c
    ${LibjpegSrc}/jfdctflt.c
    ${LibjpegSrc}/jfdctfst.c
    ${LibjpegSrc}/jfdctint.c
    ${LibjpegSrc}/jidctflt.c
    ${LibjpegSrc}/jidctfst.c
    ${LibjpegSrc}/jidctint.c
    ${LibjpegSrc}/jmemnobs.c
    ${LibjpegSrc}/jmemmgr.c
    ${LibjpegSrc}/jquant1.c
    ${LibjpegSrc}/jquant2.c
    ${LibjpegSrc}/jutils.c
    ${LibjpegSrc}/rdbmp.c
    ${LibjpegSrc}/rdcolmap.c
    ${LibjpegSrc}/rdgif.c
    ${LibjpegSrc}/rdppm.c
    ${LibjpegSrc}/rdrle.c
    ${LibjpegSrc}/rdswitch.c
    ${LibjpegSrc}/rdtarga.c
    ${LibjpegSrc}/transupp.c
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${LibjpegSrc}
)
set(PRIVATE_DEFINES
    TRUE
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
