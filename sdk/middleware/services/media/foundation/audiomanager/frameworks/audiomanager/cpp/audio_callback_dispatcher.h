/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: callback dispatcher interface
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef AUDIO_CALL_BACK_DISPATCHER_H
#define AUDIO_CALL_BACK_DISPATCHER_H

#include <cstddef>
#include <cstdint>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <unistd.h>
#include <list>
#include "media_thread_adapt.h"

namespace Audio {
struct Message {
    uintptr_t userData;
    int32_t type;
    int32_t hint;
};

class AudioCallbackDispatcher {
public:
    explicit AudioCallbackDispatcher();
    ~AudioCallbackDispatcher();

    bool Initialize();

    // Posts |msg| to the listener's queue. If |realTime| is true, the listener thread is notified
    // that a new message is available on the queue. Otherwise, the message stays on the queue, but
    // the listener is not notified of it. It will process this message when a subsequent message
    // is posted with |realTime| set to true.
    void Post(const Message &msg, bool realTime = true);

    bool Loop();

private:
    class AudioCallbackDispatcherThread;
    static constexpr uint32_t MAX_QUEUE_SIZE = 2;
    bool isInitialized_;
    MediaThreadCondHandle queueChanged_ = nullptr;
    MediaMutexHandle queueChangedMutex_ = nullptr;

    MediaMutexHandle mutex_ = nullptr;
    bool done_;
    std::list<Message> msgQueue_;

    AudioCallbackDispatcherThread *dispatcherThread_;

    void Dispatch(std::list<Message> &messages);

    AudioCallbackDispatcher(const AudioCallbackDispatcher&);
    AudioCallbackDispatcher &operator=(const AudioCallbackDispatcher&);
};
}  // namespace Audio
#endif  // AUDIO_CALL_BACK_DISPATCHER_H
