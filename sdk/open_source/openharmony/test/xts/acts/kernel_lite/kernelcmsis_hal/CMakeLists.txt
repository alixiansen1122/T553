#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ActsCMSISTest")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_event_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_msg_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_mutex_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_sem_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_task_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_task_pri_func_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmsis_timer_func_test.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
