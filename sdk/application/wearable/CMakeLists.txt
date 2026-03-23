#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(nativeabilityfwk)
add_subdirectory_if_exist(nativeapp)
add_subdirectory_if_exist(ohos_deps)
add_subdirectory_if_exist(service)
add_subdirectory_if_exist(ohos_startup)
add_subdirectory_if_exist(xts_adapt)
add_subdirectory_if_exist(alipay)
if("SUPPORT_AUTO_OTA" IN_LIST DEFINES)
    add_subdirectory_if_exist(auto_ota)
endif()
add_subdirectory_if_exist(dsoftbus_deps)
add_subdirectory_with_alias_if_exist(SystemView ${PROJECT_BINARY_DIR}/systemview)
add_subdirectory_if_exist(svr_dynload)