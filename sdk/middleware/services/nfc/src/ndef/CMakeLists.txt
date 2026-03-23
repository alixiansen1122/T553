#===============================================================================
# @brief    nfc ndef
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_HAVE_NFC_POLL)
    set(NDEF_SRC ${CMAKE_CURRENT_SOURCE_DIR}/nfc_ndef_t2t.c
                ${CMAKE_CURRENT_SOURCE_DIR}/nfc_ndef_t3t.c
                ${CMAKE_CURRENT_SOURCE_DIR}/nfc_ndef_t4t.c
                ${CMAKE_CURRENT_SOURCE_DIR}/nfc_ndef_t5t.c
                ${CMAKE_CURRENT_SOURCE_DIR}/nfc_ndef.c)
endif()

set(SOURCES "${SOURCES}"
    ${NDEF_SRC}
    PARENT_SCOPE
)
