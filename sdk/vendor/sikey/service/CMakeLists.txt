set(COMPONENT_NAME "sk_service")

set(LIBS
 ${CMAKE_CURRENT_SOURCE_DIR}/paqichong/libsample_a.a
)


set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/net_service/paqichong_web_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/net_service/net_data_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/net_service/http_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/net_service/paqichong_connect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/net_service/paqichong_debug_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/paqichong/paqichong_data_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_service/gnss_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_service/gnss_custom_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_service/gnss_nmea_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_service/gnss_mode_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_service/gnss_cnr_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_service/modem_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sk_audio/sk_audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_cmd/modem_cmd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/location_service/location_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_service/dev_storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sk_ota/sk_ota.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_service/mqtt_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_service/mqtt_cmd_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_service/mqtt_event.c
)



set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/net_service
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_service
    ${CMAKE_CURRENT_SOURCE_DIR}/paqichong
    ${CMAKE_CURRENT_SOURCE_DIR}/sk_audio
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_cmd
    ${CMAKE_CURRENT_SOURCE_DIR}/location_service
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss_service
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_service
    ${CMAKE_CURRENT_SOURCE_DIR}/sk_ota
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_service

)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/websocket/include
    ${CMAKE_SOURCE_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include/
    ${CMAKE_SOURCE_DIR}/open_source/openharmony/third_party/cJSON
    ${CMAKE_SOURCE_DIR}/middleware/chips/brandy/exception
    ${CMAKE_SOURCE_DIR}/application/samples/gnss
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/protocol/ims/include
    ${CMAKE_SOURCE_DIR}/vendor/sikey/drivers/fg/sh366102
    ${CMAKE_SOURCE_DIR}/application/wstp/at_service/app
    ${CMAKE_SOURCE_DIR}/vendor/sikey/drivers
    ${CMAKE_SOURCE_DIR}/vendor/sikey/drivers/gsensor/LSM6DSOW
    ${CMAKE_SOURCE_DIR}/open_source/mqtt/src
    ${CMAKE_SOURCE_DIR}/open_source/openharmony/kernel/liteos_m/kal/cmsis

)


set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c11
    -Wno-error
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
