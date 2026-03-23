#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(MODULE_NAME "dsp")
set(AUTO_DEF_FILE_ID TRUE)
if(NOT "audio_drv" IN_LIST TARGET_COMPONENT)
    return()
endif()

if("${CHIP}" STREQUAL "socmn1")
    set(PROJECT_CHIP_DIR melody)
else()
    set(PROJECT_CHIP_DIR ${CHIP})
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/source/CMakeLists.txt)
    set(AUDI_PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_subdirectory(source)
else()
    set(PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set(AUDIO_LIST "audio_api" "audio_core" "audio_drv" "audio_sample" "audio_vendor")

    set(COMPONENT_LIST ${AUDIO_LIST})
    set(AUDIO_LIB_LIST)

    set(SAP_BOARD ${BOARD})
    # Switch audio libs dir for brandy evb4
    if("BRANDY_PRODUCT_EVB4" IN_LIST DEFINES)
        string(APPEND SAP_BOARD "4")
    endif()

    if(DEFINED CONFIG_LOAD_FROM_FS)
        set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/audio/${SAP_BOARD}_${APPLICATION}/${OS}/fs)
    else()
        set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/audio/${SAP_BOARD}_${APPLICATION}/${OS})
    endif()
    foreach(COMPONENT ${COMPONENT_LIST})
        set(COMPONENT_NAME ${COMPONENT})
        if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
            continue()
        endif()
        find_library(LIBS${COMPONENT_NAME} "lib${COMPONENT_NAME}.a" ${LIBRARY_OUTPUT_PATH})
        set(AUDIO_LIB_LIST ${AUDIO_LIB_LIST} ${LIBS${COMPONENT_NAME}})
        build_component()
    endforeach()

    target_link_libraries(${TARGET_NAME} PRIVATE -Wl,--whole-archive  ${AUDIO_LIB_LIST} -Wl,--no-whole-archive )
endif()
