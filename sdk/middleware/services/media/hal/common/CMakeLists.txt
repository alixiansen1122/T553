#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "media_hal_common")

# use build env and option camke
include(../../build/build_env.cmake)
include(../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media_hal_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media_hal_thread_adapt.c
)

set(PUBLIC_HEADER
)

if ("${CHIP}" STREQUAL "socmn1")
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOTDIR}/application/melody/tstp/include/platform/bsp/
)
elseif ("${CHIP}" STREQUAL "brandy")
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
endif()

if ("${CHIP}" STREQUAL "socmn1")
   set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT_JOIN")
endif()

set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS})

build_component()
