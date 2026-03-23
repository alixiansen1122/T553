/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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

#ifndef GRAPHIC_LITE_DFX_FRAME_TRACE_H
#define GRAPHIC_LITE_DFX_FRAME_TRACE_H
#include <cstdlib>
#include "tcxo.h"
#include "graphic_config.h"
#include "components/ui_view.h"
#include "components/root_view.h"
#include "common/graphic_utils.h"
#include "graphic_service_wrapper.h"
#include "dfx/dfx_traceui.h"

#if ENABLE_DFX_CMD
#define FRAME_TRACE_ENABLE 1
#else
#define FRAME_TRACE_ENABLE 0
#endif
#if FRAME_TRACE_ENABLE
#define FRAME_TRACE_START(type) DfxFrameTrace::GetInstance().FrameTraceStart(type)
#define FRAME_TRACE_END(type, prect) DfxFrameTrace::GetInstance().FrameTraceEnd((type), (prect))
#define FRAME_TRACE_ENABLE_COUNT(cnt) DfxFrameTrace::GetInstance().EnableFrameTrace(cnt)
#define FRAME_TRACE_BEGIN_TRACE() DfxFrameTrace::GetInstance().BeginTrace()
#define FRAME_TRACE_SET_RECT(rect) DfxFrameTrace::GetInstance().SetDrawRect(rect)
#define FRAME_TRACE_END_TRACE() DfxFrameTrace::GetInstance().EndTrace()
#define FRAME_TRACE_SHOW() DfxFrameTrace::GetInstance().ShowFrameTrace()
#define EVENT_TRACE_START() DfxFrameTrace::GetInstance().EventTraceStart()
#define EVENT_TRACE_END() DfxFrameTrace::GetInstance().EventTraceEnd()
#define TASK_TRACE_START() DfxFrameTrace::GetInstance().TaskTraceStart()
#define TASK_TRACE_END() DfxFrameTrace::GetInstance().TaskTraceEnd()
#define SET_TRACE_PATH_TYPE(type, attr) DfxFrameTrace::GetInstance().SetPathType((type), (attr))
#define SET_TRACE_FLUSH_TYPE(isSram) DfxFrameTrace::GetInstance().SetFlushMemType(isSram)
#else
#define FRAME_TRACE_START(type)
#define FRAME_TRACE_END(type, prect)
#define FRAME_TRACE_ENABLE_COUNT(cnt)
#define FRAME_TRACE_BEGIN_TRACE()
#define FRAME_TRACE_SET_RECT(rect)
#define FRAME_TRACE_END_TRACE()
#define FRAME_TRACE_SHOW()
#define EVENT_TRACE_START()
#define EVENT_TRACE_END()
#define TASK_TRACE_START()
#define TASK_TRACE_END()
#define SET_TRACE_PATH_TYPE(type, attr)
#define SET_TRACE_FLUSH_TYPE(isSram)
#endif

namespace OHOS {
enum class FrameTraceType {
    SW_DRAW_TRANSFORM = 0,
    SW_DRAW_IMGBLIT,
    HW_DRAW_IMGBLIT,
    HW_DRAW_PATH,
    HW_CLIP_IMGBYPATH,
    SW_DRAW_CUBICBEZIER,
    SW_DRAW_RECT,
    HW_DRAW_RECT,
    HW_DRAW_BLUR,
    SW_DRAW_LETTER,
    SW_DRAW_ARC,
    SW_DRAW_LINE,
    HW_DRAW_SUBMIT,
    HW_DRAW_FLUSH,
    HW_DRAW_WAIT,
    HW_DRAW_ARC_RECT,
    HW_DRAW_LINE,
    HW_DRAW_ARC,
    HW_DRAW_CUBICBEZIER,
    HW_DRAW_ELLIPSE,
    DRAW_MAX
};

enum class FrameTracePathAttr : uint8_t {
    NONE = 0,
    FILL = 1,
    STROKE = 1 << 1,
    BOTH = (1 << 1) | 1,
};

class DfxFrameTrace : public HeapBase {
public:
    virtual ~DfxFrameTrace() {}
    static DfxFrameTrace& GetInstance();
    DfxFrameTrace(const DfxFrameTrace&) = delete;
    DfxFrameTrace& operator=(DfxFrameTrace&) = delete;
    void EnableFrameTrace(int enable)
    {
        traceCount_ = enable;
    }

    void BeginTrace()
    {
        beginTrace_ = traceCount_ > 0 ? true : false;
        if (beginTrace_) {
            isNativeRunning_ = IsNativeRunning();
            freq_ = GetFrequence();
        }
    }

