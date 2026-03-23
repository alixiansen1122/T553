set(COMPONENT_NAME "dsoftbus_deps")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/porting
    ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/core/adapter/bus_center/include
    ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/adapter/common/net/bluetooth/include
    ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/core/common/include
    ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/interfaces/kits/common
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/ohos_bt_gap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/ohos_bt_gatt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/ohos_bt_gatt_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/ohos_bt_gatt_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/porting/ohos_bt_gatt_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/porting/ohos_bt_gatt_client_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/porting/ohos_bt_gatt_server_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/dsoftbus_ble_gatt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/dsoftbus_ble_gattc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/dsoftbus_ble_gatts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/dsoftbus_cpp_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/pthread_setname_np.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsoftbus/ip_utils.c
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

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

build_component()
