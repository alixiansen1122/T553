/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: WatchInterface
 * Author:
 * Create: 2025-03-17
 */

#include <string>
#include <unistd.h>
#include "dirent.h"
#include "main/DialBinParser.h"
#include "UiConfig.h"
#include "main/dial/WatchInterface.h"
#include "gfx_utils/vector.h"
#include "common/screen.h"
#include "main/MainModel.h"
#include "main/MainViewSample.h"
#include "graphic_service.h"
#include "NativeAbility.h"
#include "graphic_mutex.h"
#include "settings/model/SettingDesktopModel.h"
#include "power_display_service.h"

using namespace OHOS;
namespace {
struct WatchInfoInner {
    std::string fileName;
    watch_dial_info watchInfo;
};

Graphic::Vector<WatchInfoInner> g_info;
GraphicMutex g_mutex;
GraphicCond g_cond;
bool g_eventFinished = true;
const uint16_t INVALID_INFO_INDEX = 0xFFFF;

bool GetActiveWatchPath(std::string &path)
{
    if (MainModel::GetInstance().GetDialSetting().dialFlag == 1) {
        path = MainModel::GetInstance().GetDialSetting().dialFullName;
        return true;
    }
    return false;
}

bool GetPreviewInfo(const std::string &fileName, DialPreviewInfo &preview, bool isNeedVerify = false)
{
    const std::string type(".bin");
    if (!(fileName.length() > type.length() &&
            fileName.substr(fileName.length() - type.length(), type.length()) == type)) {
        return false;
    }
    memset_s(&preview, sizeof(preview), 0, sizeof(preview));
    FILE *file = fopen(fileName.c_str(), "rb");
    bool isSuccess = false;
    if (file != nullptr) {
        isSuccess = DialBinParserManager::GetInstance()->GetDialPreviewInfo(fileName, file, &preview, isNeedVerify);
        fclose(file);
    }
    return isSuccess;
}

bool ConvertPreview2WatchDialInfo(const DialPreviewInfo &preview, watch_dial_info &info)
{
    errno_t ret = strcpy_s(reinterpret_cast<char *>(info.watch_protocol_version),
        sizeof(info.watch_protocol_version),
        preview.protocolVersion);
    if (ret != 0) {
        GRAPHIC_LOGE("failed to copy protocol version.");
        return false;
    }

    ret = strcpy_s(reinterpret_cast<char *>(info.watch_version), sizeof(info.watch_version), preview.watchVersion);
    if (ret != 0) {
        GRAPHIC_LOGE("failed to copy watch version.");
        return false;
    }
    info.uuid = preview.uuid;
    return true;
}

void RefreshAllWatchInfo()
{
    g_info.Clear();
    struct dirent *direntp = nullptr;

    // 打开目录
    DIR *dirp = opendir(APP_DIAL_PATH);
    // 遍历文件
    DialPreviewInfo info;
    watch_dial_info watchInfo;
    std::string activePath;
    bool isActiveExist = GetActiveWatchPath(activePath);
    if (dirp != nullptr) {
        while ((direntp = readdir(dirp)) != nullptr) {
            std::string fileName = std::string(APP_DIAL_PATH) + std::string("/") + std::string(direntp->d_name);
            if (!GetPreviewInfo(fileName, info)) {
                continue;
            }

            const std::string lowestVersion("3.0");
            std::string curVersion(info.protocolVersion);
            if (curVersion >= lowestVersion) {
                if (!ConvertPreview2WatchDialInfo(info, watchInfo)) {
                    break;
                }
                watchInfo.install_state = true;
                watchInfo.is_current_watch = isActiveExist ? (activePath == fileName) : false;
                g_info.PushBack({fileName, watchInfo});
            }
        }
    }
    // 关闭目录
    closedir(dirp);
}

inline bool IsFileExists(const std::string &file)
{
    return access(file.c_str(), F_OK) == 0;
}

uint16_t FindMaxVersionIndex(uint16_t indexArray[], uint16_t num)
{
    if (num == 0) {
        return INVALID_INFO_INDEX;
    }

    if (num == 0) {
        return indexArray[0];
    }
    std::string version = reinterpret_cast<char *>(g_info[indexArray[0]].watchInfo.watch_version);
    uint16_t index = 0;
    for (uint16_t i = 1; i < num; ++i) {
        std::string curVerion = std::string(reinterpret_cast<char *>(g_info[indexArray[i]].watchInfo.watch_version));
        if (curVerion > version) {
            version = curVerion;
            index = i;
        }
    }
    if (num > 1) {
        GRAPHIC_LOGE("There are watches with the same uuid. %u", g_info[indexArray[index]].watchInfo.uuid);
    }
    index = indexArray[index];
    return index;
}

void GetWatchInfo(uint32_t uuid, uint16_t &index)
{
    index = INVALID_INFO_INDEX;
    if (g_info.IsEmpty()) {
        RefreshAllWatchInfo();
    }
    const uint16_t MAX_INFO_NUM = 128;
    static uint16_t indexArray[MAX_INFO_NUM];
    uint16_t num = 0;
    std::string activePath;
    bool isActive = GetActiveWatchPath(activePath);
    bool isNeedRequery = false;
    for (uint16_t i = 0; i < g_info.Size(); ++i) {
        if (uuid == g_info[i].watchInfo.uuid && num < MAX_INFO_NUM) {
            if (IsFileExists(g_info[i].fileName)) {
                g_info[i].watchInfo.is_current_watch = isActive ? (g_info[i].fileName == activePath) : false;
                indexArray[num++] = i;
            } else {
                RefreshAllWatchInfo();
                isNeedRequery = true;
                break;
            }
        }
    }

    if (!isNeedRequery) {
        index = FindMaxVersionIndex(indexArray, num);
        return;
    }
    num = 0;
    for (uint16_t i = 0; i < g_info.Size(); ++i) {
        if (uuid == g_info[i].watchInfo.uuid && num < MAX_INFO_NUM) {
            indexArray[num++] = i;
        }
    }
    index = FindMaxVersionIndex(indexArray, num);
}

void TryToTurnOnScreen()
{
    const power_display_svr_api_t *ctx = power_display_svr_get_api();
    if (ctx == nullptr) {
        return;
    }
    if (ctx->get_screen_state != nullptr && ctx->get_screen_state() != SCREEN_ON) {
        if (ctx->turn_on_screen != nullptr) {
            ctx->turn_on_screen();
        }
    }
}
}  // anonymous namespace

