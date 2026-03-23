#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "storage_jsapi")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/kvstorekit/src/nativeapi_kv_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kvstorekit/src/nativeapi_kv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filekit/src/nativeapi_fs_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filekit/src/nativeapi_fs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/nativeapi_common.cpp
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/jsi
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/kvstorekit/include
    ${CMAKE_CURRENT_SOURCE_DIR}/filekit/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    FEATURE_MODULE_STORAGE
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)
set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
   false
)

build_component()
