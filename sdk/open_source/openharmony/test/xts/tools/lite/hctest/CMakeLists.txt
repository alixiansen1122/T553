#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hctest")

set(SOURCES
    ${ROOT_DIR}/open_source/openharmony//third_party/cJSON/tests/unity/src/unity.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hctest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hctest_service.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON/tests/unity/src
    ${ROOT_DIR}/application/wearable/xts_adapt
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
    ${ROOT_DIR}/open_source/openharmony/base/startup/bootstrap_lite/source
    ${ROOT_DIR}/open_source/openharmony/base/startup/syspara_lite/interfaces/kits
    ${ROOT_DIR}/open_source/openharmony/base/startup/syspara_lite/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
    -Wno-pointer-integer-compare
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
