set(COMPONENT_NAME "ohos_deps")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/service/device_msg
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/phony_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ohos_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wearable_onlinelog.c
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
