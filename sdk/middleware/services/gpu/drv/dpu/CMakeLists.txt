#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    dpu_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/intf/drv_dpu_intf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl/dpu_ioctl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/layer/dpu_layer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/disp/dpu_disp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/clock/dpu_clock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/isr/dpu_isr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx/dpu_dfx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/dpu_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/dpu_hal_crg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/dpu_hal_disp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/dpu_hal_layer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/qspi/dpu_qspi.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${dpu_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    dpu_drv_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/clock
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/disp
        ${CMAKE_CURRENT_SOURCE_DIR}/hal
        ${CMAKE_CURRENT_SOURCE_DIR}/intf
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl
        ${CMAKE_CURRENT_SOURCE_DIR}/isr
        ${CMAKE_CURRENT_SOURCE_DIR}/layer
        ${CMAKE_CURRENT_SOURCE_DIR}/qspi
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${dpu_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)