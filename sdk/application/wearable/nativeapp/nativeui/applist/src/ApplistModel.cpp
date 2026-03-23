/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistModel.cpp
 * Author:
 * Create: 2021-09-18
 */

#include <cstdint>
#include "mem_util.h"
#include "bundle_info_utils.h"
#include "securec.h"
#include "utils.h"
#include "bundlems_slite_client.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "applist/ApplistModel.h"
#include "applist/ApplistPresenter.h"
#include "font/font_global_manager.h"
#include "NativeRegisterManager.h"

namespace OHOS {
    #define FOOTBALL_PLANE_MAX_NUM 20

    const AppItem g_nativeAppList[] = {
    };

    ApplistModel &ApplistModel::GetInstance(void)
    {
        static ApplistModel instance;
        return instance;
    }

    ApplistModel::ApplistModel()
    {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ApplistModel::ApplistModel()");
    }

    ApplistModel::~ApplistModel()
    {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ApplistModel::~ApplistModel()");
    }


    void ApplistModel::Init()
    {
        if (!initFlag) {
            ResolvingJSAppItems();
            initFlag = true;
        }
    }

    void ApplistModel::SetInitFlag(bool flag)
    {
        initFlag = flag;
    }

    void ApplistModel::CleanupApplistStrings(uint8_t nativeAppNumTotal)
    {
        for (uint8_t i = nativeAppNumTotal; i < appLen; i++) {
                free((void*)applist[i].iconSmall);
                free((void*)applist[i].iconHexagon);
                free((void*)applist[i].label);
                free((void*)applist[i].uid);
                applist[i].iconSmall = nullptr;
                applist[i].iconHexagon = nullptr;
                applist[i].label = nullptr;
                applist[i].uid = nullptr;
        }
    }

    void ApplistModel::ResolvingJSAppItems(void)
    {
        List<AppItem>& nativeAppLists = NativeRegisterManager::GetInstance().GetNativeAppLists();
        uint8_t registerNativeAppNum = nativeAppLists.Size();
        if (registerNativeAppNum > APP_MAX_NUM) {
            registerNativeAppNum = APP_MAX_NUM;
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Numbers of Native app is more than APP_MAX_NUM.\r");
        }
        uint8_t nativeAppNumTotal = registerNativeAppNum;
        ListNode<AppItem>* appNode = nativeAppLists.Head();
        for (int32_t index = 0; index < nativeAppNumTotal; index++) {
            applist[index] = appNode->data_;
            appNode = appNode->next_;
        }
        CleanupApplistStrings(nativeAppNumTotal);
        bundleLen = 0;
        appLen = 0;
        appLen = nativeAppNumTotal;
#ifdef JS_ENABLE
        int32_t len = 0;
        uint8_t ret = BundleMsClient::GetInstance().GetBundleInfos(0, &bundleInfos, &len);
        if ((ret == 0) && (len > 0) && (bundleInfos != nullptr)) {
            bundleLen = len;
            if (len > APP_MAX_NUM - nativeAppNumTotal) {
                len = APP_MAX_NUM - nativeAppNumTotal;
            }
            while (len--) {
                char* smallIconPath = strdup(bundleInfos[len].smallIconPath);
                char* bigIconPath = strdup(bundleInfos[len].bigIconPath);
                char* label = strdup(bundleInfos[len].label);
                char* bundleName = strdup(bundleInfos[len].bundleName);
                applist[appLen] = AppItem(
                    VIEW_EXTERN_APP,       // id
                    smallIconPath,         // iconSmall
                    bigIconPath,           // iconHexagon
                    label,                 // label
                    bundleName             // uid
                );
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResolvingJSAppItems bundleName is %s \r", bundleName);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResolvingJSAppItems label is %s \r", label);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResolvingJSAppItems smallIconPath is %s \r", smallIconPath);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResolvingJSAppItems bigIconPath is %s \r", bigIconPath);
                appLen++;
                bundleInfos[len].abilityInfo = nullptr;
            }
        } else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ResolvingJSAppItems GetBundleInfos ret = %d \r", ret);
        }
        if (bundleInfos != nullptr) {
            for (uint8_t i = 0; i < bundleLen; i++) {
                ClearBundleInfo(bundleInfos + i);
            }
            if (bundleInfos != nullptr) {
                AdapterFree(bundleInfos);
                bundleInfos = nullptr;
            }
        }
#endif
    }

    const AppItem *ApplistModel::GetApplistItems() const
    {
        return applist;
    }

    uint8 ApplistModel::GetAppListNum() const
    {
        return appLen;
    }
    void ApplistModel::SetStartIndex(uint16_t index)
    {
        startIndex = index;
    }

    uint16_t ApplistModel::GetStartIndex(void)
    {
        return startIndex;
    }

    void ApplistModel::SetLastOffset(int16_t x, int16_t y)
    {
        lastOffsetX = x;
        lastOffsetY = y;
    }

    void ApplistModel::GetLastOffset(int16_t& x, int16_t& y)
    {
        x = lastOffsetX;
        y = lastOffsetY;
    }

    void ApplistModel::SetHeadItemY(int y)
    {
        headItemY = y;
    }

    int ApplistModel::GetHeadItemY(void)
    {
        return headItemY;
    }

    void ApplistModel::ApplistUpdate(void)
    {
        ResolvingJSAppItems();
        if (presenter != nullptr) {
            presenter->GetAppListView()->ClearAppItemToList();
            const AppItem *itemInfo = GetApplistItems();
            for (uint8 i = 0; i < GetAppListNum(); i++) {
                presenter->GetAppListView()->AddAppItemToList(itemInfo[i]);
            }
            presenter->GetAppListView()->RefreshAppList();
        }
    }

    void ApplistModel::SetPresenter(ApplistPresenter *p)
    {
        presenter = p;
    }
}
