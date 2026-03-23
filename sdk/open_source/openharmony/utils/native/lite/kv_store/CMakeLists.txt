#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "utils_kv_store")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kvstore_common/kvstore_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kvstore_impl_posix/kv_store.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kvstore_common
    ${CMAKE_CURRENT_SOURCE_DIR}/innerkits
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition 
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
