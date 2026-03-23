/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "router_module.h"
#include "ace_log.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#include "js_profiler.h"
#include "jsi/internal/jsi_internal.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"

namespace OHOS {
namespace ACELite {
void InitRouterModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "replace", RouterModule::Replace);
    JSI::SetModuleAPI(exports, "replaceToNative", RouterModule::ReplaceToNative);
}

JSIValue RouterModule::Replace(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    if (argsNum != 1 || args == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Replace args invalid, args num(%d).", argsNum);
        return AS_JSI_VALUE(jerry_create_error(JERRY_ERROR_TYPE,
            reinterpret_cast<const jerry_char_t *>("params should only be one object.")));
    }
    jerry_value_t object = AS_JERRY_VALUE(args[0]);
    // router.replace({uri: 'About', params: {id:'1'}}
    JsAppContext* appContext = JsAppContext::GetInstance();
    const JSAbilityImpl* topJsAbilityImpl = appContext->GetTopJSAbilityImpl();
    if (topJsAbilityImpl == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "topJsAbilityImpl is null.");
        return AS_JSI_VALUE(UNDEFINED);
    }
    // get top ability's router
    Router* router = const_cast<Router *>(topJsAbilityImpl->GetRouter());
    if (router == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "router is null.");
        return AS_JSI_VALUE(UNDEFINED);
    }
    jerry_value_t replaceResult = router->Replace(object);
    return AS_JSI_VALUE(replaceResult);
}

JSIValue RouterModule::ReplaceToNative(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    uint16_t sliceId = 0;
    // router.replaceToNative({sliceId: 1}}
    JSIValue jsId = JSI::GetNamedProperty(args[0], "sliceId");
    // router.replaceToNative({sliceName: "APP_LIST"}}
    JSIValue jsName = JSI::GetNamedProperty(args[0], "sliceName");

    // If sliceId and sliceName are contained, only sliceId is used.
    if (!JSI::ValueIsUndefined(jsId)) {
        sliceId = static_cast<uint16_t>(JSI::ValueToNumber(jsId));
    } else if (!JSI::ValueIsUndefined(jsName)) {
        char *sliceName = JSI::ValueToString(jsName);
        if (sliceName == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "ReplaceToSlice sliceName is invalid");
            return JSI::CreateUndefined();
        }

        if (strcmp(sliceName, "LAUNCHER") == 0) {
            sliceId = VIEW_MAIN_SAMPLE;
        } else if (strcmp(sliceName, "APP_LIST") == 0) {
            sliceId = VIEW_APPLIST;
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "ReplaceToSlice sliceName mismatch");
            JSI::ReleaseString(sliceName);
            return JSI::CreateUndefined();
        }
        JSI::ReleaseString(sliceName);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "ReplaceToSlice args is nullptr");
        return JSI::CreateUndefined();
    }

    OHOS::NativeAbility::GetInstance().ChangeSlice(sliceId);
    return JSI::CreateUndefined();
}
} // namespace ACELite
} // namespace OHOS
