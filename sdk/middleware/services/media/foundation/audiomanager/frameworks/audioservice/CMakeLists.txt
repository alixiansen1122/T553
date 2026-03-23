#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

enable_language(C)

if(NOT "${OS_TYPE}" STREQUAL "Linux")
    set(COMPONENT_NAME "audio_service")

    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_service.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src/device_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src/device_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src/device_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/devices_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/interrupt_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/policy_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/policy_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/source_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/stream_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/stream_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/track_handle.c
    )
    if ("${CHIP}" STREQUAL "brandy")
        set(SOURCES_TEMP ${SOURCES})
        list(APPEND SOURCES_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/hifi_clock_freq_manager.c)
        set(SOURCES
            ${SOURCES_TEMP}
        )
    endif()
    
    set(PUBLIC_HEADER
    )

    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../audioutils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${ROOTDIR}/open_source/openharmony/drivers/peripheral/audio/interfaces/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/include
        ${ROOTDIR}/include/middleware/utils
        ${ROOTDIR}/middleware/utils/nv/nv_storage_lib

    )

    set(PRIVATE_HEADER_TEMP ${PRIVATE_HEADER})
    if ("${CHIP}" STREQUAL "brandy")
        list(APPEND PRIVATE_HEADER_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/brandy)
    elseif ("${CHIP}" STREQUAL "socmn1")
        list(APPEND PRIVATE_HEADER_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/melody)
    endif()
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER_TEMP}
    )

    set(CDEFINES ${CDEFINES} "SUPPORT_NV")
    if ("${CHIP}" STREQUAL "brandy")
        set(CDEFINES ${CDEFINES} "SUPPORT_SET_DSP_CLOCK_FREQ")
    endif()
    set(PRIVATE_DEFINES
        ${CDEFINES}
    )
    set(PUBLIC_DEFINES
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    set(COMPONENT_CCFLAGS
    )

    set(WHOLE_LINK
        true
    )

    set(MAIN_COMPONENT
        false
    )

    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION})

    build_component()
else()
    set(COMPONENT_NAME audio_service)
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../..)
    include(../../../../../device/hisilicon/hardware/media/build/build_env.cmake)
    include(../../../../../device/hisilicon/hardware/media/build/build_option.cmake)
    aux_source_directory(src SOURCES)
    aux_source_directory(src/devicemanager/src DEVICE_MGR)
    aux_source_directory(src/effectmanager/src EFFECT_MGR)
    aux_source_directory(src/policymanager/src POLICY_MGR)
    aux_source_directory(src/streammanager/src STREAM_MGR)
    aux_source_directory(src/volumemanager/src VOLUME_MGR)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    add_library(${COMPONENT_NAME} STATIC ${SOURCES} ${DEVICE_MGR} ${EFFECT_MGR} ${POLICY_MGR} ${STREAM_MGR} ${VOLUME_MGR})
    target_include_directories(${COMPONENT_NAME} PRIVATE
        ${SECUREC_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/effectmanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/effectmanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/volumemanager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/volumemanager/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../audioutils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../drivers/peripheral/audio/interfaces/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/include
    )
endif()
