#[[
Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
Description: CMake entries of sub-directories.
Author: Media Software Group
Create: 2021-03-15
]]

add_subdirectory(audiomanager)
add_subdirectory(audioservice)
add_subdirectory(audiostream)
add_subdirectory(audioutils)
if ("${CHIP}" STREQUAL "socmn1")
    add_subdirectory(haid_manager)
    add_subdirectory(tws_manager)
endif()