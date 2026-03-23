#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(PROJECT_ROOT_DIR ${ROOTDIR})
set(COMPONENT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

#set src
set(COMPONENT_NAME "harfbuzz")

return_if_not_build()

set(COMPONENT_SRC
    ${COMPONENT_ROOT_DIR}/hb-aat-layout.cc
    ${COMPONENT_ROOT_DIR}/hb-aat-map.cc
    ${COMPONENT_ROOT_DIR}/hb-blob.cc
    ${COMPONENT_ROOT_DIR}/hb-buffer-serialize.cc
    ${COMPONENT_ROOT_DIR}/hb-buffer.cc
    ${COMPONENT_ROOT_DIR}/hb-common.cc
    ${COMPONENT_ROOT_DIR}/hb-face.cc
    ${COMPONENT_ROOT_DIR}/hb-fallback-shape.cc
    ${COMPONENT_ROOT_DIR}/hb-font.cc
    ${COMPONENT_ROOT_DIR}/hb-map.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-cff1-table.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-cff2-table.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-face.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-font.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-layout.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-map.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-math.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-meta.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-metrics.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-name.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-arabic.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-default.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-hangul.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-hebrew.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-indic-table.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-indic.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-khmer.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-myanmar.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-thai.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-use-table.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-use.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-complex-vowel-constraints.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-fallback.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape-normalize.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-shape.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-tag.cc
    ${COMPONENT_ROOT_DIR}/hb-ot-var.cc
    ${COMPONENT_ROOT_DIR}/hb-set.cc
    ${COMPONENT_ROOT_DIR}/hb-shape-plan.cc
    ${COMPONENT_ROOT_DIR}/hb-shape.cc
    ${COMPONENT_ROOT_DIR}/hb-shaper.cc
    ${COMPONENT_ROOT_DIR}/hb-static.cc
    ${COMPONENT_ROOT_DIR}/hb-subset-cff2.cc
    ${COMPONENT_ROOT_DIR}/hb-subset-cff-common.cc
    ${COMPONENT_ROOT_DIR}/hb-ucd.cc
    ${COMPONENT_ROOT_DIR}/hb-unicode.cc
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${COMPONENT_ROOT_DIR}
)
set(CCDEFINES ${defines} "HB_TINY")
set(PRIVATE_DEFINES
    ${CCDEFINES}
    __STDC_VERSION__
)

set(PUBLIC_DEFINES
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
)

if(${ARCH} STREQUAL "cortex_m7")
    list(REMOVE_ITEM COMPONENT_CCFLAGS -nostdlibinc)
    list(APPEND COMPONENT_CCFLAGS -nostdinc)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
