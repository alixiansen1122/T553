#[[
Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
Description: CMake audio sample module.
Author: audio
Create: 2022-06-05
]]

add_subdirectory_if_exist(vendor)

set(SAP_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/drivers/driver/audio/source)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_config.cmake)
#===================================================================================
set(COMPONENT_NAME audio_sample)
set(MODULE_NAME "dsp")
set(AUTO_DEF_FILE_ID TRUE)
#===================================================================================
set(COMPONENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ai/sample_ai.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ai/sample_ai_aenc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ao/sample_ao.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_decode/sample_decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_encode/sample_encode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_encode/sample_cast_aenc_play.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_lpwk/sample_lpwk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_gui_aef/sample_gui_aef.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_sea/sample_sea.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_phone/sample_phone_apps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_dump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thread/athread_cmsis.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_audio_utils.c
)

if("${CFG_SAP_HAID_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_haid/sample_haid.c
)
endif()

if("${CFG_SAP_DPM_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_dpm/dpm_fir_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_dpm/sample_dpm.c
)
endif()

if("${CHIP}" STREQUAL "brandy")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/sample_dpm "*")
endif()

#===================================================================================
set(COMPONENT_INC
    ${SAP_UAPI_DIR}
    ${SAP_TOP_DIR}/include
    ${SAP_TOP_DIR}/drv/vendor
    ${SAP_TOP_DIR}/drv/include
    ${SAP_TOP_DIR}/drv/common/include
    ${SAP_TOP_DIR}/drv/arch/include
    ${SAP_TOP_DIR}/drv/arch/${CFG_SAP_CHIP_TYPE}/include
    ${SAP_TOP_DIR}/core/adp
    ${SAP_TOP_DIR}/core/include
    ${SAP_TOP_DIR}/core/osal/liteos
    ${SAP_TOP_DIR}/core/common/include
    ${SAP_TOP_DIR}/core/arch/${CFG_SAP_CHIP_TYPE}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/8311_adca
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_phone
)
#===================================================================================
set(COMPONENT_DEF
    "SAP_PROC_SUPPORT"
)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_common.cmake)
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/sample_dsp_overlay "*")
