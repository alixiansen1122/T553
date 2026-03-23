#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    dpu_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/soc_dpu_api.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${dpu_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    dpu_api_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${dpu_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)