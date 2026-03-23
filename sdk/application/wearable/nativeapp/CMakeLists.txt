set(COMPONENT_NAME "nativelauncher")
set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)
set(NativeApp ${CMAKE_CURRENT_SOURCE_DIR})
set(UIKIT_ROOT_DIR ${ROOT_DIR}/middleware/services/gui/uikit)
set(UI_SAMPLE_ROOT ${ROOT_DIR}/middleware/services/gui/uikit/sample/uitest)

set(PUBLIC_HEADER
    ${NativeApp}/nativelauncher/include
    ${NativeApp}/nativeui/include
)

set(PRIVATE_HEADER
    ${NativeApp}/nativeui/
    ${NativeApp}/nativeui/uisample/uitest/framework/include
    ${NativeApp}/nativeui/inlcude/screennotify
    ${NativeApp}/../service/smart_voice/smart_voice_service/interfaces/innerkits/native_c/include
    ${NativeApp}/../service/smart_voice/voice_engine_hal/interfaces/innerkits/native_c/include
    ${NativeApp}/../ohos_deps

    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/innerkits

    ${UI_SAMPLE_ROOT}/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/events
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/player/interfaces/kits/
    ${ROOT_DIR}/open_source/openharmony/third_party/qrcodegen/cpp
    ${ROOT_DIR}/include/middleware/services/bts/common
    ${ROOT_DIR}/include/middleware/services/bts/ble
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/middleware/chips/brandy/nv/

    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/open_source/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr

    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
    ${ROOT_DIR}/application/wearable/service/msg_center/include
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/application/wearable/service/device_msg
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audio_player/interfaces/kits
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiomanager
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/audiomanager/interfaces/kits/audiocommon
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/player/interfaces/kits/
    ${ROOT_DIR}/open_source/openharmony/foundation/multimedia/utils/include
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/kv_store/innerkits
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/format/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${ROOT_DIR}/application/wearable/service/dial_market
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/middleware/utils/dfx/diag/
    ${ROOT_DIR}/middleware/utils/dfx/diag/diag_cmd_router/include
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hilog/lite/interfaces/native/kits/hilog_lite
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOTDIR}/open_source/flatbuffers/include
    ${ROOTDIR}/open_source/openharmony/third_party/lwip/lwip_adapter/liteos/src/include/public
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/calendarManager
)

file(GLOB_RECURSE SOURCES "${NativeApp}/nativeui/*.cpp")
file(GLOB_RECURSE UI_SAMPLE_SRC "${UI_SAMPLE_ROOT}/components/*.cpp")
list(REMOVE_ITEM UI_SAMPLE_SRC
    ${UI_SAMPLE_ROOT}/components/ui_test_bitmap_font/ui_test_bitmap_font.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/ui_test_clock_dial.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/ScreenModel.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/PullUpDownView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/MainClockView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/HeartRateView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/CompassView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/CompassControlView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/ActivityWeekView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_compass/ui_test_compass.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_dump_dom_tree/ui_test_dump_dom.cpp
)

if(NOT "CONFIG_NATIVEAPP_TEST" IN_LIST DEFINES)
    file(GLOB FILES_TO_REMOVE_MONKEY "${CMAKE_CURRENT_SOURCE_DIR}/nativeui/monkey/src/*.cpp")
    file(GLOB FILES_TO_REMOVE_EMPTY "${CMAKE_CURRENT_SOURCE_DIR}/nativeui/emptypage/src/*.cpp")
    list(APPEND FILES_TO_REMOVE ${FILES_TO_REMOVE_MONKEY} ${FILES_TO_REMOVE_EMPTY})
    list(REMOVE_ITEM SOURCES ${FILES_TO_REMOVE})
endif()

set(SOURCES
    ${SOURCES}
    ${NativeApp}/nativelauncher/src/NativeLauncher.cpp
    ${NativeApp}/nativelauncher/src/NativeRegisterManager.cpp
    ${NativeApp}/nativelauncher/src/UICardPageFactory.cpp
    ${NativeApp}/nativelauncher/src/UIWatchDialFactory.cpp
    ${UI_SAMPLE_SRC}
)

list(REMOVE_ITEM SOURCES
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParser.cpp
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParserManager.cpp
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParserV3.cpp
)

if("SUPPORT_ALIPAY_SEC" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES "${NativeApp}/nativeui/alipay/src/AlipayRefreshBindCode.cpp")
else()
list(REMOVE_ITEM SOURCES ${NativeApp}/nativeui/alipay/src/AlipayMainView.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayModel.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayPresenter.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayBind.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayExitBind.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayBindRes.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayBindAni.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayPayCode.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayBarCode.cpp

    					 ${NativeApp}/nativeui/alipay/src/AlipaySetting.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayHelp.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayUnbindFinish.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayUnbind.cpp
						 ${NativeApp}/nativeui/alipay/src/AlipayRefreshBindCode.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayList/AlipayListAdapter.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayList/AlipayListView.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayList/AlipayListItemView.cpp

    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayGetTransList.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransTips.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransCode.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransEnable.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransRes.cpp

    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransListAdapter.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransListView.cpp
   					     ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransListItemView.cpp
    					 ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransAni.cpp
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
    -Wno-missing-field-initializers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
    SUPPORT_NATIVE_UI
)

set(PRIVATE_DEFINES
)

build_component()

set(COMPONENT_NAME "uikit_fwk")
set(UIKIT_NATIVE_FWK ${ROOT_DIR}/application/wearable/nativeabilityfwk)
set(UIKIT_NATIVE_DIAL ${NativeApp}/nativeui/main)
set(COMPONENT_SRC
    ${UIKIT_NATIVE_FWK}/src/View.cpp
    ${UIKIT_NATIVE_FWK}/src/SlicePage.cpp
    ${UIKIT_NATIVE_FWK}/src/BackDragListener.cpp
    ${UIKIT_NATIVE_FWK}/src/PageTransitionMgr.cpp
    ${UIKIT_NATIVE_FWK}/src/TransitionManager.cpp
    ${UIKIT_NATIVE_FWK}/src/RecentManager.cpp
    ${UIKIT_NATIVE_FWK}/src/SlicePageManager.cpp
    ${UIKIT_NATIVE_FWK}/src/SlicePageFactory.cpp
    ${UIKIT_NATIVE_FWK}/src/TransitionRecord.cpp
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParser.cpp
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParserManager.cpp
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParserV3.cpp
)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_NATIVE_FWK}/include
    ${UIKIT_NATIVE_DIAL}/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks/
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/format/interfaces/include
    ${UIKIT_NATIVE_DIAL}/include/dial
    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
    ${ROOT_DIR}/middleware/services/media/foundation/player/interfaces/kits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${ROOT_DIR}/open_source/openharmony/base/security/appverify/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
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
    -Wno-missing-field-initializers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
