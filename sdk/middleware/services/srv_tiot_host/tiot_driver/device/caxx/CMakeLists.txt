#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_DEV_CAXX)
set(
    device_include_list
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(
    device_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/caxx_device_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/caxx_service.c
)

if(DEFINED CONFIG_FILE_BY_ARRAY)
if(NOT UT_TIOT_RUNNING)
list(
    APPEND device_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/firmware/caxx_fw_file_array.c
)

list(
    APPEND device_include_list
    "${CMAKE_CURRENT_SOURCE_DIR}/firmware"
)
endif()
endif()

set(TIOT_HEADER_LIST "${TIOT_HEADER_LIST}" "${device_include_list}" CACHE INTERNAL "tiot header list" FORCE)
set(TIOT_SRC_LIST "${TIOT_SRC_LIST}" "${device_src_list}" CACHE INTERNAL "tiot src list" FORCE)
endif()
