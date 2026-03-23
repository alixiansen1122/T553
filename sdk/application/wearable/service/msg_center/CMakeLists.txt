set(COMPONENT_NAME "msg_center")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/notify
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ohos_deps
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks/components_ext
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_voice
    ${ROOT_DIR}/application/wearable/service/watch_setting_msg/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/application/wearable/service/msg_center/src/notify/ketinput/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/player/interfaces/kits
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_center.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/notification_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_center_protocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/msg_center_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/watch_connect/watch_connect_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/watch_connect/watch_connect_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/watch_connect/watch_connect_cmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/watch_connect/watch_connect_parser.cpp
)

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jsi_utils.cpp
)
endif()

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
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
    -Wno-c++11-narrowing
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
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()

set(COMPONENT_NAME "msg_center_adapt")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/common
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../nativeapp/nativelauncher/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ohos_deps
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks/components_ext
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audio_player/interfaces/kits
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiomanager
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiocommon
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/player/interfaces/kits/
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/baidumapcomponent
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/keyinput
    ${ROOT_DIR}/middleware/utils/fs/include/fs
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/services/bts/common
    ${ROOT_DIR}/application/auto_ota
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/application/wearable/service/device_msg
    ${ROOT_DIR}/application/wearable/service/msg_center/src/notify/ketinput/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/player/interfaces/kits

)

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_center_customer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_center_cmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/alarm_notification.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/calendar_notification.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/msg_center_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/msg_center_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/msg_center_self_cmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/ketinput/src/NotifyOnKeyListener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/phone_msg_notification.cpp
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_center_customer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_center_cmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/alarm_notification.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/ketinput/src/NotifyOnKeyListener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/notify/phone_msg_notification.cpp
)
endif()

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
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
    -Wno-c++11-narrowing
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
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()
