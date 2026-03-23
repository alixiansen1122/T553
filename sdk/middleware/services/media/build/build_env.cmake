#[[
Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
Description: CMake construct system.
Author: Media Software Group
Create: 2021-05-18
]]

if("${OS_TYPE}" STREQUAL "Linux")
    set(TARGET_OS Linux)
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
    set(SECUREC_INCLUDE ${PROJECT_ROOT_DIR}/llt/multimedia/hld/ut/libs/libc_sec/include)
endif()

# HAL_RELEASE_PATH
set(HAL_RELEASE_PATH hal_release/${OS})
