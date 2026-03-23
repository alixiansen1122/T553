#[[
Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
Description: CMake compilation settings.
Author: Media Software Group
Create: 2021-05-18
]]

if("${OS_TYPE}" STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32 -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -ggdb")
else()
    if (DEFINES MATCHES "__LITEOS__")
        set(CDEFINES ${DEFINES} "__LITEOS_M__")
        set(CCDEFINES ${DEFINES} "__LITEOS_M__")
    endif()
    set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT")
    set(CCDEFINES ${CCDEFINES} "CMSIS_SUPPORT")
    if ("${CHIP}" STREQUAL "socmn1")
        set(CDEFINES ${CDEFINES} "ONLY_SUPPORT_HSO")
    endif()
    set(C_FLAGS ${ccflags})
    list(REMOVE_ITEM C_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)

    foreach(item ${C_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${item}")
    endforeach()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdinc")

    foreach(item ${ccflags})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${item}")
    endforeach()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc")
endif()
