#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ActsSamgrTest")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/broadcast_publish_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/broadcast_subscribe_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/defaultfeatureapi_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/feature_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/featureapi_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iunknown_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sendrequest_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sendresponse_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sendsharerequest_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskpool_notask_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskpool_sharedtask_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskpool_singletask_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskpool_specifiedtask_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/samgr_maintenance.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/communication/broadcast
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/services/samgr/adapter
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/services/samgr/source
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${ROOT_DIR}/open_source/openharmony/test/xts/tools/lite/hctest/include
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON/tests/unity/src
    ${ROOT_DIR}/application/wearable/xts_adapt
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
