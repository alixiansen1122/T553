#include "main/dial/DialViewGroup.h"
#include <cstdio>
#include <iostream>
#include "NativeAbility.h"
#include "msg_center.h"
#include "securec.h"
#include "main/DialBinParser.h"
#include "main/dial/ModelDialDataFactory.h"
#include "common/low_power_manager.h"
#include "common/image_cache_manager.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
#include "main/MainPresenterSample.h"
#include "aod_view/AodPresenter.h"
#include "main/dial/DialDigitalImgView.h"
#include "common/input_device_manager.h"
#include "main/dial/DialSequenceImgView.h"
#include "engines/gfx/lite_m_gfx_engine.h"

namespace OHOS {
DialViewGroup::DialViewGroup(void)
{
    SetTouchable(true);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetCoverable(true);
    isExists_ = std::make_shared<bool>(true);
}

DialViewGroup::~DialViewGroup(void)
{
    if (kaleidoscope_ != nullptr) {
        kaleidoscope_->ClearFocus();
        kaleidoscope_ = nullptr;
    }
    ClearDial();
}

void DialViewGroup::ClearDial()
{
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(filePath_);
    ListNode<DialDataType>* passiveNode = passiveUpdateTypes_.Head();
    while (passiveNode != passiveUpdateTypes_.End()) {
        DialDataType type = passiveNode->data_;
        ModelDialDataFactory::GetInstance().UnRegisterDialDataListener(type);
        passiveNode = passiveNode->next_;
    }
    passiveUpdateTypes_.Clear();

    ListNode<DialClick>* clickNode = dialClicks_.Head();
    while (clickNode != dialClicks_.End()) {
        DialClick click = clickNode->data_;
        if (!click.isSliceType) {
            UIFree(click.bundleName);
        }
        clickNode = clickNode->next_;
    }
    dialClicks_.Clear();

    periodicUpdateTypes_.Clear();

    ClearViews();

    ListNode<UIView*>* viewNode = dialViews_.Head();
    while (viewNode != dialViews_.End()) {
        UIViewGroup::Remove(viewNode->data_);
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    dialViews_.Clear();

    rollingLabelList_.Clear();
    imgAnimList_.Clear();

    period_ = DEFAULT_TASK_PERIOD;

    video_ = nullptr;

    if (fp_ != nullptr) {
        fclose(fp_);
        fp_ = nullptr;
    }
}

void DialViewGroup::SetDial(std::string filePath, DisplayState state)
{
    if (resPreloaded_) {
        ClearDial();
        resPreloaded_ = false;
    }
    filePath_ = filePath;
    state_ = state;
}

void DialViewGroup::PreLoad(void)
{
    if (!resPreloaded_) {
        fp_ = fopen(filePath_.c_str(), "rb");
        if (fp_ == nullptr) {
            GRAPHIC_LOGE("cannot open file: %s\n", filePath_.c_str());
            return;
        }
        DialBinParserManager::GetInstance()->UpdateDial(filePath_, fp_, this, state_);
        resPreloaded_ = true;
    }
}

void DialViewGroup::ProcessData(DialDataType type, DialView* view)
{
    int16_t value = static_cast<int16_t>(type) >> 13; // 13: bit shift
    if (value == 0) { // 0: float
        float floatData = 0;
        ModelDialDataFactory::GetInstance().GetDialFloatData(type, floatData);
        view->HandleFloatData(floatData);
    } else if (value == 1) { // 1: text
        std::string* textData = nullptr;
        int16_t num = 0;
        if (!ModelDialDataFactory::GetInstance().GetDialTextData(type, textData, num)) {
            std::string tmp = "";
            view->HandleTextData(&tmp, 1);
            return;
        }
        view->HandleTextData(textData, num);
    } else if (value == 2) { // 2: multifloat
        float* floatsData = nullptr;
        int16_t num = 0;
        if (!ModelDialDataFactory::GetInstance().GetDialFloatData(type, floatsData, num)) {
            float tmp = 0.0;
            view->HandleFloatData(&tmp, 1);
            return;
        }
        view->HandleFloatData(floatsData, num);
    } else {
        GRAPHIC_LOGW("invalid datatype %d", static_cast<int>(type));
    }
}
void DialViewGroup::DoStop(void)
{
    SetLableAnimatorState(Animator::PAUSE);
    SetImgAnimatorState(Animator::PAUSE);
    if (video_ != nullptr && !video_->IsStopped()) {
        video_->StopPlay();
    }
    if (kaleidoscope_ != nullptr) {
        kaleidoscope_->StopRotateAnimator();
        kaleidoscope_->ClearFocus();
    }
}

void DialViewGroup::DoStart(void)
{
    SetLableAnimatorState(Animator::START);
    if (isSeqImgLoaded_) {
        SetImgAnimatorState(Animator::START);
    }
    if (video_ != nullptr) {
        if (video_->IsPaused()) {
            video_->ResumePlay();
        } else if (!video_->IsPlayed()) {
            video_->StartPlay();
        }
    }
    if (kaleidoscope_ != nullptr) {
        kaleidoscope_->RequestFocus();
        kaleidoscope_->StartRotateAnimator();
    }
}

void DialViewGroup::DoPause(void)
{
    SetLableAnimatorState(Animator::PAUSE);
    SetImgAnimatorState(Animator::PAUSE);
    if (kaleidoscope_ != nullptr) {
        kaleidoscope_->StopRotateAnimator();
        kaleidoscope_->ClearFocus();
    }
    if (video_ != nullptr) {
        video_->PausePlay();
    }
}

void DialViewGroup::RecordSpecial(UIView *view)
{
    if (dynamic_cast<UILabel*>(view) != nullptr) {
        UILabel* label = dynamic_cast<UILabel*>(view);
        uint8_t mode = label->GetLineBreakMode();
        if (mode == UILabel::LINE_BREAK_MARQUEE || mode == UILabel::LINE_BREAK_OSCILLATION) {
            rollingLabelList_.PushBack(label);
        }
    } else if (dynamic_cast<UIImageAnimatorView*>(view) != nullptr) {
        UIImageAnimatorView* imgAnim = dynamic_cast<UIImageAnimatorView*>(view);
        imgAnimList_.PushBack(imgAnim);
    } else if (dynamic_cast<UIKaleidoscopeView*>(view) != nullptr) {
        kaleidoscope_ = dynamic_cast<UIKaleidoscopeView*>(view);
    } else if (dynamic_cast<UILiteSurfaceView*>(view) != nullptr) {
        if (video_ != nullptr) {
            GRAPHIC_LOGE("Already added a video view.\n");
            return;
        }
        video_ = reinterpret_cast<DialVideoView*>(view);
    }
}

void DialViewGroup::RecordBindData(DialDataType type, DialView* view, bool isPeriodic)
{
    if (dynamic_cast<UILabel*>(view) != nullptr) {
        UILabel* label = dynamic_cast<UILabel*>(view);
        uint8_t mode = label->GetLineBreakMode();
        if (mode == UILabel::LINE_BREAK_MARQUEE || mode == UILabel::LINE_BREAK_OSCILLATION) {
            rollingLabelList_.PushBack(label);
        }
    }

    auto it = dataTypeMap_.find(static_cast<int16_t>(type));
    if (it != dataTypeMap_.end()) {
        it->second->PushBack(view);
    } else {
        Views *container = new Views;
        if (container == nullptr) {
            GRAPHIC_LOGE("failed to alloc binddata container");
            return;
        }
        container->PushBack(view);
        dataTypeMap_.insert(std::make_pair(static_cast<int16_t>(type), container));
        viewsMemPool_.PushBack(container);
    }
    view->SetDataType(type);
    if (isPeriodic) {
        periodicUpdateTypes_.PushBack(type);
    } else {
        ModelDialDataFactory::GetInstance().RegisterDialDataListener(type, this);
        passiveUpdateTypes_.PushBack(type);
    }
}

void DialViewGroup::ClearViews()
{
    dataTypeMap_.clear();
    for (uint16_t i = 0; i < viewsMemPool_.Size(); ++i) {
        delete viewsMemPool_[i];
    }
    viewsMemPool_.Clear();
}

void DialViewGroup::AddView(UIView* view)
{
    if (view == nullptr) {
        GRAPHIC_LOGE("Parameter is nullptr.\n");
        return;
    }
    dialViews_.PushBack(view);
    Add(view);
    DialViewContainer *container = dynamic_cast<DialViewContainer*>(view);
    if (container != nullptr) {
        ListNode<DialViewContainer::SubView> *head = container->Begin();
        for (;head != container->End(); head = head->next_) {
            DialViewContainer::SubView &subView = head->data_;
            if (subView.dialView != nullptr) {
                RecordBindData(subView.type, subView.dialView, subView.isPeriodic);
                ProcessData(subView.type, subView.dialView);
            } else if (subView.view != nullptr) {
                RecordSpecial(subView.view);
            }
        }
    } else {
        RecordSpecial(view);
    }
}

void DialViewGroup::AddView(DialDataType type, DialView* view, bool isPeriodic)
{
    if (view == nullptr) {
        GRAPHIC_LOGE("Parameter is nullptr.\n");
        return;
    }

    DialDigitalImgView *digital = dynamic_cast<DialDigitalImgView*>(view);
    if (digital != nullptr) {
        digital->SetParent(this);
    }
    RecordBindData(type, view, isPeriodic);
    dialViews_.PushBack(dynamic_cast<UIView*>(view));
    Add(dynamic_cast<UIView*>(view));
    ProcessData(type, view);
}

void DialViewGroup::SetPeriod(uint32_t period)
{
    if (period_ < DEFAULT_TASK_PERIOD) {
        period_ = DEFAULT_TASK_PERIOD;
    } else {
        period_ = period;
    }
}

uint32_t DialViewGroup::GetPeriod()
{
    return period_;
}

void DialViewGroup::UpdateViewsByPeriodicUpdateData()
{
    ListNode<DialDataType>* node = periodicUpdateTypes_.Head();
    while (node != periodicUpdateTypes_.End()) {
        DialDataType type = node->data_;
        auto iter = dataTypeMap_.find(static_cast<int16_t>(type));
        if (iter != dataTypeMap_.end()) {
            Views *container = iter->second;
            for (uint16_t i = 0; i < container->Size(); ++i) {
                ProcessData(type, (*container)[i]);
            }
        }
        node = node->next_;
    }
    if (LowPowerManager::GetInstance()->IsLiteSurfaceInRootView()) {
        Invalidate();
    }
}

void DialViewGroup::OnDialFloatDataUpdate(DialDataType type, float data)
{
    auto iter = dataTypeMap_.find(static_cast<int16_t>(type));
    if (iter != dataTypeMap_.end()) {
        Views *container = iter->second;
        for (uint16_t i = 0; i < container->Size(); ++i) {
            DialView* view = (*container)[i];
            view->HandleFloatData(data);
        }
    }
}

void DialViewGroup::OnDialFloatDataUpdate(DialDataType type, const float* data, uint16_t num)
{
    auto iter = dataTypeMap_.find(static_cast<int16_t>(type));
    if (iter != dataTypeMap_.end()) {
        Views *container = iter->second;
        for (uint16_t i = 0; i < container->Size(); ++i) {
            DialView* view = (*container)[i];
            view->HandleFloatData(data, num);
        }
    }
}

void DialViewGroup::OnDialTextDataUpdate(DialDataType type, const std::string* data, uint16_t num)
{
    auto iter = dataTypeMap_.find(static_cast<int16_t>(type));
    if (iter != dataTypeMap_.end()) {
        Views *container = iter->second;
        for (uint16_t i = 0; i < container->Size(); ++i) {
            DialView* view = (*container)[i];
            view->HandleTextData(data, num);
        }
    }
}

void DialViewGroup::AddDialClick(Rect& rect, const char* bundleName)
{
    if (bundleName == nullptr) {
        return;
    }
    uint32_t len = static_cast<uint32_t>(strlen(bundleName));
    char* name = static_cast<char*>(UIMalloc(++len));
    if (name == nullptr) {
        return;
    }
    if (strcpy_s(name, len, bundleName) != EOK) {
        UIFree(name);
        name = nullptr;
        return;
    }
    DialClick click = {rect, false, {.bundleName = name}};
    dialClicks_.PushBack(click);
}

void DialViewGroup::AddDialClick(Rect& rect, uint32_t sliceId)
{
    if (sliceId == 0) {
        return;
    }

    DialClick click = {rect, true, {.sliceId = sliceId}};
    dialClicks_.PushBack(click);
}

bool DialViewGroup::OnClickEvent(const ClickEvent& event)
{
    Point point = event.GetCurrentPos();
    ListNode<DialClick>* node = dialClicks_.Head();
    while (node != dialClicks_.End()) {
        DialClick dialClick = node->data_;
        if (dialClick.area.IsContains(point)) {
            if (dialClick.isSliceType) {
                NativeAbility::GetInstance().ChangeSlice(dialClick.sliceId);
            } else {
#ifdef JS_ENABLE
                StartJsApp(dialClick.bundleName);
#endif
            }
            return true;
        }
        node = node->next_;
    }
    return false;
}

void DialViewGroup::ScrollBegin(bool isActive)
{
    if (isActive) {
        DoPause();
    }
}

void DialViewGroup::ScrollEnd(bool isActive)
{
    MainPresenterSample* mainPresenter = MainPresenterSample::GetInstance();
    if ((mainPresenter == nullptr) || (mainPresenter->GetMainPresenterState() != MainPresenterState::RESUME)) {
        return;
    }

    if (isActive) {
        DoStart();
    } else {
        if (video_ != nullptr && !video_->IsStopped()) {
            video_->StopPlay();
        }
        if (video_ != nullptr) {
            ClearCache();
        }
    }
}

void DialViewGroup::CoverBegin(bool isCovered)
{
    if (!isCovered) {
        DoPause();
    }
}

void DialViewGroup::CoverEnd(bool isCovered)
{
    MainPresenterSample* mainPresenter = MainPresenterSample::GetInstance();
    if ((mainPresenter == nullptr) || (mainPresenter->GetMainPresenterState() != MainPresenterState::RESUME)) {
        return;
    }

    if (!isCovered) {
        DoStart();
    }
}

void DialViewGroup::OnActive(void)
{
    if (state_ == DisplayState::NORMAL) {
        MainPresenterSample* mainPresenter = MainPresenterSample::GetInstance();
        if ((mainPresenter == nullptr) || (mainPresenter->GetMainPresenterState() != MainPresenterState::RESUME)) {
            return;
        }
    }

    if (!isSeqImgLoaded_) {
        std::weak_ptr<bool> wk = isExists_;
        GraphicService::GetInstance()->ForceRefreshImmediately();
        GraphicService::GetInstance()->PostGraphicEvent([this, wk]() {
            if (wk.expired()) {
                return;
            }

            InputDeviceManager::GetInstance()->ClearInputDeviceState();
            ListNode<UIImageAnimatorView *> *node = imgAnimList_.Begin();
            while (node != imgAnimList_.End()) {
                if (node->data_ != nullptr) {
                    DialSequenceImgView *imgAnim = dynamic_cast<DialSequenceImgView *>(node->data_);
                    if (imgAnim != nullptr) {
                        imgAnim->LoadCurSeqImgs();
                        imgAnim->Start();
                    }
                }
                node = node->next_;
            }
            isSeqImgLoaded_ = true;
        });
    }

    DoStart();
}

void DialViewGroup::OnInactive(void)
{
    DoStop();
}

void DialViewGroup::OnCovered(void)
{
    MainPresenterSample* mainPresenter = MainPresenterSample::GetInstance();
    if ((mainPresenter == nullptr) || (mainPresenter->GetMainPresenterState() != MainPresenterState::RESUME)) {
        return;
    }

    DoPause();
}

void DialViewGroup::OnUncovered(void)
{
    MainPresenterSample* mainPresenter = MainPresenterSample::GetInstance();
    if ((mainPresenter == nullptr) || (mainPresenter->GetMainPresenterState() != MainPresenterState::RESUME)) {
        return;
    }

    DoStart();
}

void DialViewGroup::OnPause()
{
    DoPause();
}

MediaVideoPlay* DialViewGroup::GetVideoPlay()
{
    if (video_ != nullptr) {
        return video_->GetVideoPlay();
    }
    return nullptr;
}

void DialViewGroup::SetLableAnimatorState(uint8_t state)
{
    if (rollingLabelList_.IsEmpty()) {
        return;
    }

    ListNode<UILabel*>* node = rollingLabelList_.Begin();
    while (node != rollingLabelList_.End()) {
        if (node->data_ != nullptr) {
            UILabel* label = node->data_;
            label->SetAnimState(state);
        }
        node = node->next_;
    }
}

void DialViewGroup::SetImgAnimatorState(uint8_t state)
{
    if (imgAnimList_.IsEmpty()) {
        return;
    }

    ListNode<UIImageAnimatorView*>* node = imgAnimList_.Begin();
    while (node != imgAnimList_.End()) {
        if (node->data_ != nullptr) {
            UIImageAnimatorView* imgAnim = node->data_;
            switch (state) {
                case Animator::PAUSE:
                    imgAnim->Pause();
                    break;
                case Animator::START:
                    imgAnim->Resume();
                    break;
            }
        }
        node = node->next_;
    }
}
} // namespace OHOS
