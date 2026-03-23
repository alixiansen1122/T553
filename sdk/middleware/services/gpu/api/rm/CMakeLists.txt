set(COMPONENT_NAME "gfx_rm")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rm/src/vau_stroke_dash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm/src/vau_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm/src/vau_stroke_path.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm/src/vau_math.c

    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/clock/gmmu_clock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/context/gmmu_context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/dev/gmmu_dev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/dfx/gmmu_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/hal/gmmu_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/intf/drv_gmmu_intf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/ioctl/gmmu_ioctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/isr/gmmu_isr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/mem/gmmu_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/base/gmmu_comm.c

    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/api/src/soc_gmmu_api.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/common_headers/native
    ${ROOT_DIR}/middleware/utils/common_headers/drivers/gpu
    ${ROOT_DIR}/middleware/services/gpu/api/vau/debug
    ${ROOT_DIR}/middleware/services/gpu/drv/vau/mem
    ${ROOT_DIR}/middleware/services/gpu/drv/comm
    ${ROOT_DIR}/middleware/services/gpu/drv/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/rm/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/base
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/clock
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/context
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/dev
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/dfx
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/include
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/intf
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/ioctl
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/isr
    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/drv/mem

    ${CMAKE_CURRENT_SOURCE_DIR}/gmmu/api/inc
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    CONFIG_VG_RUN_LINUX
    CONFIG_VAU_DFX_ERROR
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