bool GetAllWatchFaceInfo(watch_dial_info **info, uint8_t *num)
{
    if (info == nullptr || num == nullptr) {
        GRAPHIC_LOGE("para error!");
        return false;
    }
    GraphicLockGuard guard(g_mutex);
    while (!g_eventFinished) {
        g_cond.Wait(g_mutex);
    }
    RefreshAllWatchInfo();
    *num = g_info.Size();
    if (*num == 0) {
        *info = nullptr;
        return true;
    }
    watch_dial_info *mem = static_cast<watch_dial_info *>(malloc(*num * sizeof(watch_dial_info)));
    if (mem == nullptr) {
        GRAPHIC_LOGE("failed to malloc");
        return false;
    }
    for (uint16_t i = 0; i < g_info.Size(); ++i) {
        memcpy_s(&mem[i], sizeof(watch_dial_info), &g_info[i].watchInfo, sizeof(watch_dial_info));
    }
    *info = mem;
    return true;
}

bool GetWatchFaceInfo(uint32_t uuid, watch_dial_info *watch)
{
    if (watch == nullptr) {
        GRAPHIC_LOGE("para error!");
        return false;
    }

    uint16_t index = INVALID_INFO_INDEX;
    GraphicLockGuard guard(g_mutex);
    while (!g_eventFinished) {
        g_cond.Wait(g_mutex);
    }
    GetWatchInfo(uuid, index);
    if (index == INVALID_INFO_INDEX) {
        GRAPHIC_LOGE("uuid %u does not exist", uuid);
        return false;
    }
    memcpy_s(watch, sizeof(watch_dial_info), &g_info[index].watchInfo, sizeof(watch_dial_info));
    return true;
}

