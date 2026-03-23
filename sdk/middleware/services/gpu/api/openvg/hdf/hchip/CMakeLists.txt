#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    hchip_list
        ${CMAKE_CURRENT_SOURCE_DIR}/hdi_gfx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hdi_hardware_capability.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${hchip_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    hchip_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ROOT_DIR}/middleware/services/gpu/api/rm/inc
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${hchip_header_list}" CACHE INTERNAL "platform src obj list" FORCE)