#===============================================================================
# @brief    cmake make file
# Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(SOURCES
    "${SOURCES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_adapter.c
    PARENT_SCOPE
)

set(PUBLIC_HEADER
    "${PUBLIC_HEADER}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)