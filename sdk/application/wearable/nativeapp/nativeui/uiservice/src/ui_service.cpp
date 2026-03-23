/*
 * Copyright (c) 2021-2021 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uiservice/ui_service.h"
#include "wearable_log.h"
#include "samgr_lite.h"
#include "settings/model/SettingBluetoothModel.h"
#include "main/UiServersMsg.h"
#include "ohos_init.h"
#include "memory_adapter.h"
#include <cstdio>
#include "service_id_define.h"
#include "bundle_install_msg.h"
#include "graphic_service.h"
#include "applist/ApplistModel.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif
#include "screennotify/ui_screennotify.h"
#include "main/HealthModel.h"
#include "bts_br_gap.h"
#include "bts_le_gap.h"

#include "uiservice/ui_language.h"

#include "errcode.h"
#include "pm_definition.h"
#include "NativeAbility.h"
#include "TransitionType.h"

#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
#include "lwip_public.h"
#endif

extern "C" errcode_t uapi_pm_add_sleep_veto(pm_id_t veto_id);
extern "C" errcode_t uapi_pm_remove_sleep_veto(pm_id_t veto_id);

#ifdef __cplusplus
extern "C" {
#endif

static const char *GetServiceName(Service *service);
static BOOL ServiceInitialize(Service *service, Identity identity);
static TaskConfig GetServiceTaskConfig(Service *service);
static BOOL ServiceMessageHandle(Service *service, Request *request);
static void ServiceMsgProcess(Request *request);
static int32_t SendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);

static UiService g_uiService = {
    .GetName = GetServiceName,
    .Initialize = ServiceInitialize,
    .MessageHandle = ServiceMessageHandle,
    .GetTaskConfig = GetServiceTaskConfig,
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .SendMsg = SendMsg,
    DEFAULT_IUNKNOWN_ENTRY_END,
    .identity = { -1, -1, nullptr },
};

const int DATA_DIST_STACK_SIZE = 0xC00;
const int DATA_DIST_MAIL_LENGTH = 10;
const int BYTE_OFFSET = 8;
static uint32_t g_interval = 5;

void ScreenTurnOn(int32_t timeout)
{
    if (OHOS::HealthModel::GetInstance()->GetBriScrStatus() == 1) {
        return;
    }
#if defined(SUPPORT_POWER_MANAGER)
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScreenTurn] timeout = %d", timeout);
    g_interval = get_back_to_home_interval();
    set_back_to_home_interval(0xFFFFFFFF);
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScreenTurn] turn_on_screen");
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(timeout);
#endif
    return;
}

void ScreenTurnOff(void)
{
    if (OHOS::HealthModel::GetInstance()->GetBriScrStatus() == 1) {
        return;
    }
#if defined(SUPPORT_POWER_MANAGER)
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScreenTurnOff] timeout auto");
    set_back_to_home_interval(g_interval);
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->set_screen_set_keepon_timeout(0);
#endif
    return;
}

void ScreenOffEnable(int32_t enable)
{
#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->set_auto_timeout_function(enable);
#endif
}

void ConnectNewPhone(void)
{
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
    lwip_close_bt_pan();
#endif
    unsigned int connedDevicesNum = 0;
    bool ret = gap_get_paired_devices_num(&connedDevicesNum);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ConnectNewPhone paired_num, ret = %d, num = %d", ret, connedDevicesNum);

    if (connedDevicesNum != 0) {
        gap_paired_device_info_t *devInfo = (gap_paired_device_info_t *)malloc(sizeof(gap_paired_device_info_t) * connedDevicesNum);
        if (devInfo == nullptr) {
            return;
        }
        ret = gap_get_paired_devices_list(devInfo, (int *)&connedDevicesNum);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RemovePair gap_get_paired_devices_list ret = %d", ret);

        for (int i = 0; i < static_cast<int>(connedDevicesNum); ++i) {
            if ((gap_get_device_class((const bd_addr_t *)devInfo[i].addr.addr) & 0x200) != 0) {
                gap_remove_pair((const bd_addr_t *)devInfo[i].addr.addr);
                continue;
            }
        }
        free(devInfo);
    }

    (void)gap_ble_remove_all_pairs();

    ret = gap_br_set_bt_scan_mode(GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE, 60); // start scan
    if (!ret) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ConnectPhonePresenter bt gap_br_set_bt_scan_mode ret = 0x%x", ret);
    }
}

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    BOOL result = sm->RegisterService((Service *)&g_uiService);
    sm->RegisterDefaultFeatureApi(UI_SERVICE, GET_IUNKNOWN(g_uiService));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_UI_SERVICE, "UiService starts %s", result ? "successfully" : "unsuccessfully");
}

SYSEX_SERVICE_INIT_PRI(Init, 2);

const char *GetServiceName(Service *service)
{
    (void)service;
    return UI_SERVICE;
}

BOOL ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    UiService *uiService = reinterpret_cast<UiService *>(service);
    uiService->identity = identity;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_UI_SERVICE, "UiService init serviceId = %d, featureId = %d, queueId = %d",
                  identity.serviceId, identity.featureId, identity.queueId);
    return TRUE;
}

void ServiceMsgProcess(Request *request)
{
    switch (request->msgId) {
        case JS_INSTALL_SUCESS_TO_UI:
        case JS_UNINSTALL_SUCESS_TO_UI:
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_UI_SERVICE, "bms msg_id = %d bundleName = %s",
                          request->msgId, ((BundleData *)request->data)->bundleName);
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::ApplistModel::ApplistUpdate, &OHOS::ApplistModel::GetInstance()));
            break;
        case BT_CONNECTED_TO_UI:
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::UpdateAclConnectStatus,
                    OHOS::SettingBluetoothModel::GetInstance(), true));
            break;
        case BT_DISCONNECTED_TO_UI:
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::UpdateAclConnectStatus,
                    OHOS::SettingBluetoothModel::GetInstance(), false));
            break;
        case UI_ADD_SLEEP_VETO:
            uapi_pm_add_sleep_veto(PM_ID_SCREEN);
            break;
        case UI_RM_SLEEP_VETO:
            uapi_pm_remove_sleep_veto(PM_ID_SCREEN);
            break;
        case CONNECT_NEW_PHONE:
            ConnectNewPhone();
            break;
        case UI_UPDATE_LANGUAGE:
            OHOS::UILanguage::UpdateLanguage();
            break;
        case UI_OTA_UPDATE:
            power_display_svr_get_api()->turn_on_screen();
            OHOS::NativeAbility::GetInstance().SwitchSlice(VIEW_SETTING, OHOS::SettingPageId::SYSTEM_OTA_PAGE,
                OHOS::TransitionType::TRANSITION_INVALID, true);
            break;
    }
}

BOOL ServiceMessageHandle(Service *service, Request *request)
{
    (void)service;
    if (request == nullptr) {
        return FALSE;
    }

    ServiceMsgProcess(request);
    return TRUE;
}

TaskConfig GetServiceTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, DATA_DIST_STACK_SIZE, DATA_DIST_MAIL_LENGTH, SINGLE_TASK};
    return config;
}

static int32_t SendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen)
{
    int32_t ret;
    void *sendData = nullptr;
    if (dataLen > 0) {
        sendData = malloc(dataLen);
        if (sendData == nullptr) {
            return FALSE;
        }
        ret = memcpy_s(sendData, dataLen, data, dataLen);
        if (ret != EOK) {
            free(sendData);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "uiservice sendMsg memcpy_s fail! ret = %d", ret);
            return FALSE;
        }
    }

    Request request = {
        .msgId = msgId,
        .len = dataLen,
        .msgValue = msgValue,
        .data = sendData,
    };

    ret = SAMGR_SendRequest(IUNKNOWN_GetIdentity(GET_IUNKNOWN(g_uiService)), &request, nullptr);
    if (ret != 0) {
        if (sendData != nullptr) {
            free(sendData);
        }
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_UI_SERVICE, "SendMsg failed ret = %d ", ret);
        return FALSE;
    }

    return TRUE;
}

void SendMsgToUIService(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen)
{
    UiServiceFeatureApi *comApi = nullptr;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(UI_SERVICE);
    if (iUnknown == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_UI_SERVICE, "GetDefaultFeatureApi failed");
        return;
    }
    int result = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&comApi);
    if (result != 0 || comApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_UI_SERVICE, "QueryInterface comApi failed");
        return;
    }
    comApi->SendMsg(msgId, msgValue, data, dataLen);
    comApi->Release((IUnknown *)comApi);
}

#ifdef __cplusplus
}
#endif
