set(COMPONENT_NAME "ohosfwk_at_service")

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ohosfwk_at_process.cpp
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/openharmony/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
    ${ROOT_DIR}/openharmony/foundation/aafwk/aafwk_lite/include/aafwk/aafwk_lite/frameworks/abilitymgr_lite/include
    ${ROOT_DIR}/openharmony/foundation/aafwk/aafwk_lite/include/appexecfwk/appexecfwk_lite/interfaces/kits
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/open_source/openharmony/foundation/appexecfwk/appexecfwk_lite/include/appexecfwk/appexecfwk_lite/services/bundlemgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/sensors/location/sample
    ${ROOT_DIR}/open_source/openharmony/base/sensors/sample
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
)

set(PRIVATE_DEFINES
    OHOS_DEBUG
)

if("JS_ENABLE" IN_LIST DEFINES)
set(PUBLIC_DEFINES
    SUPPORT_AT_JSINSTALL
)
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
