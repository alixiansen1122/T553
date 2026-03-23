/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
* Description: MessageModel
* Create: 2025-06-29
*/

#include "unistd.h"
#include <sys/time.h>
#include "kv_store.h"
#include <hal_tick.h>
#include <cstdint>
#include <random>
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "securec.h"
#include "wearable_log.h"
#include "main/DialBinTypesV2.h"
#include "power_display_service.h"
#include "main/MessageMainPage.h"
#include "main/MessageModel.h"

namespace OHOS {

static constexpr int16_t MESSAGE_MAX = 20;
static constexpr int32_t SECONDS = 60;
static constexpr int32_t HOURS = 3600;
static constexpr int32_t ONE_DAY = 86400;
static constexpr int16_t BUFFER_MAX = 6;

MessageModel::MessageModel() {}

MessageModel::~MessageModel() {}

MessageModel &MessageModel::GetInstance()
{
    static MessageModel instance;
    return instance;
}

// 将数据转换,保存到model模块
void MessageModel::SaveMsgToModel(std::string app, std::string contact, std::string information)
{
    mes_.appName = app; // 应用
    mes_.appContact = contact; // 联系人
    mes_.appContents = information; // 内容
    mes_.time = "现在";
    for (int i = 0; i < vecMsg_.size(); ++i) {
        if (vecMsg_[i].appName == mes_.appName && vecMsg_[i].appContact == mes_.appContact) {
            mes_.index = index_;
            indexAdd_ = false;
            break;
        }
        indexAdd_ = true;
        continue;
    }
    if (indexAdd_) {
        mes_.index = index_++;
    }
    gettimeofday(&currentTime_, nullptr);
    mes_.initialTime = currentTime_.tv_sec;
}

// 将转换好的数据存到vecMsg_
void MessageModel::SetMsgListItems()
{
    for (size_t i = 0; i < vecMsg_.size(); ++i) {
        if (vecMsg_[i].appName == mes_.appName && vecMsg_[i].appContact == mes_.appContact) {
            vecMsg_[i].appContents = mes_.appContents;
            vecMsg_[i].time = "现在";
            return;
        }
    }
    if (vecMsg_.size() < MESSAGE_MAX) {
        vecMsg_.push_back(mes_);
    } else {
        vecMsg_.erase(vecMsg_.begin());
        vecMsg_.push_back(mes_);
        int j = 0;
        for (j; j < vecMsg_.size(); ++j) {
            vecMsg_[j].index = j;
        }
        SetIndex(j);
    }
    // 收到新的消息后，需要同步将消息进行展示
    if (MessageModel::GetInstance().GetViewStatus()) {
        if (! MessageMainPage::GetInstance()->msgList_->IsVisible()) {
            MessageMainPage::GetInstance()->NewNotify();
        }
        MessageMainPage::GetInstance()->InitScrollData();
    }
}

std::vector<MessageItem> &MessageModel::GetMsgListItems()
{
    return vecMsg_;
}

void MessageModel::ClearListItem()
{
    vecMsg_.clear();
}

void MessageModel::SetViewStatus(bool viewStatus)
{
    viewInitStatus_ = viewStatus;
}

bool MessageModel::GetViewStatus(void)
{
    return viewInitStatus_;
}

void MessageModel::SetTime(void)
{
    if (vecMsg_.size() != 0) {
        for (size_t i = 0; i < vecMsg_.size(); ++i) {
            durTime_ = UpdateTime(vecMsg_[i].initialTime);
            vecMsg_[i].time = durTime_;
        }
    }
    return;
}

std::string MessageModel::UpdateTime(long time)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    auto now = tv.tv_sec;
    auto seconds = now - time;

    if (seconds < 0) {
        return "未来时间";
    }

    if (seconds < SECONDS) {
        return "现在";
    } else if (seconds < HOURS) {
        long minutes = seconds / SECONDS;
        return std::to_string(minutes) + "分钟前";
    } else if (seconds < ONE_DAY) {
        long hours = seconds / HOURS;
        return std::to_string(hours) + "小时前";
    } else {
        struct tm timeinfo;
        time_t t = static_cast<time_t>(time);
        localtime_r(&t, &timeinfo);
        char buffer[BUFFER_MAX];
        snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%02d/%02d", timeinfo.tm_mon + 1, timeinfo.tm_mday);
        return std::string(buffer);
    }
}

void MessageModel::DeleteItem(MessageItem itemInfo)
{
    for (int i = 0; i < vecMsg_.size(); ++i) {
        if (vecMsg_[i].appName == itemInfo.appName && vecMsg_[i].appContact == itemInfo.appContact) {
            vecMsg_.erase(vecMsg_.begin() + i);
            break;
        }
        continue;
    }
    int j = 0;
    for (j; j < vecMsg_.size(); ++j) {
        vecMsg_[j].index = j;
    }
    MessageMainPage::GetInstance()->RefreshMsgList(vecMsg_);
    SetIndex(j);
}

void MessageModel::SetIndex(uint16_t itemIndex)
{
    index_ = itemIndex;
}
}
