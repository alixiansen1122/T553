#===============================================================================
# @brief    nfc hal core
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

set(CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/nfc_hal_core.c)

set(SOURCES "${SOURCES}"
    ${CORE_SRC}
    PARENT_SCOPE
)

if(DEFINED CONFIG_HAVE_NFC_POLL)
    set(PUBLIC_DEFINES "${PUBLIC_DEFINES}"
        HAVE_NFC_POLL
    )
endif()

if(DEFINED CONFIG_HAVE_NFC_LISTEN)
    set(PUBLIC_DEFINES "${PUBLIC_DEFINES}"
        HAVE_NFC_LISTEN
    )
endif()

set(PUBLIC_DEFINES "${PUBLIC_DEFINES}" PARENT_SCOPE)