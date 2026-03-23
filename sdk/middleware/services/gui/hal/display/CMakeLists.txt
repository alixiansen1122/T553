#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)

set(COMPONENT_NAME "display_gfx")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/yuv_to_rgb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_gmmu.c
    ${PROJECT_ROOT_DIR}/hal/common/src/barcode128.c
)

set(PUBLIC_HEADER
    ${PROJECT_ROOT_DIR}/hal/common/include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${ROOTDIR}/middleware/services/gpu/api/rm/gmmu/api/inc
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags} -nostdlibinc)
list(APPEND CFLAG -Wno-missing-field-initializers)
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-int-conversion
    -Wno-sign-compare
    -Wno-enum-conversion
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-incompatible-pointer-types
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "display_gralloc")

set(SOURCES_GRALLOC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_gralloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool/src/mem_pool_los.c
    )
set(SOURCES
    ${SOURCES_GRALLOC}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool/include
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${ROOTDIR}/middleware/services/gpu/api/rm/gmmu/api/inc
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags} -nostdlibinc)
list(APPEND CFLAG -Wno-missing-field-initializers)
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-sign-compare
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "display_layer")

set(SOURCES_LAYER ${CMAKE_CURRENT_SOURCE_DIR}/src/display_layer.c)
set(SOURCES
    ${SOURCES_LAYER}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOTDIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${ROOTDIR}/middleware/services/gpu/api/rm/gmmu/api/inc
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags} -nostdlibinc)
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-unused-parameter
    -Wno-unused-function
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

