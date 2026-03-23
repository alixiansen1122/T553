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

#include "securec.h"
#include "dfx/dfx_frame_trace.h"

namespace OHOS {
int DfxFrameTrace::evtCnt_ = 0;
int DfxFrameTrace::tskCnt_ = 0;
int DfxFrameTrace::frmCnt_ = 0;

const char *DfxFrameTrace::DrawTypeToString(FrameTraceType type) const
{
    switch (type) {
        case FrameTraceType::SW_DRAW_TRANSFORM: return "sw_tran";
        case FrameTraceType::SW_DRAW_IMGBLIT: return "sw_blit";
        case FrameTraceType::HW_DRAW_IMGBLIT: return "hw_blit";
        case FrameTraceType::HW_DRAW_PATH: return "hw_path";
        case FrameTraceType::HW_CLIP_IMGBYPATH: return "hw_clip";
        case FrameTraceType::SW_DRAW_CUBICBEZIER: return "sw_bezier";
        case FrameTraceType::SW_DRAW_RECT: return "sw_rect";
        case FrameTraceType::HW_DRAW_RECT: return "hw_rect";
        case FrameTraceType::HW_DRAW_BLUR: return "hw_blur";
        case FrameTraceType::SW_DRAW_LETTER: return "sw_letter";
        case FrameTraceType::SW_DRAW_ARC: return "sw_arc";
        case FrameTraceType::SW_DRAW_LINE: return "sw_line";
        case FrameTraceType::HW_DRAW_SUBMIT: return "hw_submit";
        case FrameTraceType::HW_DRAW_FLUSH: return "hw_flush";
        case FrameTraceType::HW_DRAW_WAIT: return "hw_wait";
        case FrameTraceType::HW_DRAW_ARC_RECT: return "hw_arc_rect";
        case FrameTraceType::HW_DRAW_LINE: return "hw_line";
        case FrameTraceType::HW_DRAW_ARC: return "hw_arc";
        case FrameTraceType::HW_DRAW_CUBICBEZIER: return "hw_bezier";
        case FrameTraceType::HW_DRAW_ELLIPSE: return "hw_ellipse";
        default: break;
    }
    return "hw_unknown";
}

const char *DfxFrameTrace::PathAttrToString(FrameTracePathAttr attr) const
{
    switch (attr) {
        case FrameTracePathAttr::NONE: return "";
        case FrameTracePathAttr::FILL: return "fill";
        case FrameTracePathAttr::STROKE: return "stroke";
        case FrameTracePathAttr::BOTH: return "both";
        default: break;
    }
    return "unknown";
}

uint64_t DfxFrameTrace::GetDuration(uint64_t start, uint64_t end) const
{
    uint64_t dur = end >= start ? end - start : UINT64_MAX - start + end + 1;
    return dur;
}

DfxFrameTrace& DfxFrameTrace::GetInstance()
{
    static DfxFrameTrace dfx;
    return dfx;
}

void DfxFrameTrace::SetPathType(FrameTraceType type, FrameTracePathAttr attr)
{
    pathType_ = type;
    pathAttr_ = attr;
    TRACEUI_LOG("PathType:%s %s", DrawTypeToString(type), PathAttrToString(attr));
}

void DfxFrameTrace::FrameTraceStart(FrameTraceType type)
{
    TRACEUI_BEGIN(DrawTypeToString(type));
    if (!CanTrace()) {
        return;
    }
    if (drawStack_ == 0) {
        if (type == FrameTraceType::HW_DRAW_PATH) {
            type = pathType_;
        }
        RecordStart(type);
    }
    ++drawStack_;
}

void DfxFrameTrace::FrameTraceEnd(FrameTraceType type, const Rect *rect)
{
    TRACEUI_END();
    if (!CanTrace()) {
        return;
    }
    --drawStack_;
    if (drawStack_ != 0) {
        return;
    }

    if (type == FrameTraceType::HW_DRAW_PATH) {
        type = pathType_;
        pathType_ = FrameTraceType::HW_DRAW_PATH;
        pathAttr_ = FrameTracePathAttr::NONE;
    }
    RecordEnd(type, rect);
}

void DfxFrameTrace::RecordStart(FrameTraceType type)
{
    startTime_ = uapi_tcxo_get_us();
    uint64_t preTime = 0;
    if (drawCount_ == 0) {
        if (tailEvt_ != nullptr && tailEvt_->tailTsk != nullptr) {
            preTime = GetDuration(tailEvt_->tailTsk->startTime, startTime_);
        } else {
            preTime = 0;
        }
        firstDrawTime_ = startTime_;
    } else {
        preTime = GetDuration(endTime_, startTime_);
    }

    if (type == FrameTraceType::HW_DRAW_PATH) {
        if (pathIndex_ == -1) {
            pathIndex_ = drawCount_;
            trace_[pathIndex_].cmd = type;
            trace_[pathIndex_].preTime = 0;
            trace_[pathIndex_].costTime = 0;
            ++drawCount_;
        }
        trace_[pathIndex_].preTime += preTime;
        pathAttrs_[pathIndex_] = FrameTracePathAttr::NONE;
        return;
    }
    trace_[drawCount_].cmd = type;
    trace_[drawCount_].preTime = preTime;
    pathAttrs_[drawCount_] = pathAttr_;
}

void DfxFrameTrace::RecordEnd(FrameTraceType type, const Rect *rect)
{
    endTime_ = uapi_tcxo_get_us();
    uint64_t costTime = GetDuration(startTime_, endTime_);

    if (type == FrameTraceType::HW_DRAW_PATH) {
        trace_[pathIndex_].costTime += costTime;
        ++pathCnt_;
        return;
    }
    trace_[drawCount_].costTime = costTime;
    rect_[drawCount_] = rect == nullptr ? Rect() : *rect;
    if (type == FrameTraceType::HW_DRAW_SUBMIT) {
        trace_[drawCount_].isNeedWait = (rect != nullptr);
    }
    ++drawCount_;
}

void DfxFrameTrace::ShowFrameTrace()
{
    if (!beginTrace_) {
        return;
    }

    if (events_ == nullptr || !eventRecording_ || !taskRecording_ || drawCount_ <= 0) {
        return;
    }
    EventRecord &curEvt = *tailEvt_;
    if (curEvt.tasks == nullptr) {
        return;
    }

    FrameRecord *record = new (std::nothrow) FrameRecord;
    if (record == nullptr) {
        return;
    }
    WriteFrameRecord(record);
    TaskRecord &curTask = *curEvt.tailTsk;
    curTask.frameInfo = record;
}

void DfxFrameTrace::ClearState()
{
    drawRect_ = Rect();
    drawCount_ = 0;
    drawStack_ = 0;
    beginTrace_ = false;
    startTime_ = 0;
    endTime_ = 0;
    firstDrawTime_ = 0;
    pathCnt_ = 0;
    pathIndex_ = -1;
    freq_ = 0;
    isNativeRunning_ = 0;
    pathType_ = FrameTraceType::HW_DRAW_PATH;
    pathAttr_ = FrameTracePathAttr::NONE;
    isSram_ = true;
}

void DfxFrameTrace::WriteFrameRecord(FrameRecord *frame) const
{
    if (frame == nullptr) {
        return;
    }
    uint64_t totalTime = GetDuration(firstDrawTime_, endTime_);
    frame->totalTime = totalTime;
    frame->isNativeRunning = isNativeRunning_;
    frame->freq = freq_;
    frame->pathCnt = pathCnt_;
    frame->drawRect = drawRect_;
    frame->isSram = isSram_;
    if (drawCount_ <= 0) {
        return;
    }
    frame->drawCount = drawCount_;
    frame->trace = new (std::nothrow) FrameTrace[drawCount_];
    frame->rect = new (std::nothrow) Rect[drawCount_];
    frame->attr = new (std::nothrow) FrameTracePathAttr[drawCount_];
    if (frame->trace == nullptr || frame->rect == nullptr || frame->attr == nullptr) {
        return;
    }
    for (int i = 0; i < drawCount_; ++i) {
        frame->trace[i] = trace_[i];
        frame->rect[i] = rect_[i];
        frame->attr[i] = pathAttrs_[i];
    }
}

void DfxFrameTrace::ShowFrameRecord(FrameRecord *frame) const
{
    if (frame == nullptr) {
        return;
    }
    int drawCnt = frame->drawCount;
    printf("  isNativeRunning %d, freq %d\n", frame->isNativeRunning, frame->freq);
    printf("  Cmd count: %d time: %llu, {%d %d %d %d}\n", drawCnt, frame->totalTime,
        frame->drawRect.GetLeft(), frame->drawRect.GetTop(), frame->drawRect.GetRight(), frame->drawRect.GetBottom());
    if (frame->trace == nullptr || frame->rect == nullptr) {
        return;
    }
    for (int32_t i = 0; i < drawCnt; ++i) {
        if (frame->trace[i].cmd == FrameTraceType::HW_DRAW_PATH) {
            printf("  %d. %s: %llu, %llu, %d",
                i, DrawTypeToString(frame->trace[i].cmd),
                frame->trace[i].preTime, frame->trace[i].costTime, frame->pathCnt);
        } else if (frame->trace[i].cmd == FrameTraceType::HW_DRAW_SUBMIT) {
            printf("  %d. %s: %llu, %llu, %d",
                i, DrawTypeToString(frame->trace[i].cmd),
                frame->trace[i].preTime, frame->trace[i].costTime, frame->trace[i].isNeedWait);
        } else if (frame->trace[i].cmd == FrameTraceType::HW_DRAW_IMGBLIT) {
            printf("  %d. %s: %llu, %llu, %d", i, DrawTypeToString(frame->trace[i].cmd),
                frame->trace[i].preTime, frame->trace[i].costTime, frame->rect[i].GetLeft());
        } else if (!frame->rect[i].IsValid()) {
            printf("  %d. %s: %llu, %llu", i, DrawTypeToString(frame->trace[i].cmd),
                frame->trace[i].preTime, frame->trace[i].costTime);
        } else {
            printf("  %d. %s: %llu, %llu, {%d, %d, %d, %d}",
                i, DrawTypeToString(frame->trace[i].cmd), frame->trace[i].preTime, frame->trace[i].costTime,
                frame->rect[i].GetLeft(),
                frame->rect[i].GetTop(), frame->rect[i].GetRight(), frame->rect[i].GetBottom());
        }

        if (frame->trace[i].cmd == FrameTraceType::HW_DRAW_FLUSH) {
            if (frame->isSram) {
                printf(", s");
            } else {
                printf(", p");
            }
        }

        if (frame->attr[i] == FrameTracePathAttr::NONE) {
            printf("\n");
        } else {
            printf(", %s\n", PathAttrToString(frame->attr[i]));
        }
    }
}

void DfxFrameTrace::ShowEventRecord() const
{
    int evtIndex = 0;
    int taskIndex = 0;
    printf("Total: E %d, T %d, F %d\n", evtCnt_, tskCnt_, frmCnt_);
    EventRecord *preE = events_;
    for (EventRecord *it = events_; it != nullptr; it = it->next) {
        EventRecord &evt = *it;
        uint64_t preEvtTime = evtIndex == 0 ? preE->startTime : preE->endTime;
        printf("\nE %d: %llu, %llu\n",
            evtIndex, GetDuration(preEvtTime, evt.startTime), GetDuration(evt.startTime, evt.endTime));
        taskIndex = 0;
        TaskRecord *preT = evt.tasks;
        for (TaskRecord *it1 = evt.tasks; it1 != nullptr; it1 = it1->next) {
            TaskRecord &tsk = *it1;
            uint64_t preTskTime = taskIndex == 0 ? evt.startTime : preT->endTime;
            printf(" T %d: %llu, %llu\n",
                taskIndex, GetDuration(preTskTime, tsk.startTime), GetDuration(tsk.startTime, tsk.endTime));
            if (tsk.frameInfo != nullptr) {
                ShowFrameRecord(tsk.frameInfo);
            }
            preT = it1;
            taskIndex++;
        }
        preE = it;
        evtIndex++;
    }
}

void DfxFrameTrace::ClearEvents()
{
    for (EventRecord *it = events_; it != nullptr;) {
        EventRecord &evt = *it;
        for (TaskRecord *it1 = evt.tasks; it1 != nullptr;) {
            TaskRecord *tskTmp = it1;
            it1 = it1->next;
            delete tskTmp;
        }
        EventRecord *tmp = it;
        it = it->next;
        delete tmp;
    }
    tailEvt_ = nullptr;
    events_ = nullptr;
}

void DfxFrameTrace::EventTraceStart()
{
    TRACEUI_BEGIN("GraphicEvent");
    if (traceCount_ <= 0) {
        return;
    }
    EventRecord *event = new (std::nothrow) EventRecord;
    if (event == nullptr) {
        return;
    }
    event->startTime = uapi_tcxo_get_us();
    if (events_ != nullptr) {
        tailEvt_->next = event;
    } else {
        events_ = event;
    }
    tailEvt_ = event;
    eventRecording_ = true;
}

void DfxFrameTrace::EventTraceEnd()
{
    TRACEUI_END();
    if (events_ == nullptr || !eventRecording_) {
        return;
    }
    EventRecord &curEvt = *tailEvt_;
    curEvt.endTime = uapi_tcxo_get_us();
    eventRecording_ = false;
    if (traceCount_ == 0) {
        ShowEventRecord();
        ClearEvents();
    }
}

void DfxFrameTrace::TaskTraceStart()
{
    TRACEUI_BEGIN("TaskExecute");
    if (events_ == nullptr || !eventRecording_) {
        return;
    }
    EventRecord &curEvt = *tailEvt_;
    TaskRecord *task = new (std::nothrow) TaskRecord;
    if (task == nullptr) {
        return;
    }
    task->startTime = uapi_tcxo_get_us();
    if (curEvt.tasks != nullptr) {
        curEvt.tailTsk->next = task;
    } else {
        curEvt.tasks = task;
    }
    curEvt.tailTsk = task;
    taskRecording_ = true;
}

void DfxFrameTrace::TaskTraceEnd()
{
    TRACEUI_END();
    if (events_ == nullptr || !eventRecording_ || !taskRecording_) {
        return;
    }
    EventRecord &curEvt = *tailEvt_;
    if (curEvt.tasks == nullptr) {
        return;
    }
    TaskRecord &task = *curEvt.tailTsk;
    task.endTime = uapi_tcxo_get_us();
    taskRecording_ = false;
}

} // OHOS
