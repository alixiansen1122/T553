set(COMPONENT_NAME "skdriver")
set(UIKIT_ROOT_DIR ${CMAKE_SOURCE_DIR}/middleware/services/gui/uikit)

set(LIBS
# ${CMAKE_CURRENT_SOURCE_DIR}/paqichong/libsample_a.a
)


set(SOURCES
    # SK Board
    ${CMAKE_CURRENT_SOURCE_DIR}/sk_board.c
    #GSensor Drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/gsensor/gsensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsensor/LSM6DSOW/lsm6dsow.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsensor/LSM6DSOW/lsm6dsow_api.c
    #Barometer Drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/barometer/barometer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/barometer/lqbp002ta/lqbp002ta.c
    # FG Drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/fg/sh366102/sh366102.c
    #${CMAKE_CURRENT_SOURCE_DIR}/fg/sh366102/SH366102_DataFlash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fg/sh366102/SH366102_UpdateFile.c

    ${CMAKE_CURRENT_SOURCE_DIR}/led/rgb_led.c
    # LCD Drivers
    # ${CMAKE_CURRENT_SOURCE_DIR}/aw9364/aw9364.c
    # TP Drivers
)



set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sk_board.h
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/fg/sh366102
    ${CMAKE_CURRENT_SOURCE_DIR}/gsensor/LSM6DSOW
    ${CMAKE_CURRENT_SOURCE_DIR}/barometer/lqbp002ta
    ${ROOT_DIR}/vendor/sikey/drivers
    ${ROOT_DIR}/include/driver
    ${ROOT_DIR}/vendor/sikey/service/paqichong
    ${ROOT_DIR}/middleware/chips/brandy/nv/nv_config/include
    ${ROOT_DIR}/application/wearable/service/mqtt_service
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
