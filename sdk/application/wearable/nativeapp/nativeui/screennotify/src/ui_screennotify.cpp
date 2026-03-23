#include <list>
#include <cstdio>
#include "ability_state.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "Ability.h"
#include "wearable_log.h"
#include "graphic_service.h"
#include "power_display_service.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "main/MainViewSample.h"
#include "screennotify/ui_screennotify.h"
#include "components/root_view.h"
#include "notification_manager.h"
#include "main/DialBinTypesV2.h"
#include "hal_tick.h"
#include "abilityms_slite_client.h"

#ifdef __cplusplus
extern "C" {
#endif
static uint32_t g_back_to_home_interval = 5;
static uint32_t g_back_to_pre_slice = 5;
static uint32_t g_last_aod_on_stamp = 0;
static std::string g_pre_bundle_name = "com.huawei.launcher";
static bool g_is_native = true;

// 需要屏蔽掉的slice，ignore_slice_ids数组中的slice灭屏超时后不会调转到默认界面
uint16_t ignore_slice_ids[] = {
    VIEW_INVALID,
    VIEW_SETTING,
    VIEW_UISAMPLE,
#ifdef SUPPORT_ALIPAY_SEC
    VIEW_MAIN_ALIPAY,
#endif
};

static std::list<ScreenStatusNotifyCallback> g_callback;

static void ChangeToHomeView(void)
{
    screen_context_t *sc = power_display_get_screen_context();
    uint16_t current_slice_id = OHOS::NativeAbility::GetInstance().GetCurSliceId();
    for (uint16_t i = 0; i < sizeof(ignore_slice_ids) / sizeof(uint16_t); i++) {
        if (ignore_slice_ids[i] == current_slice_id) {
            return;
        }
    }
    if (sc->screen_off_time_interval > g_back_to_home_interval) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "screen off time = %u",
                      sc->screen_off_time_interval);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GetNativeAbilityState = %u",
                      OHOS::NativeAbility::GetInstance().GetNativeAbilityState());
        if (OHOS::NativeAbility::GetInstance().GetNativeAbilityState() == STATE_ACTIVE) {
            if (current_slice_id != VIEW_MAIN_SAMPLE) { // 主表盘
                OHOS::NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            } else {
                if (!OHOS::MainViewSample::GetInstance()->IsMainClockPage()) {
                    // 当前页面为主表盘的其他页面时，切换回主表盘页面
                    OHOS::MainViewSample::GetInstance()->SwitchToClockPage();
                }
            }
        }
    }
}

static void EnterAod()
{
    using namespace OHOS;
    g_last_aod_on_stamp = HALTick::GetInstance().GetTime();
    if (NativeAbility::GetInstance().GetNativeAbilityState() == STATE_ACTIVE) {
        g_is_native = true;
    } else {
        ElementName *elementName = AbilityMsClient::GetInstance().GetTopAbility();
        if (elementName != nullptr) {
            g_pre_bundle_name = elementName->bundleName;
        }
        g_is_native = false;
    }
    NativeAbility::GetInstance().EnterAod();
}

static void ExitAod()
{
    using namespace OHOS;
    uint32_t aodDur = HALTick::GetInstance().GetElapseTime(g_last_aod_on_stamp) / 1000;  // 1000: 1s = 1000ms
    if (aodDur > g_back_to_pre_slice) {
        uint16_t sliceId = NativeAbility::GetInstance().GetPreTargetId() & SLICE_MASK;
        for (uint16_t i = 0; i < sizeof(ignore_slice_ids) / sizeof(uint16_t); i++) {
            if (ignore_slice_ids[i] == sliceId) {
                NativeAbility::GetInstance().ExitAod(sliceId);
                return;
            }
        }
        if (OHOS::MainViewSample::GetInstance() != nullptr && !OHOS::MainViewSample::GetInstance()->IsMainClockPage()) {
            // 当前页面为主表盘的其他页面时，切换回主表盘页面
            OHOS::MainViewSample::GetInstance()->SwitchToClockPage();
        }
        NativeAbility::GetInstance().ExitAod(VIEW_MAIN_SAMPLE);
    } else {
        if (!g_is_native) {
            Want *want = new Want();
            memset_s(want, sizeof(Want), 0, sizeof(Want));
            ElementName startElement = {};
            startElement.abilityName = "default";
            startElement.bundleName = const_cast<char *>(g_pre_bundle_name.c_str());
            startElement.deviceId = nullptr;
            SetWantElement(want, startElement);
            StartAbility(want);
            ClearWant(want);
            delete want;
            return;
        }
        uint32_t preId = NativeAbility::GetInstance().GetPreTargetId();
        NativeAbility::GetInstance().ExitAod(preId);
    }
}

void notify_screen_on_event(void)
{
    for (auto callback : g_callback) {
        callback(SCREEN_STATUS_ON);
    }
    ChangeToHomeView();
    if (GraphicService::GetInstance()->IsNativeRunning() && !OHOS::NotificationManager::GetInstance()->HasNotifyShowing()) {
        OHOS::NativeAbility::GetInstance().ResumeSlice(); // ensure that don't resume twice if switch to home
    }
}

void notify_screen_off_event(void)
{
    for (auto callback : g_callback) {
        callback(SCREEN_STATUS_OFF);
    }
    if (GraphicService::GetInstance()->IsNativeRunning()) {
        OHOS::NativeAbility::GetInstance().StopSliceTransition(); // make sure to end slice transition before turning screen off.
        OHOS::NativeAbility::GetInstance().PauseSlice();
    }
}

void notify_screen_aod_on_event(void)
{
    EnterAod();
}

void notify_screen_aod_off_event(void)
{
    ExitAod();
}

void set_back_to_home_interval(uint32_t time_sec)
{
    g_back_to_home_interval = time_sec;
}

uint32_t get_back_to_home_interval(void)
{
    return g_back_to_home_interval;
}

void set_back_to_pre_slice_interval(uint32_t time_sec)
{
    g_back_to_pre_slice = time_sec;
}

uint32_t get_back_to_pre_slice_interval(void)
{
    return g_back_to_pre_slice;
}

void registerNotifyScreenStatus(ScreenStatusNotifyCallback callback)
{
    if (callback != nullptr) {
        g_callback.push_back(callback);
    }
}

void deregisterNotifyScreenStatus(ScreenStatusNotifyCallback callback)
{
    if (callback != nullptr) {
        g_callback.remove(callback);
    }
}

#ifdef __cplusplus
}
#endif
