#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "huks_sdk")

# cut abilities for authentication
set(disable_authenticate false)

# if HUKS use mbedtls engine
set(huks_use_mbedtls true)

# whether use lite storeage
set(huks_use_lite_storage false)

# whether use hardware root key
set(huks_use_hardware_root_key false)

# whether huks enable log
set(huks_enable_log false)

set(huks_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/src/hks_ability.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/src/hks_base_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/src/hks_check_paramset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/src/hks_common_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/src/hks_crypto_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/src/hks_param.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/core/src/hks_local_engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/os_dependency/ipc/src/hks_client_service_passthrough.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/os_dependency/posix/hks_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/os_dependency/sysinfo/src/hks_get_process_info_passthrough.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/os_dependency/sysinfo/src/hks_get_udid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/huks_standard/main/src/hks_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/huks_standard/main/src/hks_api_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_auth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_core_interfaces.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_core_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_core_service_three_stage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_keyblob.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_keynode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_rkc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_rkc_rw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_secure_access.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_upgrade_key_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_client_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_client_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_hitrace.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_report.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_session_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_upgrade_storage_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/os_dependency/idl/passthrough/huks_access.c
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/os_dependency/idl/passthrough/huks_core_static_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/crypto_adapter/hks_client_service_adapter_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/crypto_adapter/hks_client_service_adapter_lite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/file_operator/hks_file_operator_lite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/list/hks_double_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/mutex/hks_mutex.c
)

if(${huks_use_mbedtls} STREQUAL "true")
    set(huks_mbedtls_engine_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_crypto_ed25519.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ability.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_aes.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_bn.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ecc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ecdh.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_engine.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_hmac.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_kdf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_rsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_x25519.c
    )
endif()

if(${huks_use_lite_storage} STREQUAL "true")
    set(huks_storage_source
        ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_storage_adapter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_storage_lite.c
        ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/src/hks_keyblob_lite.c
    )
else()
    set(huks_storage_source
        ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/src/hks_storage.c
    )
endif()

set(SOURCES
    ${huks_sources}
    ${huks_mbedtls_engine_sources}
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/huks_lite/hks_tmp_client.c
    ${huks_storage_source}
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/huks_standard/main/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/interfaces/base
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/innerkits/huks_standard/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/crypto_engine/mbedtls/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/os_dependency/ipc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/huks_standard/main/os_dependency/sysinfo/include
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/os_dependency/idl/passthrough
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/systemapi_wrap/useridm/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_service/main/systemapi_wrap/hitrace_meter_wrapper/include
    ${CMAKE_CURRENT_SOURCE_DIR}/services/huks_standard/huks_engine/main/core_dependency/include
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/crypto_adapter
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/file_operator
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/list
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/mutex
    ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hilog/lite/interfaces/native/kits/hilog_lite/
    ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
)

set(huks_defines)
if(${disable_authenticate} STREQUAL "true")
    set(huks_defines ${huks_defines} _CUT_AUTHENTICATE_)
endif()

if(${huks_use_hardware_root_key} STREQUAL "true")
    set(huks_defines ${huks_defines} _HARDWARE_ROOT_KEY_)
endif()

if(${huks_use_lite_storage} STREQUAL "true")
    set(huks_defines ${huks_defines} _STORAGE_LITE_)
endif()

if(${huks_enable_log} STREQUAL "true")
    set(huks_defines ${huks_defines} _HUKS_LOG_ENABLE_)
endif()

if(${CHIP} STREQUAL "brandy")
    set(huks_defines ${huks_defines} GET_DEV_UDID_ENABLE HARDWARE_UDID HKS_HARDWARE_ROOT_KEY)
endif()

set(PRIVATE_DEFINES
    _FORTIFY_SOURCE=2
    ${huks_defines}
    HKS_CONFIG_FILE="hks_config_lite.h"
    MBEDTLS_ECP_RESTARTABLE=1
    HKS_CONFIG_KEY_STORE_PATH="/user/app/user" 
    OHOS_DEBUG
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -fvisibility=hidden
    -Wall
    -Wno-error=unused-parameter
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
