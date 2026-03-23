#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "deviceauth")

set(CMAKE_DEVICEAUTH_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

if("${CHIP}" STREQUAL "ws63")
    set(SOURCES
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/hichain.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/add_auth_info.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/add_auth_info_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/auth_info.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/exchange_auth_info.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/exchange_auth_info_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/remove_auth_info_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/hichain.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/huks_adapter/huks_adapter.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/json/commonutil.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/json/jsonutil.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/key_agreement.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/key_agreement_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/key_agreement_server.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/pake_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/pake_server.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/sec_clone_server.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/sts_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/sts_server.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/log/log.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/schedule/build_object.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/schedule/distribution.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/add_auth_info_data.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/add_auth_info_request.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/add_auth_info_response.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/auth_ack_request.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/auth_ack_response.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/auth_start_request.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/auth_start_response.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/exchange_auth_data.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/exchange_request.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/exchange_response.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/import_add_auth_data.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/inform_message.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/key_agreement_version.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/key_agreement_version.h
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/pake_client_confirm.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/pake_request.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/pake_response.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/pake_server_confirm.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/parsedata.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/rmv_auth_info_data.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/rmv_auth_info_request.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/rmv_auth_info_response.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/sec_clone_data.c
    )

    set(PUBLIC_HEADER
        # hichain.h
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/interfaces/innerkits/deviceauth_lite/
    )

    set(PRIVATE_HEADER
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/base/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/huks_adapter/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/json/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/key_agreement/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/log/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/schedule/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/struct/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/deviceauth_lite/source/auth_info/
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/interfaces/innerkits/deviceauth_lite/
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    set(COMPONENT_CCFLAGS
        -Wno-error=logical-op
        -Wno-error=sign-compare
        -Wno-error=jump-misses-init
        -Wno-sign-compare
        -Wno-jump-misses-init
        -Wno-error=unused-parameter
        -Wno-unused-parameter
        -Wno-unused-but-set-variable
        -Wno-error=unused-variable
    )

    set(PRIVATE_DEFINES
        _SCANTY_MEMORY_
        _CUT_STS_CLIENT_
        _CUT_REMOVE_
        _CUT_ADD_
        _CUT_LOG_
    )

    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

else()
    # 定义源文件列表
    set(SOURCES
        #hal_common_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/clib_types.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/hc_parcel.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/hc_string.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/hc_string_vector.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/hc_tlv_parser.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/json_utils.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/impl/src/string_util.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/key_management_adapter/impl/src/alg_loader.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/hc_mutex.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/hc_task_thread.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/hc_time.c

        #mini
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/key_management_adapter/impl/src/mini/huks_adapter.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/liteos/hc_condition.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/liteos/hc_dev_info.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/liteos/hc_thread.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/liteos/hc_types.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/liteos/mini/hc_init_protection.c

        #POSIX OFF
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/impl/src/liteos/mini/hc_file.c

        #device_auth
        #dev_frameworks_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/session/session_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/session/session_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/module/dev_auth_module_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/module/version_util.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/task_manager/task_manager.c
        #deviceauth_common_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/device_auth.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/session/auth_session_common_util.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_manager_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/session/bind_session_common_util.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/channel_manager/channel_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/callback_manager/callback_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/protocol_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/iso_protocol/iso_protocol_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/pake_protocol/pake_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/pake_protocol/pake_v1_protocol/pake_v1_protocol_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/pake_protocol/pake_protocol_dl_common/pake_protocol_dl_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/pake_protocol/pake_protocol_ec_common/pake_protocol_ec_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/src/pake_protocol/pake_v2_protocol/pake_v2_protocol_common.c
        #group_auth_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/group_auth_manager/group_auth_data_operation.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/group_auth_manager/group_auth_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/session/auth_session/auth_session_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/session/auth_session/auth_session_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/session/auth_session/auth_session_server.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/session/auth_session/auth_session_util.c
        #group_manager_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_operation/group_operation.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_operation/group_operation_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/session/bind_session/bind_session_client.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/session/bind_session/bind_session_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/session/bind_session/bind_session_server.c
        #database_manager_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/data_manager/src/data_manager.c
        #broadcast_manager_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/broadcast_manager/broadcast_manager.c
        #hiview_adapter_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/hiview_adapter/hiview_mock.c
        #os_account_adapter_lite_files
        #${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/os_account_adapter_mock/os_account_adapter_mock.cpp
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/src/os_account_adapter_mock/os_account_adapter_mock.c

        #group_auth_account_unrelated_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/group_auth_manager/account_unrelated_group_auth/account_unrelated_group_auth.c

        #group_manager_peer_to_peer_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_operation/peer_to_peer_group/peer_to_peer_group.c

        #authenticators_p2p_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/das_task_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/das_module.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/das_version_util.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/das_task_main.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/pake_task/pake_task_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/pake_task/pake_message_util.c

        #authenticators_p2p_iso_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/iso_client_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/iso_server_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/iso_task_common.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/iso_task_main.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/iso_protocol_task/iso_client_protocol_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/iso_protocol_task/iso_server_protocol_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/lite_exchange_task/das_lite_token_manager.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/lite_exchange_task/iso_client_bind_exchange_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/lite_exchange_task/iso_client_unbind_exchange_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/lite_exchange_task/iso_server_bind_exchange_task.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/iso_task/lite_exchange_task/iso_server_unbind_exchange_task.c

        #authenticators_p2p_pake_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/pake_task/pake_v1_task_mock/pake_v1_task_main_mock.c
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/pake_task/pake_v2_task_mock/pake_v2_task_main_mock.c

        #authenticators_standard_exchange_task_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_unrelated/pake_task/standard_exchange_task_mock/standard_exchange_task_mock.c

        #group_auth_account_related_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/src/group_auth_manager/account_related_group_auth_mock/account_related_group_auth_mock.c

        #group_manager_identical_account_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_operation/identical_account_group_mock/identical_account_group_mock.c

        #group_manager_across_account_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/group_operation/across_account_group_mock/across_account_group_mock.c

        #authenticators_account_related_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/src/account_related_mock/account_module_mock.c

        #soft_bus_channel_files
        #${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/channel_manager/soft_bus_channel/soft_bus_channel.c

        #soft_bus_channel_mock_files
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/src/channel_manager/soft_bus_channel_mock/soft_bus_channel_mock.c

        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/src/ipc_service.c
        #${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/src/lite/ipc_service_init.c
    )

    set(PUBLIC_HEADER
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/interfaces/innerkits/
    )

    # 包含目录
    set(PRIVATE_HEADER
        #mini
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/interfaces/innerkits
        ${ROOT_DIR}/open_source/openharmony/hiviewdfx/hilog/lite/interfaces/native/kits/hilog_lite
        ${ROOT_DIR}/open_source/openharmony/base/startup/syspara_lite/include
        ${ROOT_DIR}/open_source/openharmony/utils/native/lite/include
        ${ROOT_DIR}/open_source/openharmony/utils/native/lite/memory/include

        #hals_inc_path
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/common_lib/interfaces
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/interfaces
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/os_adapter/interfaces/liteos

        ${ROOT_DIR}/open_source/libboundscheck/include

        ${ROOT_DIR}/open_source/openharmony/third_party/cJSON

        ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/interfaces/kits/common
        ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/interfaces/kits/transport
        ${ROOT_DIR}/open_source/openharmony/foundation/communication/dsoftbus/interfaces/inner_kits/transport
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/inc/hiview_adapter
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/inc/lite
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/interfaces/innerkits

        #inc_path
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/frameworks/inc
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/data_manager/inc
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/inc
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/inc/account_unrelated_group_auth
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/inc/account_related_group_auth
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/inc/session
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/inc/session/auth_session
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_auth/inc/session/auth_session_lite
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc/broadcast_manager
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc/callback_manager
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc/channel_manager
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc/channel_manager/soft_bus_channel
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc/session
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/group_manager/inc/session/bind_session
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/inc
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/inc/module
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/inc/session
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/inc/os_account_adapter
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/frameworks/inc/task_manager
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_related
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated/iso_task
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated/iso_task/iso_protocol_task
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated/iso_task/lite_exchange_task
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated/pake_task
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated/pake_task/pake_v1_task
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/authenticators/inc/account_unrelated/pake_task/pake_v2_task
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc/pake_protocol
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc/pake_protocol/pake_v1_protocol
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc/pake_protocol/pake_v2_protocol
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc/pake_protocol/pake_protocol_dl_common
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc/pake_protocol/pake_protocol_ec_common
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/services/protocol/inc/iso_protocol

        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/key_management_adapter/interfaces
        ${CMAKE_DEVICEAUTH_SOURCE_DIR}/deps_adapter/key_management_adapter/impl/inc
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    set(COMPONENT_CCFLAGS
        -Werror
        -Wno-error=missing-field-initializers
        -Wno-error=typedef-redefinition
        -Wno-error=strict-prototypes
    )

    set(PRIVATE_DEFINES
        _SCANTY_MEMORY_
        _CUT_STS_CLIENT_
        _CUT_REMOVE_
        _CUT_ADD_
        _CUT_LOG_
        HILOG_ENABLE
        DISABLE_HIVIEW
        P2P_PAKE_DL_PRIME_LEN_384
        AUTH_STORAGE_PATH=\"${deviceauth_storage_path}\"
        HICHAIN_THREAD_STACK_SIZE=8192
    )
endif()

set(PUBLIC_DEFINES
)
set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()