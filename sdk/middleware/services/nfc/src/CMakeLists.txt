#===============================================================================
# @brief    nfc src
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nfc_middleware_src")

set(SOURCES
)

set(PUBLIC_HEADER
)

set(PUBLIC_DEFINES
)

if(DEFINED CONFIG_ENABLE_NFC_CONTROLLER_SAMPLE)
    add_subdirectory_if_exist(hal)
    add_subdirectory_if_exist(ndef)
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
