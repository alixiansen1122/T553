set(COMPONENT_NAME "sensor_hdi")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hsensormgr_service/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hsensor_data_service/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsensormgr_service/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsensormgr_service/src/vsensor_task_common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_interface_impl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hsensormgr_service/src/hsensormgr_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hsensor_data_service/src/hsensor_data_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsensormgr_service/src/vsensormgr_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsensormgr_service/src/vsensor_task_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsensormgr_service/src/vsensor_task_common/src/vsensor_task_class.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsensormgr_service/src/vsensor_task_common/src/vsensor_task_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_msgqueue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task/src/sensor_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task/src/sensor_tmsg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_para_decision.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/src/sensor_state.c
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

set(PUBLIC_DEFINES
)

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "ENABLE_INFO_LEVEL_LOG")
set(PRIVATE_DEFINES
    ${CDEFINES}
)

build_component()
