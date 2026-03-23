#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    jpeg_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/clock/jpeg_clock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/context/jpeg_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dev/jpeg_dev.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx/jpeg_dfx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/jpeg_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/intf/drv_jpeg_intf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl/jpeg_ioctl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/isr/jpeg_isr.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${jpeg_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    jpeg_drv_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/clock
        ${CMAKE_CURRENT_SOURCE_DIR}/context
        ${CMAKE_CURRENT_SOURCE_DIR}/dev
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/hal
        ${CMAKE_CURRENT_SOURCE_DIR}/intf
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl
        ${CMAKE_CURRENT_SOURCE_DIR}/isr
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${jpeg_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)