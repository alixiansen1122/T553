#[[
Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
Description: CMake entries of sub-directories.
Author: Media Software Group
Create: 2022-05-09
]]

if (DEFINES MATCHES "__LITEOS__")
    set(CDEFINES ${DEFINES} "__LITEOS_M__")
    set(CCDEFINES ${DEFINES} "__LITEOS_M__")
endif()

if("${CHIP}" STREQUAL "brandy")
    add_subdirectory(utils)
    add_subdirectory(audiomanager manager)
    add_subdirectory(player player)
    add_subdirectory(audio_capturer capturer)
    add_subdirectory(audio_recorder recorder)
elseif("${CHIP}" STREQUAL "socmn1")
    set(CDEFINES ${DEFINES} "ONLY_SUPPORT_HSO")
    set(CCDEFINES ${DEFINES} "ONLY_SUPPORT_HSO")
    add_subdirectory(utils)
    add_subdirectory(audiomanager manager)
    add_subdirectory(tone_player)
endif()

if(NOT "${OS_TYPE}" STREQUAL "Linux")
if(NOT "${CHIP}" STREQUAL "socmn1")
    add_subdirectory(sample)
endif()
endif()
