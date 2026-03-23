/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: Restore Defaults Presenter
 * Create: 2024-08-12
 */

#include "restoredefaults/RestoreDefaultsPresenter.h"
#include "restoredefaults/RestoreDefaultsView.h"
#include "bts_br_gap.h"
#include "drag_event.h"
#include "cmsis_os.h"
#include "NativeRegisterManager.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "dfx_reboot.h"
#ifdef SUPPORT_ALIPAY_SEC
#include "alipay_feature.h"
#endif
#include "adapter.h"
#include "bundlems_slite_client.h"
#include "abilityms_slite_client.h"
#include "stopwatch/StopwatchModel.h"

namespace OHOS {

REGIST_SLICE(VIEW_RESTORE_DEFAULTS, RestoreDefaultsView, RestoreDefaultsPresenter);

static RestoreDefaultsPresenter *g_pRestoreDefaultsPresenter = nullptr;

RestoreDefaultsPresenter::RestoreDefaultsPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsPresenter::RestoreDefaultsPresenter");
    g_pRestoreDefaultsPresenter = this;
}

RestoreDefaultsPresenter::~RestoreDefaultsPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsPresenter::~RestoreDefaultsPresenter");
    g_pRestoreDefaultsPresenter = nullptr;
}

RestoreDefaultsPresenter *RestoreDefaultsPresenter::GetInstance()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsPresenter GetInstance");
    return g_pRestoreDefaultsPresenter;
}

bool RestoreDefaultsPresenter::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnClick OnClick");
    if (strcmp(view.GetViewId(), RESTORE_CANCEL_BUTTON) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CANCEL_BUTTON OnClick");
        NativeAbility::GetInstance().ChangeSlice(VIEW_SETTING);
    } else if (strcmp(view.GetViewId(), RESTORE_CONFIRM_BUTTON) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CONFIRM_BUTTON OnClick");
        ConfirmOnClick();
    }
    return true;
}

static void UninstallCallbackFunc(const uint8_t resultCode, const void *resultMessage)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UninstallCallbackFunc.");
}

void RestoreDefaultsPresenter::ConfirmOnClick()
{
    bool res = bluetooth_factory_reset();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bluetooth_factory_reset res:%u.", (uint32_t)res);
#ifdef SUPPORT_ALIPAY_SEC
    (void)uapi_alipay_svr_reset();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_alipay_svr_reset process.");
#endif

    BundleInfo *bundleInfos = nullptr;
    int32_t count = 0;
    int32_t ret = ERRCODE_FAIL;

    ret = OHOS::BundleMsClient::GetInstance().GetBundleInfos(0, &bundleInfos, &count);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GetBundleInfos count = %d bundleRet = 0x%x", count, ret);
    if (ret != 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Get bundleInfos Failed\n");
    }else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
            "----------------- AppList Start --------------------\n");
        for (int32_t i = 0; i < count; i++) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
            "App:%d-----------------\n"
            "label:            %s\n"
            "bundleName:       %s\n",
            i+1,
            bundleInfos[i].label,
            bundleInfos[i].bundleName
            );
            ElementName* topAbility = OHOS::AbilityMsClient::GetInstance().GetTopAbility();

            if ((topAbility != nullptr) && (strcmp(topAbility->bundleName, bundleInfos[i].bundleName) == 0)) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "js app is active, first need terminate");
                int token = 0;
                OHOS::AbilityMsClient::GetInstance().TerminateAbility(token);
            }

            InstallParam installParam = { .installLocation = 1, .keepData = false };
            OHOS::BundleMsClient::GetInstance().Uninstall(bundleInfos[i].bundleName, &installParam, UninstallCallbackFunc);
            FreeElement(topAbility);
        }
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
            "----------------- AppList End -----------------------\n");
    }
    if (bundleInfos != nullptr) {
        for (uint8_t i = 0; i < count; i++) {
            ClearBundleInfo(bundleInfos + i);
        }

        if (bundleInfos != nullptr) {
            AdapterFree(bundleInfos);
            bundleInfos = nullptr;
        }
    }
    uapi_system_reboot(SYSTEM_SOFT_REBOOT);
    return;
}

bool RestoreDefaultsPresenter::OnDrag(UIView &view, const DragEvent &event)
{
    std::string viewId = view.GetViewId();
    if (viewId == RESTORE_SELECT_VIEW) {
        if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsPresenter::OnDrag");
            NativeAbility::GetInstance().ChangeSlice(VIEW_SETTING);
        }
    }
    return true;
}
}
