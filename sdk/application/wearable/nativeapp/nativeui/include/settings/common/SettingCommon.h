/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCommon
 * Create: 2025-06-01
 */
#ifndef SETTING_COMMON_H
#define SETTING_COMMON_H

#include <cstdint>
#include <vector>
#include "gfx_utils/vector.h"
#include "components/ui_view.h"
#include "components/ui_toggle_button.h"
#include "common/image_cache_manager.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "ui_resource_settings.h"

namespace OHOS {
enum SettingPageId : uint16_t {
    INVAILPAGE = 0,                     // 无效页面
    MAIN_PAGE = 1,                      // 主页面
    BLUETOOTH_PAGE = 2,                 // 蓝牙界面
    DISPLAY_PAGE  = 3,                  // 显示与亮度界面
    DESKTOP_PAGE = 4,                   // 表盘与桌面界面
    POWER_PAGE = 5,                     // 电池界面
    CARD_PAGE = 6,                      // 卡片界面
    SYSTEM_PAGE = 7,                    // 系统与升级界面
    ABOUT_PAGE = 8,                     // 关于界面
    DISPLAY_BRIGHTNESS_PAGE = 9,        // 显示与亮度 调节亮度界面
    DISPLAY_SCREEN_OFF_PAGE = 10,       // 显示与亮度 熄屏时间界面
    DISPLAY_STEADY_ON_PAGE = 11,        // 显示与亮度 长亮时刻界面
    DESKTOP_HOME_PAGE = 12,             // 表盘与桌面 桌面界面
    DESKTOP_DIAL_PAGE = 13,             // 表盘与桌面 表盘界面
    DESKTOP_DIAL_STYLE_PAGE = 14,       // 表盘与桌面 表盘风格界面
    CARD_EFFECT_PAGE = 15,              // 卡片管理   卡片动效页面
    CARD_MGR_PAGE = 16,                 // 卡片管理   卡片管理页面
    CARD_ADD_PAGE = 17,                 // 卡片管理   卡片添加页面
    SYSTEM_OPERATION_PAGE = 18,         // 系统与升级 关机、重启、恢复出厂设置界面
    SYSTEM_OTA_PAGE = 19,               // 系统与升级 OTA界面
    CONNECT_NEW_PHONE_PAGE = 20,        // 蓝牙  连接新手机界面
    CONNECT_OPEN_PAGE = 21,             // 蓝牙  连接新手机连接已打开界面
    CONNECT_BLUE_SUCCESS_PAGE = 22,     // 蓝牙  连接耳机蓝牙成功界面
    CONNECT_BLUE_FAIL_PAGE = 23,        // 蓝牙  连接耳机蓝牙失败界面
    RECONNECTION_PAGE = 24,             // 蓝牙  重连耳机和取消配对界面
    RECONNECT_SECOND_CONFIRM_PAGE = 25, // 蓝牙  重连耳机二次确认界面
    UNPAIR_SECOND_CONFIRM_PAGE = 26,    // 蓝牙  取消配对耳机二次确认界面
    QRCODE_CONN_PAGE = 27,              // 蓝牙  二维码连接
};

#define SETTING_LIST_DEFAULT_BG_COLOR    Color::GetColorFromRGB(32, 32, 32)     // 0xFF202020
#define SETTING_LIST_BLUETOOTH_BG_COLOR  Color::GetColorFromRGB(2, 142, 247)    // 0xFF028EF7
#define SETTING_LIST_DISPLAY_BG_COLOR    Color::GetColorFromRGB(253, 200, 42)   // 0xFFFDC82A
#define SETTING_LIST_DESKTOP_BG_COLOR    Color::GetColorFromRGB(98, 87, 251)    // 0xFF6257FB
#define SETTING_LIST_POWER_BG_COLOR      Color::GetColorFromRGB(102, 192, 94)   // 0xFF66C05
#define SETTING_LIST_CARD_BG_COLOR       Color::GetColorFromRGB(255, 128, 102)  // 0xFFFF8066
#define SETTING_LIST_SYSTEM_BG_COLOR     Color::GetColorFromRGB(0, 210, 185)    // 0xFF00D2B9
#define SETTING_LIST_ABOUT_BG_COLOR      Color::GetColorFromRGB(255, 255, 255)  // 0xFFFFFFFF

#define SETTING_LIST_DEFAULT_BG_COLOR_OPA   255 // 100%
#define SETTING_LIST_BLUETOOTH_BG_COLOR_OPA 63  // 24.61
#define SETTING_LIST_DISPLAY_BG_COLOR_OPA   63  // 24.85%
#define SETTING_LIST_DESKTOP_BG_COLOR_OPA   65  // 25.42%
#define SETTING_LIST_POWER_BG_COLOR_OPA     65  // 25.33%
#define SETTING_LIST_CARD_BG_COLOR_OPA      64  // 25.24%
#define SETTING_LIST_SYSTEM_BG_COLOR_OPA    63  // 24.66%
#define SETTING_LIST_ABOUT_BG_COLOR_OPA     38  // 14.85%

// 记录ItemGroup类中所有加载过的图片, 确保所有图片资源释放
class ItemGroupImageCache {
public:
    inline virtual ~ItemGroupImageCache()
    {
        for (auto imageId: imageIdList) {
            ImageCacheManager::GetInstance().UnloadOneInMultiRes(imageId, SETTING_IMAGE);
        }
    }

    inline ImageInfo* CacheImage(uint32_t imageId)
    {
        imageIdList.push_back(imageId);
        return ImageCacheManager::GetInstance().LoadOneInMultiRes(imageId, SETTING_IMAGE);
    }
private:
    std::vector<uint32_t> imageIdList;
};

}
#endif