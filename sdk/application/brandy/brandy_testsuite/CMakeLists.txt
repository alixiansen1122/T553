#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "brandy_testsuite")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/input_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.S
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/version
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

if (DEFINED BUILD_TIME)
    set(PRIVATE_DEFINES
        TARGET_NAME="${TARGET_COMMAND}"
        BUILD_TIME="${BUILD_TIME}"
        SUPPORT_AT_APP
    )
else()
    set(PRIVATE_DEFINES
    )
endif()

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
    true
)

build_component()
