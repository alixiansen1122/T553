#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "esim")

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/liblpaipaLib.a)
    set(LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/liblpaipaLib.a
        ${CMAKE_CURRENT_SOURCE_DIR}/libcardbind.a
    )
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_bind.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_lpa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_stub.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include
)
set(PRIVATE_DEFINES
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

build_component()
