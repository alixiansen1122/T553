/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopModel
 * Create: 2025-06-01
 */
#ifndef SETTING_DESKTOP_MODEL_H
#define SETTING_DESKTOP_MODEL_H

#include <cstdint>
#include <string>
#include "gfx_utils/image_info.h"
#include "main/MainModel.h"
#include "settings/common/SettingDesktopHomeItemGroup.h"
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingRefSingleton.h"

typedef enum {
    APPLIST_STYLE,
    HEXAGONS_STYLE,
    FOOTBALL_STYLE,
    WATERFALL_STYLE,
    PLANET_STYLE,
    SPHERE_STYLE
} DesktopStyle;

namespace OHOS {
typedef struct {
    uint8_t     dialFlag;      /* 1:离线表盘 0:预制表盘 */
    uint8_t     id;            /* 仅预制表盘使用 */
    uint32_t    resId;         /* 仅离线表盘使用 */
    std::string dialFullName;  /* 仅离线表盘使用 */
    std::string imageFullName; /* 仅预制表盘使用 */
    std::string dialName;
    std::string description;   /* 仅离线表盘使用 */
    ImageInfo*  preImageInfo;
    uint8_t     capability;
} SetDialInfo;

enum class DialStyle {
    COVERFLOW = 0,
    ROLLER,
    STYLE_NUM
};

class SettingDesktopModel : public SettingRefSingleton<SettingDesktopModel> {
public:
    SettingDesktopModel();
    std::vector<SettingUnionItemData>& GetDesktopUnionData();
    std::vector<SettingDesktopHomeItemData>& GetDesktopHomeData();
    std::vector<SettingUnionItemData>& GetDialStyleUnionData();
    void RefreshDialStyleUnionDataForStyle();
    void RefreshDialStyleUnionDataForAod();
    // 桌面
    DesktopStyle GetDesktopStyle(void);
    void SetDesktopStyle(DesktopStyle style);
    uint8_t GetDesktopHomeDataSelect();
    void SetDesktopHomeDataSelect(DesktopStyle style);
    // 表盘风格
    DialStyle GetDialStyle();
    void SetDialStyle(DialStyle style);
    void SetDialStyleDataSelect(const char* DialStyleName);
    // 表盘
    void LoadAllDial(void);
    std::vector<SetDialInfo>& GetDialInfo(void);            // 表盘信息
    std::vector<SetDialInfo>& GetAmbientDialInfo(void);     // 息屏显示表盘信息 AOD(always-on display)
    void SaveDialSetting(uint8_t pageIndex);
    void SetSpecifyDialSetting(const DialSetting &setting); // 设置指定的dial
    void ClearDialInfo(void);
    uint8_t GetPageIndex(void);
protected:
    void LoadAllDialFromRam();
    void LoadAllDialFromFs();
    int32_t ParseDialHeader(const std::string &dialFileName, SetDialInfo &dialInfo);
private:
    std::vector<SettingUnionItemData> desktopUnionData_;
    std::vector<SettingDesktopHomeItemData> desktopHomeData_;
    std::vector<SettingUnionItemData> dialStyleUnionData_;
    std::vector<SetDialInfo> dialInfos_;
    std::vector<SetDialInfo> aodDialInfos_;
    bool hasSpecificDial_ = false;
    DialSetting specificDialSetting_;
};
}

#endif // SET_DESKTOP_MODEL_H