/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistModel.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef APPLIST_MODEL_H
#define APPLIST_MODEL_H

#include "bundle_info.h"
#include "AppViewIDs.h"
#include "ohos_types.h"

#define APP_MAX_NUM 50

namespace OHOS {
static const int INVALID_MASK = 0x0;

enum UIResourceTextId : uint16_t {
    STR_MAX_ID = 0,
};

struct AppItem {
    uint16_t id = VIEW_INVALID;
    const char *iconSmall = nullptr; // 小图标资源路径
    const char *iconHexagon = nullptr; // 六边形图标资源路径
    const char *label = nullptr; // 应用名
    uint32_t resId = INVALID_MASK; // 小图标压缩资源对应的id
    uint32_t hexagonId = INVALID_MASK; // 六边形图标压缩资源对应的id
    const char *resPath = nullptr; // 小图标压缩资源路径
    const char *hexagonPath = nullptr; // 六边形图标压缩资源路径
    uint16_t labelId = STR_MAX_ID; // 多语言环境下对应的字符id
    const char *uid = nullptr;
    // resId、resPath和iconSmall互斥, hexagonId、hexagonPath和iconHexagon互斥, labelId和label互斥

    AppItem() = default;

    // 使用该构造方法初始化，支持多语言注册以及多图标打包功能
    AppItem(uint16_t id, uint32_t resId, uint32_t hexagonId, const char *resPath,
           const char *hexagonPath, uint16_t labelId, const char *uid) :
        id(id),
        resId(resId),
        hexagonId(hexagonId),
        resPath(resPath),
        hexagonPath(hexagonPath),
        labelId(labelId),
        uid(uid) {}

    // 使用该构造方法初始化，不支持多语言，一个图标使用一个文件
    AppItem(uint16_t id, const char *iconSmall, const char *iconHexagon,
            const char *label, const char *uid) :
        id(id),
        iconSmall(iconSmall),
        iconHexagon(iconHexagon),
        label(label),
        uid(uid) {}
};

class ApplistPresenter;
class ApplistModel {
public:
    static ApplistModel &GetInstance(void);

    void Init();
    const AppItem *GetApplistItems() const;
    uint8 GetAppListNum() const;
    void ResolvingJSAppItems(void);
    void SetStartIndex(uint16_t index);
    uint16_t GetStartIndex(void);
    void SetLastOffset(int16_t x, int16_t y);
    void GetLastOffset(int16_t& x, int16_t& y);
    void SetHeadItemY(int index);
    int GetHeadItemY(void);
    void ApplistUpdate(void);
    void SetPresenter(ApplistPresenter *p);
    void SetInitFlag(bool flag);
    void CleanupApplistStrings(uint8_t nativeAppNumTotal);

private:
    ApplistModel();
    ApplistModel(const ApplistModel &);
    ApplistModel &operator=(const ApplistModel &);
    virtual ~ApplistModel();
    ApplistPresenter *presenter{nullptr};

    AppItem applist[APP_MAX_NUM];
    BundleInfo *bundleInfos = nullptr;
    int32 bundleLen = 0;
    uint8 appLen = 0;
    uint8 nativeAppNumTotal = 0;
    uint16_t startIndex = 0;
    int headItemY = 0;
    int16_t lastOffsetX = 0;
    int16_t lastOffsetY = 0;
    bool initFlag{false};
};
}

#endif