bool GetWatchCapability(watch_dev_cap_info *capability)
{
    if (capability == nullptr) {
        GRAPHIC_LOGE("para error!");
        return false;
    }
    capability->kaleidoscope = true;
    capability->effect_3D = false;
    capability->interaction = true;
    capability->video = true;
    capability->video_format = (1 << CODEC_JPEG);
    return true;
}

bool GetWatchSystemInfo(watch_sys_info *sysInfo)
{
    if (sysInfo == nullptr) {
        GRAPHIC_LOGE("para error!");
        return false;
    }
    sysInfo->watch_proto_support_cnt = DialBinParserManager::GetInstance()->GetSupportVersionCnt();
    for (uint8_t i = 0; i < sysInfo->watch_proto_support_cnt; ++i) {
        std::string version;
        bool isSuccess = DialBinParserManager::GetInstance()->GetVersionByIndex(i, version);
        if (!isSuccess) {
            return false;
        }
        errno_t ret =
            strcpy_s(sysInfo->watch_protocol_verion[i], sizeof(sysInfo->watch_protocol_verion[i]), version.c_str());
        if (ret != 0) {
            GRAPHIC_LOGE("get protocal version error!");
            return false;
        }
    }
    int ret = sprintf_s(sysInfo->resolution,
        sizeof(sysInfo->resolution),
        "%u*%u",
        Screen::GetInstance().GetWidth(),
        Screen::GetInstance().GetHeight());
    if (ret == -1) {
        GRAPHIC_LOGE("get resolution error!");
        return false;
    }

    return true;
}

bool GetCurrentWatchFaceInfo(watch_dial_info *watch)
{
    if (watch == nullptr) {
        GRAPHIC_LOGE("para error!");
        return false;
    }
    std::string activePath;
    if (!GetActiveWatchPath(activePath)) {
        return false;
    }

    DialPreviewInfo preview;
    if (!GetPreviewInfo(activePath, preview)) {
        return false;
    }

    if (std::string(preview.protocolVersion) < std::string("3.0")) {
        return false;
    }
    if (!ConvertPreview2WatchDialInfo(preview, *watch)) {
        return false;
    }
    watch->install_state = true;
    watch->is_current_watch = true;
    return true;
}

bool SetCurrentWatchFace(uint32_t uuid)
{
    uint16_t index = INVALID_INFO_INDEX;
    GraphicLockGuard guard(g_mutex);
    while (!g_eventFinished) {
        g_cond.Wait(g_mutex);
    }
    GetWatchInfo(uuid, index);
    if (index == INVALID_INFO_INDEX) {
        GRAPHIC_LOGE("uuid %u does not exist", uuid);
        return false;
    }
    DialSetting setting = MainModel::GetInstance().GetDialSetting();
    setting.dialFlag = 1;
    setting.dialFullName = g_info[index].fileName;
    if (NativeAbility::GetInstance().GetCurTargetId() ==
        (VIEW_SETTING | static_cast<uint32_t>(SettingPageId::DESKTOP_DIAL_PAGE) << PAGE_OFFSET)) {
        GraphicService::GetInstance()->PostGraphicEvent([setting]() {
            SettingDesktopModel::GetInstance().SetSpecifyDialSetting(setting);
        });
        NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
        return true;
    }
    if (g_info[index].watchInfo.is_current_watch) {
        return true;
    }
    g_eventFinished = false;
    GraphicService::GetInstance()->PostGraphicEvent([setting]() {
        GraphicLockGuard guardGraphic(g_mutex);
        MainModel::GetInstance().GetDialSetting() = setting;
        MainModel::GetInstance().AddDialSetting(setting);
        if (NativeAbility::GetInstance().GetCurSliceId() == VIEW_MAIN_SAMPLE) {
            MainViewSample::GetInstance()->ReloadWatchFace();
        }
        g_eventFinished = true;
        g_cond.Signal();
    });
    TryToTurnOnScreen();

    return true;
}

