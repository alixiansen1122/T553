set(COMPONENT_NAME "broadcast")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/dfx
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if("BROADCAST_SUPPORT_COMMU_SERVICE" IN_LIST DEFINES)
    list(APPEND PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/communication/include
    )
endif()

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ohos_deps
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/broadcast_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/broadcast_implement.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/broadcast_service.c
)

if("BROADCAST_SUPPORT_COMMU_SERVICE" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/communication/src/commu_feature.c
        ${CMAKE_CURRENT_SOURCE_DIR}/communication/src/commu_service.c
    )
endif()

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

