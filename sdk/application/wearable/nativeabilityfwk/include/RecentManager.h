/*
 * Copyright (c) CompanyNameMagicTag.
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

#ifndef RECENT_MANAGER_H
#define RECENT_MANAGER_H

#include <cstdio>
#include <string>
#include "gfx_utils/image_info.h"
#include "gfx_utils/list.h"
#include "graphic_mutex.h"

namespace OHOS {
#define RECENT_APP_BUNDLE_NAME_MAX_LEN 127

typedef struct {
    bool isNativeSlice; // true: native; false: js
    union {
        uint32_t targetId; // sliceId + pageId
        char bundleName[RECENT_APP_BUNDLE_NAME_MAX_LEN];
    };
    ImageInfo* snapshot;
} RecentAppInfo;

class RecentManager {
public:

    /**
     * @brief   Get <b>RecentManager*</b> instance.
     */
    static RecentManager* GetInstance();

    /**
     * @brief   Set max recent app size. Once current size > max, the oldest will be removed.
     *          Default value is 0, and it will not record any recent app.
     */
    void SetMaxSize(uint32_t max);

    /**
     * @brief   Get max recent app size.
     */
    uint32_t GetMaxSize();

    /**
     * @brief   Get current recent app size.
     */
    uint32_t GetCurrentSize();

    /**
     * @brief   Add a native app to recent manager with its snapshot to be cached.
     */
    void AddRecentApp(uint32_t targetId, const ImageInfo* snapshot);

    /**
     * @brief   Add a js app to recent manager with its snapshot to be cached.
     */
    void AddRecentApp(const char* bundleName, const ImageInfo* snapshot);

    /**
     * @brief   Remove a native app with given targetId.
     */
    bool RemoveRecentApp(uint32_t targetId, bool deleteFile = true);

    /**
     * @brief   Remove a js app with given bundleName.
     */
    bool RemoveRecentApp(const char* bundleName, bool deleteFile = true);

    /**
     * @brief   Remove all recent apps.
     */
    void RemoveAllRecentApps();

    /**
     * @brief   Get a list of RecentAppInfo.
     */
    const List<RecentAppInfo*>& GetRecentApps();

    /**
     * @brief   Set scale ratio. When an app is added, its snapshot could be scaled and cached. Default value is 0.5f.
     */
    void SetScaleRatio(float ratio);

    /**
     * @brief   Get scale ratio.
     */
    float GetScaleRatio();

    /**
     * @brief   Add a sliceId to black list so that it will not be saved as a recent app.
     */
    void AddToBlackList(uint32_t sliceId);

    /**
     * @brief   Remove a sliceId from black list so that it can be saved as a recent app.
     */
    void RemoveFromBlackList(uint32_t sliceId);

    /**
     * @brief   Clear black list.
     */
    void ClearBlackList();

    /**
     * @brief   Return whether the sliceId is on black list.
     */
    bool IsOnBlackList(uint32_t sliceId);

    /**
     * @brief   Return whether it is updated.
     */
    bool IsUpdateCompleted();

    /**
     * @brief   Set dir path to save recent snapshots.
     */
    void SetDirPath(std::string path);

    /**
     * @brief   Return dir path to save recent snapshots.
     */
    std::string GetDirPath();

    /**
     * @brief   Return whether it is enabled. It is enabled when max size is a valid value and dir path is set.
     */
    bool IsEnabled();

    /**
     * @brief   Used internally.
     */
    void PushRecentApp(RecentAppInfo* info);

    /**
     * @brief   Used internally.
     */
    void RemoveOldest();

    /**
     * @brief   Used internally.
     */
    void DecLoadingCnt();

    /**
     * @brief   Used internally.
     */
    uint32_t GetLoadingCnt();

    /**
     * @brief   Remove a native app with same sliceId. If pageId is same, file will be deleted as well.
     */
    bool RemoveNativeRecentApp(uint32_t targetId);

private:
    RecentManager();
    virtual ~RecentManager();

    void AddRecentAppWithSourceInfo(RecentAppInfo* appInfo, const ImageInfo* snapshot);
    void AddRecentAppInner(RecentAppInfo* appInfo, const ImageInfo* snapshot);
    void RemoveRecentAppNode(ListNode<RecentAppInfo*>* node, bool deleteFile);
    bool ScaleSnapshot(const ImageInfo* src, ImageInfo* dst);
    ListNode<RecentAppInfo*>* FindRecentAppInfo(List<RecentAppInfo*>& list,
        bool isNativeSlice, uint32_t sliceId, const char* bundleName);
    void AddPersistingTask(RecentAppInfo* info);
    static void AsyncPersisterRunnable(void* args);
    static void FreeRecentAppInfo(void* data);
    void AsyncFreeRecentAppInfo(RecentAppInfo* info);
    void ProcessPersistingTasks();
    void ProcessTask(RecentAppInfo* info);
    static void LoadRecentApp(void* data);
    List<RecentAppInfo*> recentApps_;
    List<RecentAppInfo*> persistingTasks_;
    List<uint32_t> blackList_;
    static GraphicMutex recentAppsMutex_;
    static GraphicMutex persistingTaskMutex_;
    static GraphicCond newPersistingTaskCond_;
    static GraphicCond persistingTaskDoneCond_;
    uint32_t maxSize_ = 0;
    float scaleRatio_ = 0.5f;
    std::string dirPath_;
    uint32_t loadingCnt_ = 0;
};
} // namespace OHOS

#endif // RECENT_MANAGER_H
