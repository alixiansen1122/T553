 /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: FloatAccessView
 * Author: Hisi Graphic Team
 * Created: 2025-09
 */

#include <string>
#include <cmath>
#include "NativeRegisterManager.h"
#include "wearable_log.h"
#include "components/root_view.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"
#include "TransitionType.h"
#include "NativeAbility.h"
#include "kv_store.h"
#include "easing_equation.h"
#include "quickaccess/FloatAccessView.h"

namespace OHOS {
constexpr uint16_t ITEM_SIZE = 60;
constexpr uint16_t ITEM_PADDING = 10;
constexpr uint16_t VIEW_ID_VALUE_LEN = 4;
constexpr uint16_t EXT_OFFSET = 50;
constexpr uint16_t MAX_FILE_NAME_LENGTH = 7;

inline bool ItemIsNotNull(AppItem item)
{
    return item.id != VIEW_INVALID;
}

FloatAccessView* FloatAccessView::GetInstance(void)
{
    static FloatAccessView instance;
    return &instance;
}

void FloatAccessView::AddItem(AppItem item, uint8_t index)
{
    if (index >= MAX_ITEM_SIZE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "index value must smaller than 3");
        return;
    }
    items_[index] = item;
    SaveAppItemsData();
}

void FloatAccessView::DeleteItem(const char* label)
{
    if (label == nullptr) {
        return;
    }
    for (int i = 0; i < MAX_ITEM_SIZE; ++i) {
        if (strcmp(items_[i].label, label) == 0) {
            items_[i] = {};
            SaveAppItemsData();
            return;
        }
    }
}

AppItem FloatAccessView::GetItem(uint8_t index)
{
    if (index >= MAX_ITEM_SIZE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "index value must smaller than 3");
        return {};
    }
    return items_[index];
}

void FloatAccessView::OnItemClick(const std::string& name)
{
    uint32_t targetId = NativeAbility::GetInstance().GetCurTargetId();
    uint16_t curSlice = static_cast<uint16_t>(targetId & SLICE_MASK);
    
    for (int i = 0; i < MAX_ITEM_SIZE; ++i) {
        if (strcmp(items_[i].label, name.c_str()) == 0 && curSlice != items_[i].id) {
            // 添加点击应用标记 （进入应用时，退出悬浮框，并且不显示退出动画）
            isClickItem_ = true;
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FloatAccessView::OnClick viewid = %d", items_[i].id);
            NativeAbility::GetInstance().ChangeSlice(items_[i].id, TransitionType::TRANSITION_ZOOM);
            return;
        }
    }
}

bool FloatAccessView::OnClick(UIView& view, const ClickEvent& event)
{
    ExitView();
    return false;
}

void FloatAccessView::EnterApp()
{
    uint32_t targetId = NativeAbility::GetInstance().GetCurTargetId();
    uint16_t curSlice = static_cast<uint16_t>(targetId & SLICE_MASK);
    
    for (int i = 0; i < MAX_ITEM_SIZE; ++i) {
        // 快捷应用存在且不为当前应用，则进入应用页
        if (curSlice != (items_[i].id)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FloatAccessView::OnClick viewid = %d", items_[i].id);
            NativeAbility::GetInstance().ChangeSlice(items_[i].id, TransitionType::TRANSITION_ZOOM);
            return;
        }
    }
}

void FloatAccessView::InitView()
{
    if (!isInitView_) {
        LoadAppItemsData();
        if (GetItemSize() == 0) {
            return;
        } else if (GetItemSize() == 1) {
            EnterApp();
        } else {
            Resize(Screen::GetInstance().GetWidth() + EXT_OFFSET, Screen::GetInstance().GetHeight() + EXT_OFFSET);
            SetStyle(STYLE_BACKGROUND_OPA, 100); // opa 100
            SetDraggable(false);
            SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
            floatBoxView_ = new UIFloatBoxView();
            enterCallback_ = new FloatAnimatorCallback(floatBoxView_);
            if (floatBoxView_ == nullptr || enterCallback_  == nullptr) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FloatAccessView::InitView failed, can't alloc memory");
                return;
            }
            floatBoxView_->SetOnItemClickListener(this);
            floatBoxView_->SetOnClickListener(this);
            floatBoxView_->SetPosition(EXT_OFFSET, EXT_OFFSET,
                        Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
            floatBoxView_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
            floatBoxView_->SetItemSize(ITEM_SIZE);
            floatBoxView_->SetItemSpacing(ITEM_PADDING);
            floatBoxView_->SetDragFactor(0.4f); // 0.4 : factor
            floatBoxView_->SetAccFactor(0.5f);  // 0.5 : factor
            floatBoxView_->SetAlignAvgSpeed(5.0f); // 5.0 : factor
            AddAppItemsData();
            Add(floatBoxView_);
            isInitView_ = true;
        }
    }
}

void FloatAccessView::ShowView()
{
    if (!isInitView_) {
        InitView();
    }
    if (enterCallback_ == nullptr || enterCallback_->GetState() == Animator::START) {
        return;
    }
    isShowView_ = true;
    RootView::GetInstance()->AddSystemView(this);
    Invalidate();
    enterCallback_->DoEnter();
}

void FloatAccessView::ExitView()
{
    if (!isInitView_ || !isShowView_ || enterCallback_ == nullptr || enterCallback_->GetState() == Animator::START) {
        return;
    }
    if (isClickItem_) {
        CleanView();
        return;
    }
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    enterCallback_->DoExit();
}

