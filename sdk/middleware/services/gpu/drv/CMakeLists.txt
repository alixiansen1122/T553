#[[
Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
Description: CMake entries of sub-directories.
Create: 2021-03-15
]]
add_subdirectory(dpu)
add_subdirectory(vau)
if("SUPPORT_GPU_JPEG" IN_LIST DEFINES)
    add_subdirectory(jpeg)
endif()
add_subdirectory(comm)
add_subdirectory(include)
