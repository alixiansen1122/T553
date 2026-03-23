#[[
Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
Description: CMake audio sample module.
Author: audio
Create: 2023-06-09
]]

set(SAP_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../drivers/drivers/driver/audio/source)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_config.cmake)
#===================================================================================
set(COMPONENT_NAME audio_vendor)
set(MODULE_NAME "dsp")
set(AUTO_DEF_FILE_ID TRUE)
#===================================================================================
set(COMPONENT_SRC )
#===================================================================================
set(COMPONENT_INC
    ${SAP_UAPI_DIR}
    ${SAP_TOP_DIR}/include
    ${SAP_TOP_DIR}/drv/vendor
    ${SAP_TOP_DIR}/drv/include
    ${SAP_TOP_DIR}/drv/common/include
    ${SAP_TOP_DIR}/drv/arch/include
    ${SAP_TOP_DIR}/drv/arch/${CFG_SAP_CHIP_TYPE}/include
    ${SAP_TOP_DIR}/core/include
    ${SAP_TOP_DIR}/core/common/include
    ${SAP_TOP_DIR}/core/arch/${CFG_SAP_CHIP_TYPE}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
#===================================================================================
if("${CFG_SAP_VENDOR_DRIVER_AW88166_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw88166.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw883xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw883xx_calib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw883xx_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/product_init_files/aw88166/aw883xx_pid_2066_init.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/product_init_files/aw88166
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/config/aw88166/mono/16bit
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_ANC_CODEC_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/anc_codec/anc_codec.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/anc_codec
    ${CMAKE_CURRENT_SOURCE_DIR}/anc_codec/sai01_dac_32.768k
)
endif()
#===================================================================================
set(COMPONENT_DEF
    "SAP_PROC_SUPPORT"
)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_common.cmake)
# install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/aw88166 "*")
