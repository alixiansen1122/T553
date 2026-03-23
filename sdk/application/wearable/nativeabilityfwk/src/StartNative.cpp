#include "NativeAbility.h"
#include "NativeLauncher.h"
#include "StartNative.h"
#include "graphic_service_wrapper.h"
#include "adapter.h"

void StartNative() {
    OHOS::NativeLauncher::GetInstance().InitNativeLauncher();
    HideGraphicLogo();
    Want *info = static_cast<Want *>(AdapterMalloc(sizeof(Want)));
    memset_s(info, sizeof(Want), 0, sizeof(Want));
    OHOS::NativeAbility::GetInstance().OnActive(*info);
    AdapterFree(info);
}
