#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ActsHuksHalFunctionTest")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_aes_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_agreement_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_bn_exp_mod_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_cipher_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_curve25519_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_delete_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_derive_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_exist_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_generate_key_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_generate_random_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_hash_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_mac_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_others_test.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
    ${ROOT_DIR}/open_source/openharmony/base/security/huks/interfaces/innerkits/huks_standard/main/include
    ${ROOT_DIR}/open_source/openharmony/base/iothardware/peripheral/interfaces/inner_api
    ${ROOT_DIR}/open_source/openharmony/base/security/huks/frameworks/huks_standard/main/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${ROOT_DIR}/open_source/openharmony/test/xts/tools/lite/hctest/include
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON/tests/unity/src
    ${ROOT_DIR}/application/wearable/xts_adapt
)

set(PRIVATE_DEFINES
    _CUT_ED25519_
    _CUT_LOG_
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-strict-prototypes
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