bool UninstallWatch(uint32_t uuid)
{
    uint16_t index = INVALID_INFO_INDEX;
    GraphicLockGuard guard(g_mutex);
    while (!g_eventFinished) {
        g_cond.Wait(g_mutex);
    }
    GetWatchInfo(uuid, index);
    if (index == INVALID_INFO_INDEX) {
        GRAPHIC_LOGE("uuid %u does not exist", uuid);
        return false;
    }

    DialSetting setting = MainModel::GetInstance().GetDialSetting();
    if (g_info[index].watchInfo.is_current_watch) {
        setting.dialFlag = 0;
        setting.dialId = 0;
    }
    if (NativeAbility::GetInstance().GetCurTargetId() ==
        (VIEW_SETTING | static_cast<uint32_t>(SettingPageId::DESKTOP_DIAL_PAGE) << PAGE_OFFSET)) {
        if (g_info[index].watchInfo.is_current_watch) {
            // If it is the current watch face, set it as the default watch face. Otherwise, the configuration remains
            // unchanged.
            GraphicService::GetInstance()->PostGraphicEvent([setting]() {
                SettingDesktopModel::GetInstance().SetSpecifyDialSetting(setting);
            });
        }
        NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
        g_eventFinished = false;
        GraphicService::GetInstance()->PostGraphicEvent([index]() {
            GraphicLockGuard guardGraphic(g_mutex);
            ImageCacheManager::GetInstance().UnloadAllInMultiRes(g_info[index].fileName);
            if (remove(g_info[index].fileName.c_str()) != 0) {
                GRAPHIC_LOGE("failed to remove file %s", g_info[index].fileName.c_str());
                g_eventFinished = true;
                g_cond.Signal();
                return;
            }
            g_info.Erase(index);
            g_eventFinished = true;
            g_cond.Signal();
        });
        return true;
    }

    g_eventFinished = false;
    GraphicService::GetInstance()->PostGraphicEvent([index, setting]() {
        GraphicLockGuard guardGraphic(g_mutex);
        if (g_info[index].watchInfo.is_current_watch &&
            NativeAbility::GetInstance().GetCurSliceId() == VIEW_MAIN_SAMPLE) {
            MainModel::GetInstance().GetDialSetting() = setting;
            MainModel::GetInstance().AddDialSetting(setting);
            MainViewSample::GetInstance()->ReloadWatchFace();
        }
        ImageCacheManager::GetInstance().UnloadAllInMultiRes(g_info[index].fileName);
        if (remove(g_info[index].fileName.c_str()) != 0) {
            GRAPHIC_LOGE("failed to remove file %s", g_info[index].fileName.c_str());
            g_eventFinished = true;
            g_cond.Signal();
            return;
        }
        g_info.Erase(index);
        g_eventFinished = true;
        g_cond.Signal();
    });
    TryToTurnOnScreen();
    return true;
}

bool GetUuidByPath(const char *path, uint32_t *uuid)
{
    if (path == nullptr || uuid == nullptr) {
        GRAPHIC_LOGE("para error!");
        return false;
    }

    DialPreviewInfo preview;
    if (!GetPreviewInfo(std::string(path), preview, true)) {
        return false;
    }
    if (std::string(preview.protocolVersion) < std::string("3.0")) {
        return false;
    }

    watch_dial_info watchInfo;
    if (ConvertPreview2WatchDialInfo(preview, watchInfo)) {
        std::string activePath;
        std::string appendPath(path);
        bool isActiveExist = GetActiveWatchPath(activePath);
        watchInfo.install_state = true;
        watchInfo.is_current_watch = isActiveExist ? (activePath == appendPath) : false;
        bool isExist = false;
        for (uint16_t i = 0; i < g_info.Size(); ++i) {
            if (g_info[i].fileName == appendPath) {
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            g_info.PushBack({appendPath, watchInfo});
        }
    }
    *uuid = preview.uuid;
    return true;
}