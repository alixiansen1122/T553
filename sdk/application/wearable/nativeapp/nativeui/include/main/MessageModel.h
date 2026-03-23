/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageModel
 * Create: 2025-06-29
 */

#ifndef MESSAGE_MODEL_H
#define MESSAGE_MODEL_H

#include <string>
#include <vector>
#include <sys/time.h>
#include "UiConfig.h"

namespace OHOS {
typedef struct {
    std::string appName;
    std::string appContact;
    std::string appContents;
    std::string time;
    uint16_t index;
    long initialTime;
    bool status;
} MessageItem;

    class MessageModel {
    public:
        static MessageModel &GetInstance(void);
        void SaveMsgToModel(std::string app, std::string contact, std::string information);
        void SetMsgListItems();
        std::vector<MessageItem> &GetMsgListItems();
        void ClearListItem();
        void SetViewStatus(bool viewStatus);
        bool GetViewStatus(void);
        std::string UpdateTime(long time);
        void SetTime(void);
        void DeleteItem(MessageItem itemInfo);
        void SetIndex(uint16_t itemIndex);
    private:
        MessageModel();
        MessageModel(const MessageModel &);
        MessageModel &operator=(const MessageModel &);
        virtual ~MessageModel();
        std::vector<MessageItem> vecMsg_;
        MessageItem mes_;
        struct timeval currentTime_;
        bool viewInitStatus_ = false;
        bool indexAdd_ = true;
        std::string durTime_;
        uint16_t index_ = 0;
    };
}

#endif