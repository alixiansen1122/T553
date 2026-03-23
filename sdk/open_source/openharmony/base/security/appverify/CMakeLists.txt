set(COMPONENT_NAME "appverify")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_provision.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_mbedtls_pkcs7.c
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
    MBEDTLS_ALLOW_PRIVATE_ACCESS
)

set(AUTO_ADD_PATH
    false
)

build_component()
