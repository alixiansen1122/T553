/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: callback dispatcher
* Author: Media Software Group
* Create: 2021-02-28
*/

#include "audio_callback_dispatcher.h"
#include <climits>
#include <list>
#include <sys/time.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <unistd.h>
#include "audio_errors.h"
#include "audio_utils.h"
#include "securec.h"
#include "audio_manager.h"
#include "media_thread_adapt.h"

#define LOG_MODULE_NAME "AudioCallbackDispatcher"

namespace Audio {
// This provides the underlying Thread used by CallbackDispatcher.
// Note that deriving CallbackDispatcher from Thread does not work.
// Will be debugged and modified according to different chip platforms
// change to 5 on FPGA
constexpr uint32_t DISPATCHER_THREAD_PRIORITY = 5;

class AudioCallbackDispatcher::AudioCallbackDispatcherThread {
public:
    explicit AudioCallbackDispatcherThread(AudioCallbackDispatcher &dispatcher);
    ~AudioCallbackDispatcherThread();
    bool Run();
    void Join();

private:
    AudioCallbackDispatcher *dispatcher_;

    static void *ThreadLoop(void *arg)
    {
#ifdef SUPPORT_SYS_PRCTL
        prctl(PR_SET_NAME, "DispatcherThread", 0, 0, 0);
#endif
        AudioCallbackDispatcher *dispatcher = (AudioCallbackDispatcher *)arg;
        if (dispatcher == nullptr) {
            return nullptr;
        }
        dispatcher->Loop();
        return nullptr;
    }
    MediaThreadIdHandle thread_;
    AudioCallbackDispatcherThread(const AudioCallbackDispatcherThread &);
    AudioCallbackDispatcherThread &operator=(const AudioCallbackDispatcherThread &);
};

AudioCallbackDispatcher::AudioCallbackDispatcher()
    : isInitialized_(false),
      done_(false),
      dispatcherThread_(nullptr)
{
    queueChangedMutex_ = MediaMutexCreate(nullptr);
    mutex_ = MediaMutexCreate(nullptr);
    queueChanged_ = MediaThreadCondCreate();
}

AudioCallbackDispatcher::~AudioCallbackDispatcher()
{
    (void)MediaMutexLock(queueChangedMutex_);
    done_ = true;
    MediaThreadCondSignal(queueChanged_);
    (void)MediaMutexUnLock(queueChangedMutex_);
    if (isInitialized_) {
        dispatcherThread_->Join();
    }
    if (dispatcherThread_ != nullptr) {
        delete dispatcherThread_;
    }

    MediaMutexDestroy(&queueChangedMutex_);
    MediaMutexDestroy(&mutex_);
    MediaThreadCondDestroy(&queueChanged_);
}

bool AudioCallbackDispatcher::Initialize()
{
    if (isInitialized_) {
        return true;
    }
    if (dispatcherThread_ == nullptr) {
        dispatcherThread_ = new AudioCallbackDispatcherThread(*this);
    }
    CHK_NULL_RETURN(dispatcherThread_, false, "dispatcherThread_ is nullptr");
    if (dispatcherThread_->Run()) {
        isInitialized_ = true;
        return true;
    }
    return false;
}

void AudioCallbackDispatcher::Post(const Message &msg, bool realTime)
{
    ALOGI("type:%d hint:%d", msg.type, msg.hint);
    (void)MediaMutexLock(queueChangedMutex_);
    msgQueue_.push_back(msg);
    if (realTime || msgQueue_.size() >= MAX_QUEUE_SIZE) {
        MediaThreadCondSignal(queueChanged_);
    }
    (void)MediaMutexUnLock(queueChangedMutex_);

    ALOGI("out unlock");
}

void AudioCallbackDispatcher::Dispatch(std::list<Message> &messages)
{
    ALOGI("func in");
    for (std::list<Message>::iterator it = messages.begin(); it != messages.end(); ++it) {
        InterruptListener *interruptListener = reinterpret_cast<InterruptListener *>(it->userData);
        if (interruptListener != nullptr) {
            interruptListener->OnInterrupt(it->type, it->hint);
        }
    }
    messages.clear();
    ALOGI("func out");
}

bool AudioCallbackDispatcher::Loop()
{
    ALOGI("Loop done_:%d", done_);
    for (;;) {
        std::list<Message> messages;
        MediaMutexLock(queueChangedMutex_);
        while (!done_ && msgQueue_.empty()) {
            MediaThreadCondWait(queueChanged_, queueChangedMutex_);
        }
        if (done_) {
            MediaMutexUnLock(queueChangedMutex_);
            ALOGI("done Loop");
            break;
        }
        messages.swap(msgQueue_);
        (void)MediaMutexUnLock(queueChangedMutex_);
        Dispatch(messages);
    }
    return false;
}

AudioCallbackDispatcher::AudioCallbackDispatcherThread::AudioCallbackDispatcherThread(
    AudioCallbackDispatcher &dispatcher)
    : dispatcher_(&dispatcher),
      thread_(nullptr)
{
}

AudioCallbackDispatcher::AudioCallbackDispatcherThread::~AudioCallbackDispatcherThread()
{
}

bool AudioCallbackDispatcher::AudioCallbackDispatcherThread::Run()
{
    (void)(DISPATCHER_THREAD_PRIORITY); // Dependent chip
    // stack size 0xC00: obtained from construct function deepest path scene, and upward adjust 512 bytes
    MediaThreadattr attr = { "DispatcherThread", 0x1000u, THREAD_SCHED_RR, 0, false };
    thread_ = MediaThreadCreate(ThreadLoop, dispatcher_, &attr);
    if (thread_ == nullptr) {
        ALOGE("thread create failed");
        return false;
    }
    return true;
}

void AudioCallbackDispatcher::AudioCallbackDispatcherThread::Join()
{
    MediaThreadJoin(&thread_);
    thread_ = nullptr;
}
}  // namespace Audio
