/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: NativeManager
 * Author:
 * Create: 2024-07
 */

#ifndef NATIVE_MANAGER_H
#define NATIVE_MANAGER_H

#include "gfx_utils/vector.h"
#include "gfx_utils/list.h"
#include "NativeRegisterType.h"
#include "SliceProxy.h"
#include "AbilitySliceProxy.h"
#include "applist/ApplistModel.h"
#include "NativeAbility.h"
#include "wearable_log.h"
#include "UiConfig.h"

namespace OHOS {
class NativeRegisterCallback {
public:
    NativeRegisterCallback() {}

    virtual ~NativeRegisterCallback() = default;
    virtual void InitSlice() {};
};

struct SliceMap {
    uint16_t id;
    SliceProxy* slice;
};

class NativeRegisterManager {
public:
    static NativeRegisterManager& GetInstance()
    {
        static NativeRegisterManager instance;
        return instance;
    }
    void InitAllNativesSlices();
    List<AppItem>& GetNativeAppLists();
    void RegisterSlice(const uint16_t id, SliceProxy* slice);
    void RegisterMenu(const uint16_t id, const char *icon, const char *iconHexagon, const char *label);
    void RegisterMenu(const AppItem &item);
    bool IsSliceIdExist(AppViewId id);

private:
    NativeRegisterManager() {};
    ~NativeRegisterManager() {};
    NativeRegisterManager(const NativeRegisterManager&) = delete;
    NativeRegisterManager &operator=(const NativeRegisterManager&) = delete;
    Graphic::Vector<SliceMap> slices_;
    List<AppItem> nativeAppLists_;
};

class SliceRegister {
public:
    SliceRegister(const uint16_t id, SliceProxy* sliceProxy)
    {
        NativeRegisterManager::GetInstance().RegisterSlice(id, sliceProxy);
    }
    SliceRegister(const uint16_t id, SliceProxy* sliceProxy,
                  const char *icon, const char *iconHexagon, const char *label)
    {
        NativeRegisterManager::GetInstance().RegisterSlice(id, sliceProxy);
        NativeRegisterManager::GetInstance().RegisterMenu(id, icon, iconHexagon, label);
    }
    SliceRegister(const AppItem &item, SliceProxy* sliceProxy)
    {
        NativeRegisterManager::GetInstance().RegisterSlice(item.id, sliceProxy);
        NativeRegisterManager::GetInstance().RegisterMenu(item);
    }
};
} // namespace OHOS

#define REGIST_SLICE(id, V, P) \
    const static OHOS::SliceRegister (STATIC_VAR)(static_cast<uint16_t>(id), new AbilitySliceProxy<V, P>())
#define REGIST_MENU(id, V, P, icon, iconHexagon, label) \
    const static OHOS::SliceRegister (STATIC_VAR)(static_cast<uint16_t>(id), new AbilitySliceProxy<V, P>(), \
                                                  icon, iconHexagon, label)
/*
 * 目前支持两种模式的注册，如果需要扩展，请自行扩展AppItem的构造方法
 * 支持多语言注册以及多图标打包功能的注册示例：REGIST_MENU_EXT((AppItem{VIEW_OFFLINEVOICE, VOICE_RES_ID,
 * VOICE_HEXAGON_ID, RES_PATH, HEXAGON_PATH, VOICE_LANGUAGE_ID, nullptr}), MainVocassistView, VocassistPresenter);
 * 不支持多语言，一个图标使用一个文件的注册示例：REGIST_MENU_EXT((AppItem{VIEW_OFFLINEVOICE, PNG_APPLIST_VOCASSIST_IMAGE,
 * PNG_APPLIST_DEFAULT_IMG, "语音助手", nullptr}), MainVocassistView, VocassistPresenter);
 */
#define REGIST_MENU_EXT(item, V, P) \
    const static OHOS::SliceRegister (STATIC_VAR)(item, new AbilitySliceProxy<V, P>())
#endif // NATIVE_MANAGER_H