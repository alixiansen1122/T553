/*
 * Copyright (c) 2020-2021 CompanyNameMagicTag.
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

#include "NativeRegisterManager.h"
#include "NativeLauncher.h"
#include "SliceProxy.h"
#include "AbilitySliceProxy.h"
#include "wearable_log.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "activity/ActivityModel.h"
#include "player/PlayersModel.h"
#include "main/HealthModel.h"
#include "offlinevoice/VocassistModel.h"
#include "screensetting/ScreenModels.h"
#include "graphic_hardware_config.h"
#include "graphic_service.h"
#include "common/graphic_startup.h"
#ifdef JS_ENABLE
#include "product_adapter.h"
#endif
#ifdef ENABLE_MODULE_CALENDAR
#include "event_id_manager.h"
#endif
#include "RecentManager.h"
#include "PageTransitionMgr.h"
#include "date/DateModel.h"
#include "settings/model/SettingDisplayModel.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/model/SettingBluetoothHeadsetEvent.h"
#include "NativeLauncher.h"
namespace OHOS {
int16_t BackDragListener::maxDragRegionX_ = MAX_X_IN_DRAG_REGION;
NativeLauncher::NativeLauncher() {}
NativeLauncher::~NativeLauncher() {}

NativeLauncher &NativeLauncher::GetInstance(void)
{
    static NativeLauncher instance;
    return instance;
}

void NativeLauncher::InitNativeLauncher()
{
#ifdef JS_ENABLE
    ACELite::ProductAdapter::SetScreenSize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    ACELite::ProductAdapter::SetDefaultFontStyle(
        DEFAULT_VECTOR_FONT_FILENAME, ACELite::ProductAdapter::GetDefaultFontSize());
#endif
    GraphicStartUp::InitFontEngine(MEM_POOL_UI_FONT, MEM_POOL_UI_FONT_SIZE,
        OHOS::VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
    GraphicStartUp::InitFontEngine(MEM_POOL_UI_FONT, MEM_POOL_UI_FONT_SIZE,
        OHOS::VECTOR_FONT_DIR, BOLD_VECTOR_FONT_FILENAME);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeLauncher::InitNativeLauncher");
    // 初始化所有预加载slice
    SettingInitBtHeadsetEvent(); // 蓝牙ct和tg适配初始化
    NativeRegisterManager::GetInstance().InitAllNativesSlices();
    PhoneMenuCallerLogModel::GetInstance()->InitPhoneService();
    PlayersModel::GetInstance()->AudioInit();
    ActivityModel::GetInstance()->Init();
    ScreenModels::GetInstance()->InitKvStorage();
#ifdef ENABLE_MODULE_CALENDAR
    ACELite::InitEventMaps();
#endif
    VocassistModel::GetInstance()->Init();
    SettingBluetoothModel::GetInstance()->Init();
    // 初始化默认首页
    ScreenModels::GetInstance()->PreLoadAppimages();
    NativeAbility::GetInstance().SetDefaultSliceId(VIEW_MAIN_SAMPLE);

    RecentManager::GetInstance()->SetDirPath(APP_RECENT_PATH);
    RecentManager::GetInstance()->AddToBlackList(VIEW_AOD);
    RecentManager::GetInstance()->AddToBlackList(VIEW_RECENT_APP);
    RecentManager::GetInstance()->AddToBlackList(VIEW_APPLIST);
    RecentManager::GetInstance()->AddToBlackList(VIEW_MAIN_SAMPLE);
    RecentManager::GetInstance()->SetMaxSize(5); // 5: max number of recent apps.
    PageTransitionMgr::GetInstance().SetRecentMinPeriod(1000); // 1000: min period
    DateModel::GetInstance().Init();

    SettingDisplayModel::GetInstance().InitDisplayModel();
}
}
