/*
 * Copyright (c) CompanyNameMagicTag Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: The application core startup logo display.
 * Author: CompanyName
 * Create: 2022-02-19
 */

#include "common_def.h"
#include "debug_print.h"
#include "system_init.h"
#include "ohosfwk_startup.h"
#ifdef ENABLE_UIKIT
#include "graphic_service_wrapper.h"
#endif
#ifdef SUPPORT_POWER_MANAGER
#include "power_display_service.h"
#endif
#include "soc_lcd_api.h"

void ohos_startup()
{
    /* 未接lcd时不能调用图形相关类的实例 */
    if (!uapi_is_lcd_connect()) {
        return;
    }

#ifdef ENABLE_UIKIT
    InitGraphicService();
#endif
#ifdef SUPPORT_POWER_MANAGER
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->turn_on_screen();
    display_api->set_screen_set_keepon_timeout(5000);
#endif
#ifdef ENABLE_UIKIT
    ShowGraphicLogo();
#endif
    OHOS_SystemInit();

#ifdef PUBLISH_TEST_DATA
    PublishTestData();
#endif
}
