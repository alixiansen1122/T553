#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)

set(COMPONENT_NAME "uikit_test")

file(GLOB_RECURSE src_file ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
if(NOT "JS_ENABLE" IN_LIST DEFINES)
    file(GLOB files_to_remove "${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_transition/*.cpp")
    list(REMOVE_ITEM src_file ${files_to_remove})
endif()
list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/components/test_bitmap_font.cpp)
list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_bitmap_font/ui_test_bitmap_font.cpp)
list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_font/ui_test_font_global.cpp)
list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_image/ui_test_image_packer.cpp)

set(COMPONENT_SRC
    ${src_file}
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/sample
    ${UIKIT_ROOT_DIR}/sample/uitest
    ${UIKIT_ROOT_DIR}/sample/uitest/components
    ${UIKIT_ROOT_DIR}/sample/uitest/framework/include
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/proprietary/include/service
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${PROJECT_ROOT_DIR}/hal/input/include
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOTDIR}/open_source/openharmony/third_party/bounds_checking_function/include
    ${ROOTDIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
    ${ROOTDIR}/open_source/openharmony/foundation/multimedia/player/interfaces/kits
    ${ROOTDIR}/open_source/openharmony/foundation/multimedia/utils/include
)

if("JS_ENABLE" IN_LIST DEFINES)
set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${ROOTDIR}/application/wearable/nativeabilityfwk/include
)
endif()

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-missing-field-initializers
    -Wno-writable-strings
    -Wno-reorder-ctor
    -Wno-tautological-pointer-compare
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
