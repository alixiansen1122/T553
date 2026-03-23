#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_LIST "ims")

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/volte/${BOARD}_${APPLICATION})
foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()
    set(PUBLIC_HEADER ${ROOT_DIR}/protocol/ims/include)
    find_library(LIBS${COMPONENT_NAME} "lib${COMPONENT_NAME}.a" ${LIBRARY_OUTPUT_PATH})
    target_link_libraries(${TARGET_NAME} PRIVATE -Wl,--whole-archive  ${LIBS${COMPONENT_NAME}} -Wl,--no-whole-archive )
    build_component()
endforeach()
