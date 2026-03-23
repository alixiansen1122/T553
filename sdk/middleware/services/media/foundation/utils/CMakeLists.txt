#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

if(NOT "${OS_TYPE}" STREQUAL "Linux")
    set(COMPONENT_NAME "media_common")
if(${CHIP} MATCHES "socmn1|socmn2")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_log.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_thread_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_mem.c
    )
elseif (${CHIP} MATCHES "brandy|3322")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/format.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_thread_adapt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/source.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_mem.cpp
    )
endif()
    set(PUBLIC_HEADER
    )

    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700" "CMSIS_SUPPORT")

    if(${CHIP} MATCHES "3322")
        set(CCDEFINES ${CCDEFINES} "ENABLE_INFO_LEVEL_LOG")
    endif()
    if(${CHIP} MATCHES "socmn1|socmn2")
        set(CCDEFINES ${CCDEFINES} "CMSIS_SUPPORT_JOIN")
    endif()

    if (DEFINED BUILD_TIME)
        set(CCDEFINES ${CCDEFINES} "ENABLE_INFO_LEVEL_LOG")
    endif()

    set(PRIVATE_DEFINES
        ${CCDEFINES}
    )

    set(PUBLIC_DEFINES
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )
   
    if (${CHIP} MATCHES "brandy|3322")
        set(COMPONENT_FLAGS)
        list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
        list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)
    endif()

    set(COMPONENT_CCFLAGS
        ${COMPONENT_FLAGS}
    )

    set(WHOLE_LINK
        true
    )

    set(MAIN_COMPONENT
        false
    )

    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION})

    build_component()
else()
    set(COMPONENT_NAME media_common)
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)
    set(SECUREC_INCLUDE ${PROJECT_ROOT_DIR}/test/common/libs_ut/libc_sec/include)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    include(../../../media/build/build_env.cmake)
    include(../../../media/build/build_option.cmake)
    if("${PDT_TYPE}" MATCHES "socmn1|socmn2")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_log.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_thread_adapt.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_mem.c
        )
    elseif("${PDT_TYPE}" MATCHES "brandy")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/format.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_log.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_thread_adapt.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/source.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_mem.cpp
        )
    endif()
    add_library(${COMPONENT_NAME} STATIC ${SOURCES})
    target_include_directories(${COMPONENT_NAME} PRIVATE
        ${SECUREC_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()
