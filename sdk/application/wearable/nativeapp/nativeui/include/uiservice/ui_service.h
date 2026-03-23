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

#ifndef OHOS_UI_SERVICE_H
#define OHOS_UI_SERVICE_H

#include <cstdint>
#include "service.h"
#include "iunknown.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_SERVICE "uiservice"

typedef struct UiServiceFeatureApi {
    INHERIT_IUNKNOWN;
    int32_t (*SendMsg)(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);
} UiServiceFeatureApi;

typedef struct UiService UiService;

struct UiService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(UiServiceFeatureApi);
    Identity identity;
};

typedef enum {
    JS_INSTALL_SUCESS_TO_UI,
    JS_UNINSTALL_SUCESS_TO_UI,
    BT_CONNECTED_TO_UI,
    BT_DISCONNECTED_TO_UI,
    UI_ADD_SLEEP_VETO,
    UI_RM_SLEEP_VETO,
    UI_UPDATE_LANGUAGE,
    CONNECT_NEW_PHONE,
    UI_OTA_UPDATE,
} UiServiceMsgId;

#define BUNDLE_NAME_LEN 128
#define BUNDLE_PATH_LEN 255
typedef struct {
    char bundleName[BUNDLE_NAME_LEN];
    char label[BUNDLE_PATH_LEN];
    char smallIconPath[BUNDLE_PATH_LEN];
    char bigIconPath[BUNDLE_PATH_LEN];
} BundleData;

void SendMsgToUIService(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);

void ScreenTurnOn(int32_t timeout);
void ScreenTurnOff(void);
void ScreenOffEnable(int32_t enable);

#ifdef __cplusplus
}
#endif

#endif // OHOS_UiService_SERVICE_H
