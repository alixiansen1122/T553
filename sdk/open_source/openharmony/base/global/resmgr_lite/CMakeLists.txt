set(COMPONENT_NAME "resmgr")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/resmgr_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/resmgr_lite/src/global_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/resmgr_lite/src/global.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

build_component()
