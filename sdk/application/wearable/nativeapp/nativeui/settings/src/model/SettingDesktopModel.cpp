/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopModel
 * Create: 2025-06-01
 */
#include <algorithm>
#include "dirent.h"
#include "UiConfig.h"
#include "kv_store.h"
#include "common/image_cache_manager.h"
#include "wearable_log.h"
#include "main/DialBinParser.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingDesktopModel.h"

namespace OHOS {
static constexpr uint32_t STYLE_VALUE_LEN = 32;
static constexpr uint8_t SETTING_DIAL_STYLE_ROLLER_IDX = 1;
static constexpr uint8_t SETTING_DIAL_STYLE_COVERFLOW_IDX = 2;
static constexpr uint8_t SETTING_DIAL_STYLE_AOD_IDX = 3;

SettingDesktopModel::SettingDesktopModel()
{
    std::vector<SettingUnionItemData> tempDesktopUnionData = {{
        PlainTextItemData{"表盘与桌面"},
        IconTextNavigationItemData{SettingPageId::DESKTOP_DIAL_PAGE, IMAGE_DESKTOP_LIST_DIAL, "表盘",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DESKTOP_HOME_PAGE, IMAGE_DESKTOP_LIST_HOME, "桌面风格",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DESKTOP_DIAL_STYLE_PAGE, IMAGE_DESKTOP_LIST_DIAL_STYLE, "表盘风格",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
    }};
    desktopUnionData_.swap(tempDesktopUnionData);

    std::vector<SettingDesktopHomeItemData> tempDesktopHomeData = {{
        {APPLIST_STYLE, IMAGE_DESKTOP_HOME_SELECTED_APPLIST, IMAGE_DESKTOP_HOME_UNSELECT_APPLIST, "列表", false},
        {HEXAGONS_STYLE, IMAGE_DESKTOP_HOME_SELECTED_HEXAGONS, IMAGE_DESKTOP_HOME_UNSELECT_HEXAGONS, "蜂窝", false},
        {FOOTBALL_STYLE, IMAGE_DESKTOP_HOME_SELECTED_FOOTBALL, IMAGE_DESKTOP_HOME_UNSELECT_FOOTBALL, "足球", false},
        {WATERFALL_STYLE, IMAGE_DESKTOP_HOME_SELECTED_WATERFALL, IMAGE_DESKTOP_HOME_UNSELECT_WATERFALL, "瀑布", false},
        {PLANET_STYLE, IMAGE_DESKTOP_HOME_SELECTED_PLANET, IMAGE_DESKTOP_HOME_UNSELECT_PLANET, "星球", false},
    }};
    desktopHomeData_.swap(tempDesktopHomeData);

    std::vector<SettingUnionItemData> tempDialStyleUnionData = {{
        PlainTextItemData{"表盘风格"},
        IconTextToggleItemData{IMAGE_DESKTOP_DIALSTYLE_COVERFLOW, "coverflow", false,
            SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextToggleItemData{IMAGE_DESKTOP_DIALSTYLE_ROLLER, "roller", false,
            SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextToggleItemData{IMAGE_DESKTOP_DIALSTYLE_AOD, "息屏显示", false,
            SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
    }};
    dialStyleUnionData_.swap(tempDialStyleUnionData);
}

std::vector<SettingUnionItemData>& SettingDesktopModel::GetDesktopUnionData()
{
    return desktopUnionData_;
}

std::vector<SettingDesktopHomeItemData>& SettingDesktopModel::GetDesktopHomeData()
{
    DesktopStyle style = GetDesktopStyle();
    for (auto& it : desktopHomeData_) {
        it.isSelect = (it.style == style);
    }
    return desktopHomeData_;
}

std::vector<SettingUnionItemData>& SettingDesktopModel::GetDialStyleUnionData()
{
    RefreshDialStyleUnionDataForStyle();
    RefreshDialStyleUnionDataForAod();
    return dialStyleUnionData_;
}

void SettingDesktopModel::RefreshDialStyleUnionDataForStyle()
{
    DialStyle style = GetDialStyle();
    auto coverflowItemData = static_cast<IconTextToggleItemData*>(
        dialStyleUnionData_[SETTING_DIAL_STYLE_COVERFLOW_IDX].GetUnionItemData());
    auto rollerItemData = static_cast<IconTextToggleItemData*>(
        dialStyleUnionData_[SETTING_DIAL_STYLE_ROLLER_IDX].GetUnionItemData());
    coverflowItemData->selected = (style == DialStyle::COVERFLOW);
    rollerItemData->selected = (style == DialStyle::ROLLER);
}

void SettingDesktopModel::RefreshDialStyleUnionDataForAod()
{
    auto aodItemData = static_cast<IconTextToggleItemData*>(
        dialStyleUnionData_[SETTING_DIAL_STYLE_AOD_IDX].GetUnionItemData());
    aodItemData->selected = MainModel::GetInstance().IsAodEnabled();
}

DesktopStyle SettingDesktopModel::GetDesktopStyle(void)
{
    DesktopStyle style;
    char temp[STYLE_VALUE_LEN] = { 0 };
    (void)UtilsGetValue("applist_style", temp, STYLE_VALUE_LEN);
    if (strcmp(temp, "hexagons") == 0) {
        style = HEXAGONS_STYLE;
    } else if (strcmp(temp, "football") == 0) {
        style = FOOTBALL_STYLE;
    } else if (strcmp(temp, "waterfall") == 0) {
        style = WATERFALL_STYLE;
    } else if (strcmp(temp, "planet") == 0)
        style = PLANET_STYLE;
    else {
        style = APPLIST_STYLE;
    }
    return style;
}

void SettingDesktopModel::SetDesktopStyle(DesktopStyle style)
{
    if (style == APPLIST_STYLE) {
        (void)UtilsSetValue("applist_style", "list");
    } else if (style == HEXAGONS_STYLE) {
        (void)UtilsSetValue("applist_style", "hexagons");
    } else if (style == FOOTBALL_STYLE) {
        (void)UtilsSetValue("applist_style", "football");
    } else if (style == WATERFALL_STYLE) {
        (void)UtilsSetValue("applist_style", "waterfall");
    } else if (style == PLANET_STYLE) {
        (void)UtilsSetValue("applist_style", "planet");
    }
}

uint8_t SettingDesktopModel::GetDesktopHomeDataSelect()
{
    DesktopStyle style = GetDesktopStyle();
    for (size_t i = 0; i < desktopHomeData_.size(); i++) {
        if (desktopHomeData_[i].style == style) {
            return i;
        }
    }
    return 0;
}

void SettingDesktopModel::SetDesktopHomeDataSelect(DesktopStyle style)
{
    for (auto& it : desktopHomeData_) {
        it.isSelect = (it.style == style);
    }
    SetDesktopStyle(style);
}

DialStyle SettingDesktopModel::GetDialStyle()
{
    DialStyle style;
    char temp[STYLE_VALUE_LEN] = { 0 };
    (void)UtilsGetValue("set_dial_style", temp, STYLE_VALUE_LEN);
    if (strcmp(temp, "DialStyle::ROLLER") == 0) {
        style = DialStyle::ROLLER;
    } else { // "DialStyle::COVERFLOW" or ""
        style = DialStyle::COVERFLOW;
    }
    return style;
}

void SettingDesktopModel::SetDialStyle(DialStyle style)
{
    if (style == DialStyle::COVERFLOW) {
        (void)UtilsSetValue("set_dial_style", "DialStyle::COVERFLOW");
    } else if (style == DialStyle::ROLLER) {
        (void)UtilsSetValue("set_dial_style", "DialStyle::ROLLER");
    }
}

void SettingDesktopModel::SetDialStyleDataSelect(const char* DialStyleName)
{
    auto coverflowItemData = static_cast<IconTextToggleItemData*>(
        dialStyleUnionData_[SETTING_DIAL_STYLE_COVERFLOW_IDX].GetUnionItemData());
    auto rollerItemData = static_cast<IconTextToggleItemData*>(
        dialStyleUnionData_[SETTING_DIAL_STYLE_ROLLER_IDX].GetUnionItemData());
    auto aodItemData = static_cast<IconTextToggleItemData*>(
        dialStyleUnionData_[SETTING_DIAL_STYLE_AOD_IDX].GetUnionItemData());

    if (strcmp(DialStyleName, aodItemData->textContent) == 0) {
        aodItemData->selected = !aodItemData->selected;
        MainModel::GetInstance().EnableAod(aodItemData->selected);
    } else {
        DialStyle style;
        rollerItemData->selected = !rollerItemData->selected;
        coverflowItemData->selected = !coverflowItemData->selected;
        style = rollerItemData->selected ? DialStyle::ROLLER : DialStyle::COVERFLOW;
        SetDialStyle(style);
    }
}

void SettingDesktopModel::LoadAllDial(void)
{
    LoadAllDialFromRam(); // 导入在线表盘
    size_t dialSize = dialInfos_.size();
    size_t aodSize = aodDialInfos_.size();
    LoadAllDialFromFs(); // 导入离线表盘
    /* 只排序离线表盘 */
    std::sort(dialInfos_.begin() + dialSize, dialInfos_.end(), [](const SetDialInfo& f, const SetDialInfo& n) {
                return f.dialName < n.dialName;
            });
    std::sort(aodDialInfos_.begin() + aodSize,
        aodDialInfos_.end(),
        [](const SetDialInfo &f, const SetDialInfo &n) { return f.dialName < n.dialName; });
}

std::vector<SetDialInfo>& SettingDesktopModel::GetDialInfo(void)
{
    return dialInfos_;
}

std::vector<SetDialInfo>& SettingDesktopModel::GetAmbientDialInfo(void)
{
    return aodDialInfos_;
}

void SettingDesktopModel::SaveDialSetting(uint8_t pageIndex)
{
    if (pageIndex >= dialInfos_.size()) {
        return;
    }
    DialSetting diagSetting;
    SetDialInfo &info = dialInfos_[pageIndex];
    diagSetting.dialFlag = info.dialFlag;
    diagSetting.dialId = info.id;
    diagSetting.dialFullName = info.dialFullName;
    MainModel::GetInstance().AddDialSetting(diagSetting);
}

void SettingDesktopModel::SetSpecifyDialSetting(const DialSetting &setting)
{
    hasSpecificDial_ = true;
    specificDialSetting_ = setting;
}

void SettingDesktopModel::ClearDialInfo(void)
{
    for (auto iter : dialInfos_) {
        if (iter.dialFlag) {
            ImageCacheManager::GetInstance().UnloadOneInMultiRes(iter.resId, iter.dialFullName);
        } else {
            ImageCacheManager::GetInstance().UnloadSingleRes(iter.imageFullName);
        }
    }
    dialInfos_.clear();
    aodDialInfos_.clear();

    if (hasSpecificDial_) {
        MainModel::GetInstance().GetDialSetting() = specificDialSetting_;
        MainModel::GetInstance().AddDialSetting(specificDialSetting_);
    }
}

uint8_t SettingDesktopModel::GetPageIndex(void)
{
    uint8_t index = 0;
    DialSetting dialSetting = MainModel::GetInstance().GetDialSetting();
    for (auto& iter : dialInfos_) {
        if (dialSetting.dialFlag) {
            if (strcmp(iter.dialFullName.c_str(), dialSetting.dialFullName.c_str()) == 0) {
                return index;
            }
        } else {
            if ((!iter.dialFlag) && (iter.id == dialSetting.dialId)) {
                return index;
            }
        }
        index++;
    }
    return 0;
}

void SettingDesktopModel::LoadAllDialFromRam()
{
    for (uint8_t i = 0; i < MainModel::GetInstance().GetDialInfoNum(); i++) {
        const DialInfo *info = MainModel::GetInstance().GetDialInfo(i);
        if (info == nullptr) {
            continue;
        }
        SetDialInfo dialInfo{0};
        dialInfo.dialFlag = 0;
        dialInfo.id = i;
        dialInfo.dialName = info->dialName;
        dialInfo.imageFullName = info->dialImg;
        ImageInfo* imageInfo = ImageCacheManager::GetInstance().LoadSingleRes(info->dialImg, false);
        if (info == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadAllDialFromRam loadimage %s fail", info->dialImg);
            continue;
        }
        dialInfo.preImageInfo = imageInfo;
        if (info->capability == DIAL_DISPLAY_CAPABILITY_NORMAL_ONLY) {
            dialInfos_.push_back(dialInfo);
        } else if (info->capability == DIAL_DISPLAY_CAPABILITY_AMBITIENT_ONLY) {
            aodDialInfos_.push_back(dialInfo);
        } else {
            dialInfos_.push_back(dialInfo);
            aodDialInfos_.push_back(dialInfo);
        }
    }
}

void SettingDesktopModel::LoadAllDialFromFs()
{
    struct dirent *direntp;

    // 打开目录
    DIR *dirp = opendir(APP_DIAL_PATH);
    // 遍历文件
    if (dirp != nullptr) {
        while ((direntp = readdir(dirp)) != nullptr) {
            SetDialInfo dialInfo{0};
            if (ParseDialHeader(direntp->d_name, dialInfo) != 0) {
                continue;
            }
            if (dialInfo.capability == DIAL_DISPLAY_CAPABILITY_NORMAL_ONLY) {
                dialInfos_.push_back(dialInfo);
            } else if (dialInfo.capability == DIAL_DISPLAY_CAPABILITY_AMBITIENT_ONLY) {
                aodDialInfos_.push_back(dialInfo);
            } else {
                dialInfos_.push_back(dialInfo);
                aodDialInfos_.push_back(dialInfo);
            }
        }
    }
    // 关闭目录
    closedir(dirp);
}

int32_t SettingDesktopModel::ParseDialHeader(const std::string &dialFileName, SetDialInfo &dialInfo)
{
    std::string dialFullName = APP_DIAL_PATH;
    dialFullName += "/";
    dialFullName += dialFileName;
    FILE *fp = fopen(dialFullName.c_str(), "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ParseDialHeader cannot open file: %s",
            dialFullName.c_str());
        return -1;
    }
    DialPreviewInfo preInfo{0};
    bool ret = DialBinParserManager::GetInstance()->GetDialPreviewInfo(dialFullName, fp, &preInfo);
    if (!ret) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetDialPreviewInfo %s fail", dialFullName.c_str());
        fclose(fp);
        return -1;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ParseDialHeader dialFullName: %s", dialFullName.c_str());
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ParseDialHeader dialName: %s", preInfo.dialName);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ParseDialHeader description: %s", preInfo.description);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ParseDialHeader previewImgId: %u", preInfo.imgId);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ParseDialHeader imageOffset: %u", preInfo.offset);
    ImageInfo *info = ImageCacheManager::GetInstance().LoadOneInMultiRes(
        preInfo.imgId, dialFullName, fp, false, preInfo.offset);
    if (info == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ParseDialHeader loadimage fail");
        fclose(fp);
        return -1;
    }

    dialInfo.dialFlag = 1;
    dialInfo.id = 0;
    dialInfo.resId = preInfo.imgId;
    dialInfo.dialFullName = dialFullName;
    dialInfo.dialName = preInfo.dialName;
    dialInfo.description = preInfo.description;
    dialInfo.preImageInfo = info;
    dialInfo.capability = preInfo.capability;

    fclose(fp);
    return 0;
}
}
