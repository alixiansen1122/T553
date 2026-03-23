#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    egl_list
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_display.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_egl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/egl_surface.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${egl_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    egl_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${egl_header_list}" CACHE INTERNAL "platform src obj list" FORCE)