set(COMPONENT_NAME "ams")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/frameworks/ability_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/frameworks/abilitymgr_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/frameworks/want_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/interfaces/innerkits/abilitymgr_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/interfaces/innerkits/abilitymgr_lite/slite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/interfaces/kits/ability_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/interfaces/kits/ability_lite/slite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/interfaces/kits/want_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/services/abilitymgr_lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/services/abilitymgr_lite/include/client
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/services/abilitymgr_lite/include/task
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aafwk/aafwk_lite/services/abilitymgr_lite/include/util
    ${CMAKE_CURRENT_SOURCE_DIR}/extcmd/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/context
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/base
    ${ROOT_DIR}/application/wearable/ohos_deps

    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
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
    ${ROOT_DIR}/open_source/openharmony/drivers/peripheral/display/interfaces/include

    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/presets
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
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/ability_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/ability_manager_inner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/abilityms_slite_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/js_app_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/lite_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/slite_ability.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/want.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/ability_env.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilitykit/ability_env_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilityms/ability_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilityms/ability_mgr_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilityms/ability_mgr_slite_feature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilityms/ability_record.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilityms/ability_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abilityms/ability_stack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extcmd/appsys_ams_cmd.cpp
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

