#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved.
#===============================================================================
if(RAM_COMPONENT_SET MATCHES "media")
set(COMPONENT_NAME "ogg")
set(LIBOGG_ROOT_DIR ${ROOT_DIR}/open_source/libogg/libogg-1.3.5)

if (NOT EXISTS ${LIBOGG_ROOT_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${ROOT_DIR}/open_source/libogg/libogg-1.3.5.tar.xz
        WORKING_DIRECTORY ${ROOT_DIR}/open_source/libogg/
    )
endif()


include(CheckIncludeFiles)
include(CMakePackageConfigHelpers)


# Configure config_type.h
check_include_files(inttypes.h INCLUDE_INTTYPES_H)
check_include_files(stdint.h INCLUDE_STDINT_H)
check_include_files(sys/types.h INCLUDE_SYS_TYPES_H)

list(APPEND CMAKE_MODULE_PATH "${LIBOGG_ROOT_DIR}/cmake")

set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)
set(USIZE64 uint64_t)

set(CONFIG_TYPE_HEADER ${LIBOGG_ROOT_DIR}/include/ogg/config_types.h)
if (NOT EXISTS ${CONFIG_TYPE_HEADER})
configure_file(${LIBOGG_ROOT_DIR}/include/ogg/config_types.h.in ${LIBOGG_ROOT_DIR}/include/ogg/config_types.h @ONLY)
endif()

set(OGG_HEADERS
    ${LIBOGG_ROOT_DIR}/include/ogg/
    ${LIBOGG_ROOT_DIR}/include/
)

set(OGG_SOURCES
    ${LIBOGG_ROOT_DIR}/src/bitwise.c
    ${LIBOGG_ROOT_DIR}/src/framing.c
    ${LIBOGG_ROOT_DIR}/src/crctable.h
)

set(SOURCES
    ${OGG_SOURCES}
)

set(PRIVATE_HEADER
    ${OGG_HEADERS}
)

build_component()
endif()