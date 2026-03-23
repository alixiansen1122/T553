/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef LOCATOR_H
#define LOCATOR_H

#include <vector>
#include "constant_definition.h"
#include "i_gnss_status_callback.h"
#include "i_locator_callback.h"
#include "i_nmea_message_callback.h"
#include "request_config.h"

namespace OHOS {
namespace Location {

class Locator {
public:
    static Locator &GetInstance();

    /**
     * @brief Obtain current location switch status.
     *
     * @param isEnabled Indicates if the location switch on.
     * @return Returns ERRCODE_SUCCESS if obtain current location switch status succeed.
     */
    LocationErrCode IsLocationEnabled(bool &isEnabled);

    /**
     * @brief Enable location switch.
     *
     * @param enable Status of the location switch to be set.
     * @return Returns ERRCODE_SUCCESS if enable location switch succeed.
     */
    LocationErrCode EnableAbility(bool enable);

    /**
     * @brief Subscribe location changed.
     *
     * @param requestConfig Indicates the location request parameters.
     * @param callback Indicates the callback for reporting the location result.
     * @return Returns ERRCODE_SUCCESS if subscribe location changed succeed.
     */
    LocationErrCode StartLocating(const RequestConfig *requestConfig,
        ILocatorCallback *callback);

    /**
     * @brief Unsubscribe location changed.
     *
     * @param callback Indicates the callback for reporting the location result.
     * @return Returns ERRCODE_SUCCESS if Unsubscribe location changed succeed.
     */
    LocationErrCode StopLocating(ILocatorCallback *callback);

    /**
     * @brief Subscribe satellite status changed.
     *
     * @param callback Indicates the callback for reporting the satellite status.
     * @return Return ERRCODE_SUCCESS if the registration is successful.
     */
    LocationErrCode RegisterGnssStatusCallback(IGnssStatusCallback *callback);

    /**
     * @brief Unsubscribe satellite status changed.
     *
     * @param callback Indicates the callback for reporting the satellite status.
     * @return Return ERRCODE_SUCCESS if the deregistration is successful.
     */
    LocationErrCode UnregisterGnssStatusCallback(IGnssStatusCallback *callback);

    /**
     * @brief Subscribe nmea message changed.
     *
     * @param callback Indicates the callback for reporting the nmea message.
     * @return Return ERRCODE_SUCCESS if the registration is successful.
     */
    LocationErrCode RegisterNmeaMessageCallback(INmeaMessageCallback *callback);

    /**
     * @brief Unsubscribe nmea message changed.
     *
     * @param callback Indicates the callback for reporting the nmea message.
     * @return Return ERRCODE_SUCCESS if the deregistration is successful.
     */
    LocationErrCode UnregisterNmeaMessageCallback(INmeaMessageCallback *callback);

    /**
     * @brief Querying location support coordinate system type.
     *
     * @param coordinateSystemTypes Indicates location support coordinate system type.
     * @return Return ERRCODE_SUCCESS if querying location support coordinate system type is successful.
     */
    LocationErrCode QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes);

    /**
     * @brief Querying location privacy protocol confirmation status,Not supported.
     *
     * @param type Indicates location privacy protocol type.
     * @param isConfirmed Indicates if the location privacy protocol has been confirmed
     * @return Return ERRCODE_SUCCESS if querying location privacy protocol confirmation status is successful.
     */
    LocationErrCode IsLocationPrivacyConfirmed(const int type, bool &isConfirmed);

    /**
     * @brief Set location privacy protocol confirmation status,Not supported.
     *
     * @param type Indicates location privacy protocol type.
     * @param isConfirmed Indicates whether the location privacy protocol should be confirmed.
     * @return Return ERRCODE_SUCCESS if set location privacy protocol confirmation status is successful.
     */
    LocationErrCode SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed);

    /**
     * @brief Send extended commands to location subsystem,Not supported.
     *
     * @param commands Indicates the extended command message body.
     * @return Returns ERRCODE_SUCCESS if the command has been sent successfully.
     */
    LocationErrCode SendCommand(const LocationCommand *commands);

private:
    Locator();
    ~Locator();
};

}  // namespace Location
}  // namespace OHOS
#endif // LOCATOR_H