    void SetDrawRect(const Rect &rect)
    {
        TRACEUI_LOG("SetDrawRect{%d,%d,%d,%d}", rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
        drawRect_ = beginTrace_ ? rect : drawRect_;
    }

    void SetFlushMemType(bool isSram)
    {
        TRACEUI_LOG("FlushType:%s", isSram ? "SRAM" : "PSRAM");
        isSram_ = isSram;
    }

    void EndTrace()
    {
        if (beginTrace_) {
            traceCount_ -= (drawCount_ != 0);
            ClearState();
        }
    }

    void SetPathType(FrameTraceType type, FrameTracePathAttr attr);
    void FrameTraceStart(FrameTraceType type);
    void FrameTraceEnd(FrameTraceType type, const Rect *rect = nullptr);
    void ShowFrameTrace();

    void EventTraceStart();
    void EventTraceEnd();
    void TaskTraceStart();
    void TaskTraceEnd();

private:
    DfxFrameTrace()
    {
        ClearState();
    }

    bool CanTrace() const
    {
        return beginTrace_ && drawCount_ < DRAW_RECORD_MAX;
    }

    void ClearState();
    const char *DrawTypeToString(FrameTraceType type) const;
    const char *PathAttrToString(FrameTracePathAttr attr) const;
    uint64_t GetDuration(uint64_t start, uint64_t end) const;
    uint64_t ComputeTime() const;
    void RecordStart(FrameTraceType type);
    void RecordEnd(FrameTraceType type, const Rect *rect);
    static constexpr int32_t DRAW_RECORD_MAX = 128;
    struct FrameTrace {
        FrameTraceType cmd;
        uint64_t preTime;
        uint64_t costTime;
        bool isNeedWait;
    } trace_[DRAW_RECORD_MAX];

    struct FrameRecord {
        FrameRecord() : totalTime(0), isNativeRunning(0), freq(0), pathCnt(0),
            drawCount(0), isSram(true), trace(nullptr), rect(nullptr), attr(nullptr)
        {
            ++frmCnt_;
        }

        ~FrameRecord()
        {
            if (trace != nullptr) {
                delete []trace;
                trace = nullptr;
            }
            if (rect != nullptr) {
                delete []rect;
                rect = nullptr;
            }
            if (attr != nullptr) {
                delete []attr;
                attr = nullptr;
            }
            --frmCnt_;
        }
        uint64_t totalTime;
        int isNativeRunning;
        int freq;
        int pathCnt;
        int drawCount;
        Rect drawRect;
        bool isSram;
        FrameTrace *trace;
        Rect *rect;
        FrameTracePathAttr *attr;
    };

    struct TaskRecord {
        TaskRecord() : startTime(0), endTime(0), frameInfo(nullptr), next(nullptr)
        {
            ++tskCnt_;
        }
        ~TaskRecord()
        {
            if (frameInfo != nullptr) {
                delete frameInfo;
                frameInfo = nullptr;
            }
            --tskCnt_;
        }
        uint64_t startTime;
        uint64_t endTime;
        FrameRecord *frameInfo;
        TaskRecord *next;
    };

    struct EventRecord {
        EventRecord() : startTime(0), endTime(0), tasks(nullptr), tailTsk(nullptr), next(nullptr)
        {
            ++evtCnt_;
        }
        ~EventRecord()
        {
            --evtCnt_;
        }
        uint64_t startTime;
        uint64_t endTime;
        TaskRecord *tasks;
        TaskRecord *tailTsk;
        EventRecord *next;
    };

    void WriteFrameRecord(FrameRecord *frame) const;
    void ShowFrameRecord(FrameRecord *frame) const;
    void ShowEventRecord() const;
    void ClearEvents();

    static int evtCnt_;
    static int tskCnt_;
    static int frmCnt_;
    Rect rect_[DRAW_RECORD_MAX];
    FrameTracePathAttr pathAttrs_[DRAW_RECORD_MAX];
    int pathCnt_ = 0;
    int pathIndex_ = -1;
    Rect drawRect_;
    bool isSram_ = true;
    uint64_t firstDrawTime_ = 0;
    uint64_t startTime_ = 0;
    uint64_t endTime_ = 0;
    int32_t drawCount_ = 0;
    int32_t drawStack_ = 0;
    int traceCount_ = 0;
    bool beginTrace_ = false;
    int isNativeRunning_ = 0;
    int freq_ = 0;
    EventRecord *events_ = nullptr;
    EventRecord *tailEvt_ = nullptr;
    bool eventRecording_ = false;
    bool taskRecording_ = false;
    FrameTraceType pathType_ = FrameTraceType::HW_DRAW_PATH;
    FrameTracePathAttr pathAttr_ = FrameTracePathAttr::NONE;
};
} // OHOS
#endif // GRAPHIC_LITE_DFX_FRAME_TRACE_H
