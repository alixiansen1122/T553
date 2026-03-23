#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)

set(COMPONENT_NAME "graphic_service")

set(COMPONENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphic_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphic_event_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphic_service_wrapper.cpp
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/proprietary/include/service
)

set(PRIVATE_HEADER
    ${PROJECT_ROOT_DIR}/hal/input/include
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOTDIR}/middleware/services/gpu/drv/include
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-missing-field-initializers
    -Wno-unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
