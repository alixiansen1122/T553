/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */

#include "msg_center_utils.h"
#include "power_display_service.h"
#include "msg_center_customer.h"
#include "msg_center_cmd.h"
#include "errcode.h"
#include "bundle_install_msg.h"
#include "bundlems_slite_client.h"
#include "element_name.h"
#include "abilityms_slite_client.h"
#include <string.h>
#include "appexecfwk_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool g_isForciblyScreen = false;

void msg_center_turn_on_screen(void)
{
#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(30000);  // 30000ms
#endif
}

errcode_t msg_center_start_js(char *bundleName)
{
    msg_center_turn_on_screen();
#ifdef JS_ENABLE
    StartJsApp(bundleName);
#endif
    return ERRCODE_SUCC;
}

errcode_t msg_center_send_response(uint8_t cmd_id, uint8_t type, uint8_t *usr_data, uint16_t usr_len)
{
    return msg_center_send_data(cmd_id, type, usr_data, usr_len);
}

bool msg_center_is_forcibly_screen()
{
    return g_isForciblyScreen;
}

void msg_center_forcibly_screen(bool isForciblyScreen)
{
    g_isForciblyScreen = isForciblyScreen;
    return;
}

errcode_t msg_center_install_js_app(char *bundleName)
{
    return ERRCODE_SUCC;
}

bool msg_center_is_js_app_install(char *bundleName)
{
    if (bundleName == nullptr) {
        return false;
    }
    BundleInfo bundleInfo;
    uint8_t ret = OHOS::BundleMsClient::GetInstance().GetBundleInfo(bundleName, false, &bundleInfo);
    if (ret == ERR_OK) {
        return true;
    }
    
    return false;
}

bool msg_center_has_js_app_package(char *bundleName)
{
    return true;
}

bool msg_center_is_js_app_front(char *bundleName)
{
    ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
    if (elementName != nullptr && strcmp(elementName->bundleName, bundleName) == 0) {
        return true;
    }
    return false;
}

int32_t msg_center_get_js_app_version(char *bundleName)
{
    BundleInfo bundleInfo;
    int ret = OHOS::BundleMsClient::GetInstance().GetBundleInfo(bundleName, 0, &bundleInfo);
    if (ret == 0) {
        return bundleInfo.versionCode;
    } else {
        return -1;
    }
}

#ifdef __cplusplus
}
#endif