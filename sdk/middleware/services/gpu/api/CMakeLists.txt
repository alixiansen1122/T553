#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory(dpu)
add_subdirectory(vau)
add_subdirectory(rm)

if("SUPPORT_GPU_OPENVG" IN_LIST DEFINES)
    add_subdirectory(openvg)
endif()
if("SUPPORT_GPU_JPEG" IN_LIST DEFINES)
    add_subdirectory(jpeg)
endif()