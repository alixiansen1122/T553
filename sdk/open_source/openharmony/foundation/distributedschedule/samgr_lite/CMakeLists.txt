set(COMPONENT_NAME "samgr")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/adapter
    ${CMAKE_CURRENT_SOURCE_DIR}/communication/ipc_lite/interfaces/kits
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/kits/communication/broadcast
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/kits/registry
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/kits/samgr
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/dependency
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/registry
    ${PUBLIC_HEADER}

)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/services/communication/broadcast/source/broadcast_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/communication/broadcast/source/pub_sub_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/communication/broadcast/source/pub_sub_implement.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/adapter/cmsis/memory_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/adapter/cmsis/queue_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/adapter/cmsis/thread_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/adapter/cmsis/time_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/dependency/dependency.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/registry/service_registry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/iunknown.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/message_alloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/samgr_lite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/samgr_lite_vtbl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/samgr/source/task_manager.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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
    -Wno-error=unused-function
    -Wno-int-conversion
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

build_component()
