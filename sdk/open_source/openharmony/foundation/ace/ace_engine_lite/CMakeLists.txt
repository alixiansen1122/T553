set(COMPONENT_NAME "ace")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/async
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/base/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/context
    ${CMAKE_CURRENT_SOURCE_DIR}/directive
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
    ${CMAKE_CURRENT_SOURCE_DIR}/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets
    ${CMAKE_CURRENT_SOURCE_DIR}/router
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/targets
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/frameworks/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/interfaces/kits/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/api
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/ext
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/include
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/ecma/base
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/jmem
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/jrt
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-core/lit
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-ext/include
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-ext/include/jerryscript-ext
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-port/default/include
    ${ROOT_DIR}/open_source/openharmony/third_party/jerryscript/jerry-libm/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/downloadManager
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/http
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext
)

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/animation/transition_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/async/js_async_work.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/async/message_queue_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/ace_log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/ace_mem_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/async_task_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/cache/cache_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/dft_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/dfx_assist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/event_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/js_debugger_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/js_fwk_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/key_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lazy_load_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lazy_load_watcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/locale_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/memory_heap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/number_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/product_adapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/string_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/time_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mem_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/ace_event_error_code.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/fatal_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_ability.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_ability_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_app_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_app_environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_framework_raw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_profiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/js_timer_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/directive/descriptor_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/directive/directive_watcher_callback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jsi/jsi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/router/js_page_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/router/js_page_state_machine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/router/js_router.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr/app_style.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr/app_style_item.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr/app_style_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr/app_style_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr/app_style_sheet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/targets/platform_adapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/js.cpp
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/base/ace_mem_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mem_util.cpp
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
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()


set(COMPONENT_NAME "ace_adapt")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/async
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/base/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/context
    ${CMAKE_CURRENT_SOURCE_DIR}/directive
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
    ${CMAKE_CURRENT_SOURCE_DIR}/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets
    ${CMAKE_CURRENT_SOURCE_DIR}/router
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/targets
    ${ROOT_DIR}/application/wearable/service/msg_center/include/vendor_app
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/network/connection_manage
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/frameworks/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/interfaces/kits/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/keyinput
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/network
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${ROOT_DIR}/open_source/openharmony/base/sensors/interfaces/kits/native/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/downloadManager
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/http
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext
)

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/components/analog_clock_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/canvas_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/chart_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/circle_progress_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/clock_hand_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/component_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/div_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/event_listener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/horizon_progress_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/image_animator_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/image_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/input_button_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/input_checkbox_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/input_radio_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/input_edittext_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/list_adapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/list_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/marquee_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/picker_view_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/qrcode_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/scroll_layer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/slider_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/stack_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/swiper_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/switch_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/tab_bar_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/tab_content_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/tabs_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/text_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/components/baidu_map_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/app_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/device_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/nativeapi_ohos_deviceinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/network/network_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/network/network_listener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/network/connection_manage/connection_manage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/nfc_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/vibrator_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/battery_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/sensor_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/geolocation_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/dfx_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/module_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/locale_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/app_data_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/cjson_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/console_log_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/console_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/date_time_format_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/feature_ability_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/image_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/intl_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/jstest_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/localization_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/number_format_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/preset_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/profiler_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/render_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/require_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/timer_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets/version_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/router_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/sample_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/keyinput/keyinput.cpp
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
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "ace_ext")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/async
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/base/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/context
    ${CMAKE_CURRENT_SOURCE_DIR}/directive
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
    ${CMAKE_CURRENT_SOURCE_DIR}/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets
    ${CMAKE_CURRENT_SOURCE_DIR}/router
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/targets
    ${ROOT_DIR}/application/wearable/service/msg_center/include/vendor_app
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/network/connection_manage
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/frameworks/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/interfaces/kits/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/keyinput
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/network
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${ROOT_DIR}/open_source/openharmony/base/sensors/interfaces/kits/native/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/downloadManager
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/http
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext
)

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext/menu_item_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext/list_ext_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext/scroll_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext/list_adapter_ext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ace_ext/webimage_component.cpp
)
endif()

if("XTS_SUPPORT" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ace_ext/webimage_component.cpp")
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
    FEATURE_COMPONENT_LISTEXT
    FEATURE_COMPONENT_SCROLL
    FEATURE_COMPONENT_WEBIMAGE
)

if("XTS_SUPPORT" IN_LIST DEFINES)
    list(REMOVE_ITEM PUBLIC_DEFINES FEATURE_COMPONENT_WEBIMAGE)
