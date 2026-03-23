#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    gpu_mem_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/gpu_mem_config.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${gpu_mem_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    drv_comm_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${drv_comm_header_list}" CACHE INTERNAL "platform src obj list" FORCE)