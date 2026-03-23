#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    openvg_list
        ${CMAKE_CURRENT_SOURCE_DIR}/src/openvg_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_draw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_image.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_math.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_matrix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_paint.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_path.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vg_resource.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vgu.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${openvg_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(OPENVG_INC_DIR "${ROOT_DIR}/open_source/OpenVG/1.1-Specification/header/linux/include")

set(
    openvg_header_list
        ${OPENVG_INC_DIR}
        ${OPENVG_INC_DIR}/VG
        ${OPENVG_INC_DIR}/EGL
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${openvg_header_list}" CACHE INTERNAL "platform src obj list" FORCE)