endif()


set(PRIVATE_DEFINES
    ENABLE_LOCATION
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "baidumapcomponent")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/async
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/base/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/context
    ${CMAKE_CURRENT_SOURCE_DIR}/directive
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
    ${CMAKE_CURRENT_SOURCE_DIR}/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets
    ${CMAKE_CURRENT_SOURCE_DIR}/router
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/targets
    ${ROOT_DIR}/application/wearable/service/msg_center/include/vendor_app
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/baidumapcomponent
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/application/wearable/service/xiaodu_voice
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/baidumapcomponent/baidumap.cpp
)

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
    FEATURE_MODULE_BAIDU
    FEATURE_COMPONENT_BAIDUMAP
)

set(PRIVATE_DEFINES
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "brightness")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/display
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/display/display.cpp
)

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
    FEATURE_MODULE_BRIGHTNESS
)

set(PRIVATE_DEFINES
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "ace_http")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/http
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/js/builtin/filekit/include
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/js/builtin/common/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/downloadManager
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/http/http_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/http/http_module_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/http/http_module_callback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/downloadManager/DownloadManager.cpp
)

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
    FEATURE_MODULE_HTTP
)

set(PRIVATE_DEFINES
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "ace_websocket")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/http
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/websocket/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/websocket/websocket_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/websocket/websocket_module_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/websocket/websocket_module_callback.cpp
)

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
    FEATURE_MODULE_WEBSOCKET
)

set(PRIVATE_DEFINES
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "ace_ble")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/bluetooth
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include/bluetooth
    ${ROOT_DIR}/middleware/services/bts/ble
    ${ROOT_DIR}/middleware/services/bts/br
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_gattc_callback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_discover.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_advertise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_scan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_gatts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/bluetooth/ble_module_gatts_callback.cpp
)

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

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    FEATURE_MODULE_BLE
)

build_component()

set(COMPONENT_NAME "netstack_http")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/http/internal
)

if("SUPPORT_LWIP" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/http/src/http_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/http/src/http_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/http/src/http_wrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/http/src/http_form_data.c
)
endif()

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
    CONFIG_USE_LWIP
    CONFIG_HTTP_SECURE
)

set(PRIVATE_DEFINES
    CONFIG_HTTP_FORM_DATA
    CONFIG_HTTP_FILE_OPERATE
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "websocket")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/websocket/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/netstack/websocket/internal
    ${ROOT_DIR}/open_source/openharmony/third_party/lwip/lwip_v2.1.3/src/include
)

if("SUPPORT_LWIP" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/websocket/src/net_websocket_frame.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/websocket/src/net_websocket_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/websocket/src/net_websocket_priv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/websocket/src/net_websocket_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/websocket/src/net_websocket_string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/netstack/websocket/src/net_websocket_thread.c
)
endif()

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
    WEBSOCKET_SSL_ENABLE
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()

set(COMPONENT_NAME "ace_watchface")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/watchface
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/js/builtin/filekit/include
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/js/builtin/common/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include/main/dial/
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/format/interfaces/include/
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/watchface/watchface_module.cpp
)

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
    FEATURE_MODULE_WATCHFACE
)

set(PRIVATE_DEFINES
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()


set(COMPONENT_NAME "ace_calendar")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/async
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/base/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/context
    ${CMAKE_CURRENT_SOURCE_DIR}/directive
    ${CMAKE_CURRENT_SOURCE_DIR}/include/base
    ${CMAKE_CURRENT_SOURCE_DIR}/include/context
    ${CMAKE_CURRENT_SOURCE_DIR}/jsi
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/presets
    ${CMAKE_CURRENT_SOURCE_DIR}/router
    ${CMAKE_CURRENT_SOURCE_DIR}/stylemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/targets
    ${ROOT_DIR}/application/wearable/service/msg_center/include/vendor_app
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits

    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/frameworks/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/i18n_lite/interfaces/kits/i18n/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/keyinput
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/network
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/interfaces/kits/include
    ${ROOT_DIR}/open_source/openharmony/base/sensors/interfaces/kits/native/include
    ${ROOT_DIR}/application/wearable/service/msg_center/include/client
)

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/calendarManager/calendar_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/calendarManager/calendar_timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/calendarManager/calendar_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/calendarManager/event_id_manager.cpp
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
    ENABLE_MODULE_CALENDAR
)

set(PRIVATE_DEFINES
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()
