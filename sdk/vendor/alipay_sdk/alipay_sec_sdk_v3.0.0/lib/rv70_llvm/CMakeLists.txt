#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(hal)
add_subdirectory_if_exist(app)

set(COMPONENT_INDEX 0)
set(ALIPAY_LIB_LIST)
set(ALIPAY_COMPONNET_LIST 
    "hal_csi_rv70_llvm"
    "app_setting_rv70_llvm"
    "app_aid_code_rv70_llvm"
    "app_bind_ble_rv70_llvm"
    "app_pay_code_rv70_llvm"
    "app_trans_code_rv70_llvm"
    "fwk_bind_status_rv70_llvm"
    "fwk_ble_communicate_rv70_llvm"
    "fwk_common_rv70_llvm"
    "fwk_crypt_rv70_llvm"
    "fwk_secure_mode_se_rv70_llvm"
    "fwk_device_certificate_rv70_llvm"
    "fwk_network_rv70_llvm"
    "fwk_sm2_rv70_llvm"
    "fwk_quickjs_adapt_rv70_llvm"
    "fwk_js_api_rv70_llvm"
    "3rdparty_mbedtls_rv70_llvm"
    "3rdparty_uEcc_rv70_llvm"
    "3rdparty_sm3_rv70_llvm"
    "3rdparty_cJSON_rv70_llvm"
    "3rdparty_protobuf-c_rv70_llvm"
    "app_antchain_rv70_llvm"
    "app_provision_rv70_llvm"
    "fwk_storage_rv70_llvm"
    "fwk_crypt_ext_rv70_llvm"
)

set(ALIPAY_COMPONENT_PATH_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/hal/csi/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/setting/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/aid_code/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/bind_ble/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/pay_code/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/trans_code/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/bind_status/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/ble_communicate/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/common/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/crypt/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/secure_mode_se/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/device_certificate/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/network/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/sm2/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/quickjs_adapt/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/js_api/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mbedtls/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uEcc/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sm3/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cJSON/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/protobuf-c/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/antchain/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/provision/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/storage/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/fwk/crypt_ext/lib"
)

foreach(COMPONENT ${ALIPAY_COMPONNET_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    list (GET ALIPAY_COMPONENT_PATH_LIST ${COMPONENT_INDEX} COMPONENT_PATH)
    set(WHOLE_LINK true)
    set(LIBS ${COMPONENT_PATH}/lib${COMPONENT_NAME}.a)
    build_component()
    math(EXPR COMPONENT_INDEX "${COMPONENT_INDEX} + 1")
endforeach()
