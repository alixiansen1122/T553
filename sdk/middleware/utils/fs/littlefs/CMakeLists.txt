set(COMPONENT_NAME "fs_littlefs")

set(FS_PATH ${ROOT_DIR}/middleware/utils/fs)

set(SOURCES
    # littlefs core
    ${CMAKE_CURRENT_SOURCE_DIR}/lfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lfs_util.c

    # OS adapt layer
    ${CMAKE_CURRENT_SOURCE_DIR}/os_adapt/littlefs_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/os_adapt/vfs_littlefs.c
)

set(PUBLIC_HEADER
    ${FS_PATH}/include
    ${FS_PATH}/vfs/include/disk
    ${FS_PATH}/vfs/include/multi_partition

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/os_adapt
)


set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    CONFIG_FS_LITTLEFS
    LFS_THREADSAFE
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()