#===============================================================================
# @brief    nfc hal
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(core)
add_subdirectory_if_exist(hal_adapter)
add_subdirectory_if_exist(mxxce)
add_subdirectory_if_exist(tag)

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
set(PUBLIC_DEFINES "${PUBLIC_DEFINES}" PARENT_SCOPE)
