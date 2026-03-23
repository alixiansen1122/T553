#===============================================================================
# @brief    sample product
# Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_HAVE_NFC_POLL)
    set(SOURCES "${SOURCES}"
        ${CMAKE_CURRENT_SOURCE_DIR}/sample_alipay.c
    )
endif()

if(DEFINED CONFIG_HAVE_NFC_POLL AND DEFINED CONFIG_HAVE_NFC_LISTEN)
    set(SOURCES "${SOURCES}"
        ${CMAKE_CURRENT_SOURCE_DIR}/sample_access.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sample_rawdata_exchange.c
    )
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)

set(PUBLIC_HEADER "${PUBLIC_HEADER}"
    ${CMAKE_CURRENT_SOURCE_DIR}/
    PARENT_SCOPE
)