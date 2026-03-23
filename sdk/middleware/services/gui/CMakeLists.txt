set(MODULE_NAME "gui")
set(AUTO_DEF_FILE_ID TRUE)
if (NOT DEFINES MATCHES "SW_UART_DEBUG" AND NOT DEFINES MATCHES "SW_RTT_DEBUG")
set(CCDEFINES ${DEFINES} "USE_DIAG_LOG")
endif()

if (DEFINES MATCHES "ENABLE_LVGL")
    add_subdirectory(lvgl ${PROJECT_BINARY_DIR}/lvgl)
endif()

if (DEFINES MATCHES "__LITEOS__")
    set(CCDEFINES ${CCDEFINES} "__LITEOS_M__")
endif()

if (DEFINES MATCHES "USE_CMSIS_OS")
    set(CCDEFINES ${CCDEFINES} "ENABLE_CMSIS")
endif()
set(ROOTDIR ${ROOT_DIR})
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (DEFINES MATCHES "ENABLE_UIKIT")
    add_subdirectory(uikit ${PROJECT_BINARY_DIR}/uikit)
endif()

add_subdirectory(hal)

if (DEFINES MATCHES "ENABLE_LVGL" OR DEFINES MATCHES "ENABLE_UIKIT")
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/lvgl/demos "*")
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/uikit/res "*")
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/uikit/sample "*")
endif()
