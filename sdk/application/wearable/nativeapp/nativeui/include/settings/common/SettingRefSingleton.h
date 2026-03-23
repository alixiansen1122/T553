/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingRefSingleton
 * Create: 2025-06-01
 */
#ifndef SETTING_REF_SINGLETON_H
#define SETTING_REF_SINGLETON_H

namespace OHOS {
template<typename T>
class SettingRefSingleton {
public:
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }
protected:
    SettingRefSingleton() = default;
    virtual ~SettingRefSingleton() = default;
    SettingRefSingleton(const SettingRefSingleton&) = delete;
    SettingRefSingleton& operator=(const SettingRefSingleton&) = delete;
};
} // OHOS
#endif