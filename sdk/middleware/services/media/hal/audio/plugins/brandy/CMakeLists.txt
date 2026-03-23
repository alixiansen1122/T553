#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory(primary)
add_subdirectory(bluetooth)
if ("${CHIP}" STREQUAL "brandy")
    add_subdirectory(modem)
endif()
add_subdirectory(common)
