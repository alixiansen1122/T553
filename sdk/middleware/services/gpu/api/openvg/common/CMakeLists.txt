#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    openvg_common_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${openvg_common_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
