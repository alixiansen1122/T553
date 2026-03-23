#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
#===============================================================================
file(GLOB_RECURSE SOURCES host/*.c)
list(LENGTH SOURCES SOURCES_NUM)
if(NOT ${SOURCES_NUM} EQUAL 0)
    add_subdirectory_if_exist(host)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/ims.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/ims.cmake)
endif()
