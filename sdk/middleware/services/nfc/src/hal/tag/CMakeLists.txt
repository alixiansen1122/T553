#===============================================================================
# @brief    nfc hal tag
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

set(TAG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_t1t.c
            ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_t2t.c
            ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_t3t.c
            ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_t4t.c
            ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_t5t.c)

set(SOURCES "${SOURCES}"
    ${TAG_SRC}
    PARENT_SCOPE
)