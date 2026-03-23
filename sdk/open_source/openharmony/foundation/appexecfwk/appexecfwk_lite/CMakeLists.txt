set(COMPONENT_NAME "bms")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/frameworks/bundle_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/innerkits/bundlemgr_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/innerkits/bundlemgr_lite/slite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/kits/bundle_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/kits/bundle_lite/slite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/services/bundlemgr_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/utils/bundle_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/extcmd/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/open_source/openharmony/base/security/appverify/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
)

if("__LITEOS__" IN_LIST DEFINES)
elseif ("__FREERTOS__" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${ROOT_DIR}/middleware/services/utils/fs/include/freertos_adapt
    )
endif()

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/ability_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/ability_info_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/bundle_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/bundle_info_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/bundle_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/bundle_manager_inner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/bundlems_slite_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/element_name.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/module_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlekit/module_info_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/bundle_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/bundle_mgr_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/bundle_mgr_slite_feature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/bundle_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/gt_bundle_extractor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/gt_bundle_installer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/gt_bundle_manager_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/gt_bundle_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/gt_extractor_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extcmd/appsys_bms_cmd.cpp
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

set(AUTO_ADD_PATH
    false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    OHOS_DEBUG
    JERRY_FOR_IAR_CONFIG
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/extcmd/include" "*")

build_component()

set(COMPONENT_NAME "bms_sign_mode")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/frameworks/bundle_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/innerkits/bundlemgr_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/innerkits/bundlemgr_lite/slite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/kits/bundle_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/interfaces/kits/bundle_lite/slite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/services/bundlemgr_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include/appexecfwk/appexecfwk_lite/utils/bundle_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/extcmd/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
)

if("__LITEOS__" IN_LIST DEFINES)
elseif ("__FREERTOS__" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${ROOT_DIR}/middleware/services/utils/fs/include/freertos_adapt
    )
endif()

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bundlems/sign_mode.cpp
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

set(AUTO_ADD_PATH
    false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    OHOS_DEBUG
    JERRY_FOR_IAR_CONFIG
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/extcmd/include" "*")

build_component()