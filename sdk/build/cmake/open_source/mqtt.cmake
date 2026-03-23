set(COMPONENT_NAME "mqtt")

set(CMAKE_MQTT_SOURCE_DIR
    ${ROOT_DIR}/open_source/mqtt/src)

set(SOURCES
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTTime.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTProtocolClient.c
    ${CMAKE_MQTT_SOURCE_DIR}/Clients.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTClient.c
    #${CMAKE_MQTT_SOURCE_DIR}/MQTTAsync.c
    #${CMAKE_MQTT_SOURCE_DIR}/MQTTAsyncUtils.c
    ${CMAKE_MQTT_SOURCE_DIR}/utf-8.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTPacket.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTPacketOut.c
    ${CMAKE_MQTT_SOURCE_DIR}/Messages.c
    ${CMAKE_MQTT_SOURCE_DIR}/Tree.c
    ${CMAKE_MQTT_SOURCE_DIR}/Socket.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTPersistence.c
    ${CMAKE_MQTT_SOURCE_DIR}/Thread.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTProtocolOut.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTPersistenceDefault.c
    ${CMAKE_MQTT_SOURCE_DIR}/SocketBuffer.c
    ${CMAKE_MQTT_SOURCE_DIR}/LinkedList.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTProperties.c
    ${CMAKE_MQTT_SOURCE_DIR}/MQTTReasonCodes.c
    ${CMAKE_MQTT_SOURCE_DIR}/Base64.c
    ${CMAKE_MQTT_SOURCE_DIR}/SHA1.c
    ${CMAKE_MQTT_SOURCE_DIR}/WebSocket.c
    ${CMAKE_MQTT_SOURCE_DIR}/Heap.c
    ${CMAKE_MQTT_SOURCE_DIR}/Log.c
    ${CMAKE_MQTT_SOURCE_DIR}/SSLSocket_mbedtls.c
    ${CMAKE_MQTT_SOURCE_DIR}/Proxy.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_MQTT_SOURCE_DIR}/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library
)

set(PRIVATE_DEFINES
    HIGH_PERFORMANCE
    NOSTACKTRACE
    MBEDTLS
)

set(PUBLIC_DEFINES
    WEAR_LITEOS_ADAPT
    MBEDTLS_USE_CRT
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