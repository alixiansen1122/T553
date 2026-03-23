#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================

set(
    vau_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/soc_vau_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/vau_draw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/vau_utility.c
        ${CMAKE_CURRENT_SOURCE_DIR}/matrix/vau_matrix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compress/vau_compress.c
        ${CMAKE_CURRENT_SOURCE_DIR}/debug/vau_debug.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${vau_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    vau_api_header_list
        ${ROOT_DIR}/middleware/services/gpu/api/rm/rm/inc/
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/draw
        ${CMAKE_CURRENT_SOURCE_DIR}/matrix
        ${CMAKE_CURRENT_SOURCE_DIR}/compress
        ${CMAKE_CURRENT_SOURCE_DIR}/debug
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${vau_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