void FloatAccessView::CleanView()
{
    RootView::GetInstance()->RemoveSystemView(this);
    RemoveAll();
    if (floatBoxView_ != nullptr) {
        delete floatBoxView_;
        floatBoxView_ = nullptr;
    }
    if (enterCallback_ != nullptr) {
        delete enterCallback_;
        enterCallback_ = nullptr;
    }
    isInitView_ = false;
    isShowView_ = false;
    isClickItem_ = false;
}

bool FloatAccessView::IsShowView()
{
    return isShowView_;
}

uint8_t FloatAccessView::GetItemSize()
{
    uint8_t count = 0;
    for (int i = 0; i< MAX_ITEM_SIZE; i++) {
        if (ItemIsNotNull(items_[i])) {
            ++count;
        }
    }
    return count;
}

void FloatAccessView::LoadAppItemsData()
{
    // 只加载一次
    if (isLoadData_) {
        return;
    }
    char quick[MAX_ITEM_SIZE][VIEW_ID_VALUE_LEN] = {0, 0, 0};
    // 从文件中读取快捷应用的viewId
    (void)UtilsGetValue("quick1", quick[0], VIEW_ID_VALUE_LEN); // 0: index
    (void)UtilsGetValue("quick2", quick[1], VIEW_ID_VALUE_LEN); // 1: index
    (void)UtilsGetValue("quick3", quick[2], VIEW_ID_VALUE_LEN); // 2: index

    List<AppItem>& nativeAppLists = NativeRegisterManager::GetInstance().GetNativeAppLists();
    uint8_t registerNativeAppNum = nativeAppLists.Size();
    ListNode<AppItem>* appNode = nativeAppLists.Head();
    for (int32_t index = 0; index < registerNativeAppNum; index++) {
        const char* viewIdStr = std::to_string(appNode->data_.id).c_str();
        // 根据id设置应用的信息
        for (int k = 0; k < MAX_ITEM_SIZE; k++) {
            if (strcmp(viewIdStr, quick[k]) == 0) {
                items_[k] = appNode->data_;
                break;
            }
        }
        appNode = appNode->next_;
    }
    isLoadData_ = true;
}

void FloatAccessView::AddAppItemsData()
{
    if (floatBoxView_ == nullptr) {
        return;
    }
    FloatItem items[MAX_ITEM_SIZE];
    int cnt = 0;
    for (int i = 0; i < MAX_ITEM_SIZE; i++) {
        if (ItemIsNotNull(items_[i])) {
            std::string s(items_[i].label);
            ImageInfo* image = ImageCacheManager::GetInstance().LoadSingleRes(items_[i].iconSmall);
            items[cnt++] = {s, image};
        }
    }
    floatBoxView_->AddItems(items, cnt);
}

void FloatAccessView::SaveAppItemsData()
{
    char dest[MAX_FILE_NAME_LENGTH] = "quick"; // quick1,quick2,quick3 
    dest[MAX_FILE_NAME_LENGTH - 1] = '\0';
    for (int i = 0; i<MAX_ITEM_SIZE; i++) {
        if (ItemIsNotNull(items_[i])) {
            dest[MAX_FILE_NAME_LENGTH - 2] = '1'+ i;   // 2: 变量数字在倒数第二个位置
            (void)UtilsSetValue(dest, std::to_string(items_[i].id).c_str());
        }
    }
}

/**
 * @Param overshot : 超出位置占位移的比例
 * @Param peakTime : 到达峰值的时间（0~1）
 */
double ReboundFunc(double distance, double t, double duration,
                            double overshot = 0.3, double peakTime = 0.6) {
    if (t <= 0) return 0.0;
    if (t >= duration) return distance;

    double x = t / duration;
    double peakPosition = distance * (1 + overshot);

    // 使用三次贝塞尔曲线控制点确保连续性
    if (x < peakTime) {
        // 三次曲线: 曲速到峰值
        double p = x / peakTime;
        return distance * (3 * p * p - 2 * p * p * p) + (peakPosition - distance) * p * p; // 3,2: 阶数
    } else {
        // 平滑回弹曲线: 从峰值回弹
        double r = (x - peakTime) / (1 - peakTime);
        return peakPosition - (peakPosition - distance) * (3 * r * r - 2 * r * r * r); // 3,2: 阶数
    }
}

void FloatAnimatorCallback::Callback(UIView *view)
{
    UIFloatBoxView* floatBoxView_ = dynamic_cast<UIFloatBoxView*>(view);
    int16_t pos;
    if (isExit_) {
        pos = EasingEquation::QuadEaseOut(startPos_, endPos_, animator_->GetRunTime(), animator_->GetTime());
    } else {
        // 进入时使用反弹动画
        int16_t totalTime = 100;  // 100: 表示总时间为100份
        int16_t curTime = EasingEquation::LinearEaseNone(0, totalTime, animator_->GetRunTime(), animator_->GetTime());
        int distance = endPos_ - startPos_;
        pos = startPos_ + ReboundFunc(distance, curTime, totalTime);
    }
    floatBoxView_->SetPosition(pos, pos);
    floatBoxView_->Invalidate();
}

void FloatAnimatorCallback::DoEnter()
{
    isExit_ = false;
    startPos_ = EXT_OFFSET;
    endPos_ = -10;  // -10 end pos
    animator_->Start();
}

void FloatAnimatorCallback::DoExit()
{
    isExit_ = true;
    startPos_ = 0;
    endPos_ = EXT_OFFSET;
    animator_->Start();
}

void FloatAnimatorCallback::OnStop(UIView& view)
{
    if (isExit_) {
        FloatAccessView::GetInstance()->CleanView();
    }
}
}