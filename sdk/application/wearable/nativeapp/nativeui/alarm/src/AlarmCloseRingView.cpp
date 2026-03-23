/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmCloseRingView.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include <string>
#include "ohos_timer.h"
#include "graphic_timer.h"

#include "alarm/AlarmPopUpPresenter.h"
#include "phoneservice/PhoneService.h"

namespace OHOS {
#define CLOSE "关闭"

GraphicTimer* g_stopAlarmCloseHandle = nullptr;

static AlarmCloseRingView *g_pAlarmCloseRingView = nullptr;
constexpr int16_t MAX_SET_ZERO_LEN = 0;
constexpr int16_t MAX_SET_SRC_LEN = 90;
constexpr int16_t MAX_IMAGE_X_LEN = 165;
constexpr int16_t MAX_IMAGE_Y_LEN = 150;
constexpr int16_t MAX_LABEL_X_LEN = 187;
constexpr int16_t MAX_LABEL_Y_LEN = 300;
constexpr int16_t MAX_LABEL_LENGTH_LEN = 350;
constexpr int16_t MAX_LABEL_HEIGHT_LEN = 40;
constexpr int16_t MAX_LABEL_FONT_LEN = 35;

static constexpr uint64_t CLOSE_THREE_SECONDS = 3000;
void CloseTimerCallback(void* data);
void EndCloseHandle(void);

AlarmCloseRingView::AlarmCloseRingView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmCloseRingView::AlarmCloseRingView");
    g_pAlarmCloseRingView = this;
}

AlarmCloseRingView::~AlarmCloseRingView()
{
    group->RemoveAll();
    delete imageClose;
    imageClose = nullptr;
    delete label;
    label = nullptr;
    delete group;
    group = nullptr;
    g_pAlarmCloseRingView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmCloseRingView::~AlarmCloseRingView");
}

AlarmCloseRingView *AlarmCloseRingView::GetInstance(void)
{
    return g_pAlarmCloseRingView;
}

UIViewGroup *AlarmCloseRingView::InitCloseRingView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmCloseRingView::InitCloseRingView");
    group = new UIViewGroup();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmCloseRingView::group null!");
        return nullptr;
    }
    group->SetPosition(MAX_SET_ZERO_LEN, MAX_SET_ZERO_LEN);
    group->SetWidth(HORIZONTAL_RESOLUTION);
    group->SetHeight(VERTICAL_RESOLUTION);

    imageClose = new UIImageView();
    if (imageClose == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmCloseRingView::imageClose null!");
        return nullptr;
    }
    imageClose->SetPosition(MAX_IMAGE_X_LEN, MAX_IMAGE_Y_LEN);
    imageClose->SetWidth(MAX_SET_SRC_LEN);
    imageClose->SetHeight(MAX_SET_SRC_LEN);
    imageClose->SetSrc(ALARM_CLOCK_CLOSE_ADD_PATH);
    imageClose->SetTouchable(true);
    group->Add(imageClose);

    label = new UILabel();
    if (label == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmCloseRingView::label null!");
        return nullptr;
    }
    label->SetPosition(MAX_LABEL_X_LEN, MAX_LABEL_Y_LEN, MAX_LABEL_LENGTH_LEN, MAX_LABEL_HEIGHT_LEN);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MAX_LABEL_FONT_LEN);
    label->SetText(CLOSE);
    group->Add(label);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(AlarmPopUpPresenter::GetInstance());
    group->SetOnDragListener(listener);
    group->SetDraggable(true);
    group->SetTouchable(true);
    return group;
}

void AlarmCloseRingView::CancelCloseHandle(void)
{
    if (g_stopAlarmCloseHandle != nullptr) {
		delete g_stopAlarmCloseHandle;

        g_stopAlarmCloseHandle = nullptr;
    }
}

void EndCloseHandle(void)
{
    uint16_t preSlice = AlarmClockModel::GetInstance()->JudgePreSlice();
    NativeAbility::GetInstance().ChangeSlice(preSlice);
}

static void CloseTimerCallbackProc(void* data)
{
    (void)data;
    if (AlarmCloseRingView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CloseTimerCallbackProc AlarmCloseRingView::GetInstance() nullptr!!");
        return;
    }
    AlarmCloseRingView::GetInstance()->CancelCloseHandle();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClose scan end jumpView");
    EndCloseHandle();
}

void CloseTimerCallback(void* data)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(CloseTimerCallbackProc, data));
}

void AlarmCloseRingView::AlarmCloseShowTimer(void)
{
    uint32 tick = GetOSTick(CLOSE_THREE_SECONDS);
    g_stopAlarmCloseHandle = new GraphicTimer(tick, CloseTimerCallback, nullptr, true);
    if (g_stopAlarmCloseHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmCloseShowTimer new GraphicTime failed!!");
        return;
    }
    bool retTimer = g_stopAlarmCloseHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmCloseShowTimer Start failed!!");
        if (g_stopAlarmCloseHandle != nullptr) {
            delete g_stopAlarmCloseHandle;

            g_stopAlarmCloseHandle = nullptr;
        }
        return;
    }
}
} // namespace OHOS
