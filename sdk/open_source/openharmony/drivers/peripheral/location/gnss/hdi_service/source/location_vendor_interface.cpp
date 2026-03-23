/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "location_vendor_interface.h"

#include "gnss_interface_impl.h"
#include "location_utils.h"
#include "location_log.h"

namespace OHOS {
namespace HDI {
namespace Location {

MutexId LocationVendorInterface::mutex_ = nullptr;
LocationVendorInterface* LocationVendorInterface::instance_ = nullptr;

LocationVendorInterface::LocationVendorInterface()
{
    Init();
    LBSLOGI(HDI_GNSS, "constructed.");
}

LocationVendorInterface::~LocationVendorInterface()
{
    CleanUp();
    LBSLOGI(HDI_GNSS, "destructed.");
}
 
LocationVendorInterface* LocationVendorInterface::GetInstance()
{
    if (instance_ == nullptr) {
        InitStaticMutexLock(&mutex_);
        AutoLock lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new LocationVendorInterface();
        }
    }
    return instance_;
}

void LocationVendorInterface::DestroyInstance()
{
    AutoLock lock(mutex_);
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

void LocationVendorInterface::Init()
{
    LBSLOGI(HDI_GNSS, "enter");
    GnssVendorDevice *gnssDevice = LocationVendorGetGnssVendorDevice();
    if (gnssDevice == nullptr) {
        LBSLOGE(HDI_GNSS, "GetGnssVendorDevice failed.");
        return;
    }
    vendorInterface_ = gnssDevice->getGnssInterface();
    if (vendorInterface_ == nullptr) {
        LBSLOGE(HDI_GNSS, "getGnssInterface failed.");
        return;
    }
}

GnssVendorInterface *LocationVendorInterface::GetGnssVendorInterface()
{
    if (vendorInterface_ == nullptr) {
        LBSLOGE(HDI_GNSS, "vendorInterface_ is null.");
    }
    return vendorInterface_;
}

void *LocationVendorInterface::GetModuleInterface(int moduleId)
{
    auto vendorInterface = GetGnssVendorInterface();
    if (vendorInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "can not get vendorInterface.");
        return nullptr;
    }
    auto moduleInterface = vendorInterface->getGnssModuleIface(moduleId);
    if (moduleInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "can not get moduleInterface.moduleId:%d", moduleId);
    }
    return moduleInterface;
}

void LocationVendorInterface::CleanUp()
{
    if (vendorInterface_ == nullptr) {
        return;
    }
    vendorInterface_ = nullptr;
}
} // Location
} // HDI
} // OHOS
