#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_kit_cipher_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/open_source/openharmony/base/security/huks/frameworks/crypto_lite/cipher/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include
    ${ROOT_DIR}/foundation/arkui/ace_engine_lite/interfaces/jsi
    ${ROOT_DIR}/foundation/arkui/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/base/security/appverify/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_module.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    HUKS_LITEWEARABLE_SUPPORT_ENABLED
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

