#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "huks_test_common")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_api_performance.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_cipher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_curve25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_file_operator.c
    #${CMAKE_CURRENT_SOURCE_DIR}/hks_test_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hks_test_mem.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/open_source/openharmony//base/security/huks/interfaces/innerkits/huks_standard/main/include
    ${ROOT_DIR}/open_source/openharmony/test/xts/tools/lite/hctest/include
)

set(PRIVATE_DEFINES
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

set(AUTO_ADD_PATH
    false
)

build_component()
