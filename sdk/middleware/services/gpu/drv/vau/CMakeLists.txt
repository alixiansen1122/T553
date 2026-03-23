#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    vau_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx/vau_dfx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/intf/drv_vau_intf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/intf/vau_pre_process.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl/vau_ioctl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/isr/vau_isr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/job/vau_job.c
        ${CMAKE_CURRENT_SOURCE_DIR}/list/vau_list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/list/vau_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mem/vau_mem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/node/vau_node.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/vau_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/vau_hal_apb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/vau_hal_node.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/vau_hal_affine.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/vau_hal_capability.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/vau_hal_vector_draw.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${vau_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    vau_drv_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/hal
        ${CMAKE_CURRENT_SOURCE_DIR}/intf
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl
        ${CMAKE_CURRENT_SOURCE_DIR}/isr
        ${CMAKE_CURRENT_SOURCE_DIR}/job
        ${CMAKE_CURRENT_SOURCE_DIR}/list
        ${CMAKE_CURRENT_SOURCE_DIR}/mem
        ${CMAKE_CURRENT_SOURCE_DIR}/node
        ${ROOT_DIR}/middleware/services/gpu/api/rm/gmmu/drv/include
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${vau_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)