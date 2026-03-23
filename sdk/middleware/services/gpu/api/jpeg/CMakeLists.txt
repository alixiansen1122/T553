#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(
    jpeg_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/src/soc_jpeg_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg_csc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg_exif.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg_mem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg_parse.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg_stream.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg_table.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${jpeg_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    jpeg_api_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${jpeg_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)