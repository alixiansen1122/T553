#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory(api)
add_subdirectory(drv)

set(COMPONENT_NAME "gpu")

set(SOURCES
    ${GPU_SRC_LIST}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${GPU_INC_LIST}
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    CONFIG_GFX_PROC_SUPPORT
    CONFIG_VAU_DFX_ERROR
    CONFIG_VAU_DRAW_DIRECTLY
    GPU_DFX_VIDEO_SUB_RESET
)

if (NOT DEFINES MATCHES "BUILD_RECOVERY_IMAGE")
    list(APPEND PRIVATE_DEFINES "CONFIG_DPU_DFX_ERROR")
endif()

if (NOT DEFINES MATCHES "SW_UART_DEBUG" AND NOT DEFINES MATCHES "SW_RTT_DEBUG")
    list(APPEND PRIVATE_DEFINES "NO_UART_DEBUG")
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Werror=out-of-line-declaration"
    "-Werror=uninitialized"
    "-Werror=int-conversion"
    "-Werror=enum-conversion"
    "-Werror=long-long"
    "-Werror=unused-variable"
    "-Werror=unused-value"
    "-Werror=unused-function"
    "-Werror=sign-compare"
    "-Werror=unused-parameter"
    "-Werror=tautological-compare"
    "-Werror=bad-function-cast"
    "-Werror=incompatible-pointer-types"
    "-Werror=implicit-function-declaration"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(MODULE_NAME "gpu")

set(AUTO_DEF_FILE_ID TRUE)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/gpu/${BOARD}_${APPLICATION})
build_component()
