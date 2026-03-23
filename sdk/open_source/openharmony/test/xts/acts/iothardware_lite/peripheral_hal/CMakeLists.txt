#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ActsWifiIotTest")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_flash_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_gpio_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_i2c_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_pwm_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_uart_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_watchdog_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lowpower_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reset_test.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
    ${ROOT_DIR}/open_source/openharmony/test/xts/tools/lite/hctest/include
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON/tests/unity/src
    ${ROOT_DIR}/application/wearable/xts_adapt
    ${ROOT_DIR}/open_source/openharmony//iothardware/peripheral/interfaces/kits
)

set(PRIVATE_DEFINES
    CONFIG_PWM_SUPPORT
    CONFIG_I2C_SUPPORT
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
