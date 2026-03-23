#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(ALIPAY_SDK_DIR ${ROOT_DIR}/vendor/alipay_sdk/alipay_sec_sdk_v3.0.0)

add_subdirectory_if_exist(driver_adapt)
add_subdirectory_if_exist(service)