#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(broadcast)
add_subdirectory_if_exist(msg_center)
add_subdirectory_if_exist(xiaodu_navi)
add_subdirectory_if_exist(xiaodu_voice)
add_subdirectory_if_exist(xiaodu_navi_vector)
add_subdirectory_if_exist(hc_demo_msg)
add_subdirectory_if_exist(device_msg)
add_subdirectory_if_exist(remote_msg)
add_subdirectory_if_exist(auto_ota_msg)
add_subdirectory_if_exist(alipay_msg)
add_subdirectory_if_exist(app_store)
if("CONFIG_SEA_PHS_SUPPORT" IN_LIST DEFINES)
    add_subdirectory_if_exist(smart_voice)
endif()
add_subdirectory_if_exist(power_manager)
add_subdirectory_if_exist(dial_market)