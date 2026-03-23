set(COMPONENT_NAME "bootstrap")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/bootstrap_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/system_init.c
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
