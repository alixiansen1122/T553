#===============================================================================
# @brief    cmake make file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(_CHIP "${CHIP}")
if(${CHIP} STREQUAL "socmn1")
    set(_CHIP "melody")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/wifi/CMakeLists.txt)
	add_subdirectory(wifi)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/wifi.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/wifi.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/controller/CMakeLists.txt)
    add_subdirectory(bt/controller btc)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/btc.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/btc.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/CMakeLists.txt)
    add_subdirectory(bt/host bth)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/bth.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/bth.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/audio/bth_audio/CMakeLists.txt)
    add_subdirectory(bt/host/audio/bth_audio)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/bt/bts/CMakeLists.txt)
    add_subdirectory(bt/host/bt/bts)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/bts.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/bts.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/audio/bth_audio_service/CMakeLists.txt)
    add_subdirectory(bt/host/audio/bth_audio_service)
elseif(EXISTS ${BIN_DIR}/${PROJECT_CHIP_DIR}/melody/libs/bt/CMakeLists.txt)
    add_subdirectory(${BIN_DIR}/melody/libs/bt bt)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nfc/CMakeLists.txt)
    add_subdirectory(nfc)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/nfc.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/nfc.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ims/host/CMakeLists.txt)
    add_subdirectory(ims)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/ims.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/ims.cmake)
endif()
