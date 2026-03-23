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

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H
#include <functional>
#include <vector>

#include "common/task.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "alarm_notification.h"
#include "phone_msg_notification.h"
#include "calendar_notification.h"

constexpr uint16_t HALF_SCREEN = 2;
constexpr int16_t NOTIFY_PRIORITY_DEFAULT = 4;

namespace OHOS {
using NewNotify = std::function<UIViewGroup* ()>;
using RefreshNotifyContent = std::function<void()>;
typedef void (*FreeNotifyfunc)();
class NotificationManager : public Task {
public:
    static NotificationManager* GetInstance()
    {
        static NotificationManager instance;
        return &instance;
    }

    /**
    * @brief add a new notify.
    * @param notify is a function of inform layout.
    * @param id is current notify id.
    * @param fullScreen is or not full screen inform notify.
    */
    void ShowNotify(const NewNotify& notify, uint16_t id, bool fullScreen);

    /**
    * @brief refresh showing notify content.
    * @param refreshNotifyContent is a function of refresh notify content.
    */
    void RefreshNotify(const RefreshNotifyContent& refreshNotifyContent);

    /**
    * @brief stop and destroy showing notify.
    */
    void StopNotify(void);

    /**
    * @brief notify is or not showing.
    */
    bool HasNotifyShowing();
    void StartShowNotify(void);
    uint16_t GetNotifyPriority(void);
    uint16_t GetShowingNotifyId(void);
    void SetNotifyPriority(uint16_t prio);
    void SetNativeUIRunning(bool flag);
    bool GetVolumeScreenStatus();
    void SetVolumeScreenStatus(bool status);
    bool GetWatchCallStatus();
    void SetWatchCallStatus(bool status);
    void RegisterNotifyCleanupFunction(FreeNotifyfunc);
    void Callback() override;
    UIViewGroup* screenContainer_ = nullptr;

private:
    NotificationManager() {}
    ~NotificationManager() override {}

    void StartRefreshNotifyContent(void);
    void StopShowNotify(void);
    void FreeNotifyViews(void);

    NewNotify notify_;
    RefreshNotifyContent refreshNotifyContent_;

    bool isShowing_ = false;
    bool isFullScreen_ = false;
    bool isNativeUI = false;
    bool volumeScreenStatus = false;
    bool watchCallStatus = false;

    bool stopIsLast_ = false;
    bool showIsLast_ = false;
    bool hasNewNotifyNeedShow_ = false;
    bool hasNotifyNeedRefresh_ = false;
    bool hasNewNotifyNeedStop_ = false;
    std::vector<FreeNotifyfunc> notifyCleanupFunctions_;
    uint16_t notifyPri = NOTIFY_PRIORITY_DEFAULT;
    uint16_t notifyId = -1;

    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
    NotificationManager(NotificationManager&&) = delete;
    NotificationManager& operator=(NotificationManager&&) = delete;
};
}; // namespace OHOS
#endif // NOTIFICATION_MANAGER_H
