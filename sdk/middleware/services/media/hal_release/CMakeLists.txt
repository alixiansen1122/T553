#[[
Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
Description: module compilation script.
Author: Media Software Group
Create: 2022-05-10
]]

set(PROJECT_NAME hal_release)

set(COMPONENT_DIR ${ROOT_DIR}/middleware/services/media/component/hmf_demuxer)

if (EXISTS ${COMPONENT_DIR})
    set(DEPEND_LIBS plugin_demuxer_hmf plugin_protocol_https plugin_demuxer_mjpeg hmf_demuxer)
endif()

if("MEMORY_MINI" IN_LIST DEFINES)
    set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION}/${OS}/mini)
elseif("SUPPORT_OHOSFWK" IN_LIST DEFINES)
    set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION}/${OS}/js)
else()
    set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION}/${OS}/normal)
endif()

if (NOT EXISTS ${LIBRARY_OUTPUT_PATH})
    file(MAKE_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
endif()

if("MEMORY_MINI" IN_LIST DEFINES)
    set(FORMAT_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/format/libs/mini)
elseif("SUPPORT_OHOSFWK" IN_LIST DEFINES)
    set(FORMAT_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/format/libs/js)
else()
    set(FORMAT_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/format/libs/normal)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../hal)
    add_custom_command(
        OUTPUT install_hal_release
        COMMENT "Install hal_release ..."
        DEPENDS ${DEPEND_LIBS}
        COMMAND ${CP_PY_FORCE} ${FORMAT_LIBS_DIR} ${LIBRARY_OUTPUT_PATH} ".a"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    add_custom_command(
        OUTPUT install_hal_release
        COMMENT "Install hal_release ..."
        COMMAND ${CP_PY_FORCE} ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/audio/libs/ ${LIBRARY_OUTPUT_PATH} ".a"
        COMMAND ${CP_PY_FORCE} ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/codec/libs/ ${LIBRARY_OUTPUT_PATH} ".a"
        COMMAND ${CP_PY_FORCE} ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/common/libs/ ${LIBRARY_OUTPUT_PATH} ".a"
        COMMAND ${CP_PY_FORCE} ${FORMAT_LIBS_DIR} ${LIBRARY_OUTPUT_PATH} ".a"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# closed source library need compile rule to make
set(HAL_LIBS media_hal_common audio_hw audio_primary_port audio_bluetooth_port format_hw audio_port_common codec_hw codec_plugin_vdec camera_codec_hw hdi_camera_intf)

if (EXISTS ${COMPONENT_DIR})
    set(COMPONENT_LIBS)
else()
    set(COMPONENT_LIBS hmf_demuxer plugin_demuxer_hmf plugin_demuxer_mjpeg plugin_protocol_https)
endif()

set(COMPONENT_LIST ${COMPONENT_LIBS})
set(MEDIA_LIB_LIST)
foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    set(LIB_DIR ${LIBRARY_OUTPUT_PATH})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()
    set(WHOLE_LINK true)
    set(MEDIA_LIB_LIST ${MEDIA_LIB_LIST} ${LIBRARY_OUTPUT_PATH}/lib${COMPONENT_NAME}.a)
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../hal)
        if (${COMPONENT_NAME} IN_LIST HAL_LIBS)
            build_component()
        endif()
    endif()
    if (NOT EXISTS ${COMPONENT_DIR})
        if (${COMPONENT_NAME} IN_LIST COMPONENT_LIBS)
            build_component()
        endif()
    endif()
endforeach()

target_link_libraries(${TARGET_NAME} PRIVATE -Wl,--whole-archive ${MEDIA_LIB_LIST} -Wl,--no-whole-archive)

add_custom_target(${PROJECT_NAME} ALL DEPENDS install_hal_release)

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/${OS} "*")
