/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_input_device.h"
#if LV_USE_LOW_POWER_MANAGER
#include "lv_low_power_manager.h"
#endif
#include "input_manager.h"
#include "stdlib.h"

static lv_indev_data_t g_curTouchData = {0};
static bool g_isPendingTouchData = false;
static lv_indev_data_t g_curKeyData = {0};
static bool g_isPendingKeyData = false;
static lv_indev_data_t g_curRotateData = {0};
static bool g_isPendingRotateData = false;
static uint8_t g_inputDevNums;
static InputDevType* g_inputDevs = NULL;
static InputFuncs* g_inputFuncs = NULL;

static bool LvIsDevAvailable(InputDevType type)
{
    for (uint8_t i = 0; i < g_inputDevNums; i++) {
        if (g_inputDevs[i] == type) {
            return true;
        }
    }
    return false;
}

static bool LvConvert2LVState(InputDevEventState state, lv_indev_state_t* lvState)
{
    switch (state) {
        case INDEV_STATE_RELEASE:
            *lvState = LV_INDEV_STATE_RELEASED;
            return true;
        case INDEV_STATE_PRESS:
            *lvState = LV_INDEV_STATE_PRESSED;
            return true;
        default:
            return false;
    }
}

static void LvTouchEventCallback(InputDevData* data)
{
    if ((data == NULL) || (data->type != INDEV_TYPE_TOUCH)) {
        return;
    }
    if (!LvConvert2LVState(data->state, &g_curTouchData.state)) {
        return;
    }
    g_curTouchData.point.x = data->x;
    g_curTouchData.point.y = data->y;
    g_isPendingTouchData = true;
#if LV_USE_LOW_POWER_MANAGER
    ExitLowPower();
#endif
}

bool LvSetUpInputDevices(void)
{
    if (InitInputManager(&g_inputFuncs) != INPUT_SUCCESS) {
        return false;
    }
    if ((g_inputFuncs->ScanInputDevice == NULL) || (g_inputFuncs->RegisterEventCallback == NULL) ||
        (g_inputFuncs->UnregisterReportCallback == NULL) || (g_inputFuncs->OpenInputDevice == NULL) ||
        (g_inputFuncs->CloseInputDevice == NULL)) {
        DeinitInputManager(g_inputFuncs);
        g_inputFuncs = NULL;
        LV_LOGE("input funcs is NULL.\n");
        return false;
    }

    g_inputDevs = g_inputFuncs->ScanInputDevice(&g_inputDevNums);
    return true;
}

bool LvOpenTouchDev(void)
{
    if (!LvIsDevAvailable(INDEV_TYPE_TOUCH)) {
        return false;
    }
    if (g_inputFuncs->RegisterEventCallback(INDEV_TYPE_TOUCH, LvTouchEventCallback) != INPUT_SUCCESS) {
        return false;
    }
    if (g_inputFuncs->OpenInputDevice(INDEV_TYPE_TOUCH) != INPUT_SUCCESS) {
        g_inputFuncs->UnregisterReportCallback(INDEV_TYPE_TOUCH);
        return false;
    }
    return true;
}

void LvTouchDevRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data)
{
    (void)indevDrv;
    if (!g_isPendingTouchData) {
        data->state = g_curTouchData.state;
        return;
    }
    data->point.x = g_curTouchData.point.x;
    data->point.y = g_curTouchData.point.y;
    data->state = g_curTouchData.state;
    g_isPendingTouchData = false;
}

static void LvKeyEventCallback(InputDevData* data)
{
    if ((data == NULL) || (data->type != INDEV_TYPE_KEY)) {
        return;
    }
    if (!LvConvert2LVState(data->state, &g_curKeyData.state)) {
        return;
    }
    g_curKeyData.key = data->keyId;
    g_isPendingKeyData = true;
#if LV_USE_LOW_POWER_MANAGER
    ExitLowPower();
#endif
}

bool LvOpenKeyDev(void)
{
    if (!LvIsDevAvailable(INDEV_TYPE_KEY)) {
        return false;
    }
    if (g_inputFuncs->RegisterEventCallback(INDEV_TYPE_KEY, LvKeyEventCallback) != INPUT_SUCCESS) {
        return false;
    }
    if (g_inputFuncs->OpenInputDevice(INDEV_TYPE_KEY) != INPUT_SUCCESS) {
        g_inputFuncs->UnregisterReportCallback(INDEV_TYPE_KEY);
        return false;
    }
    return true;
}

void LvKeyDevRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data)
{
    (void) indevDrv;
    if (!g_isPendingKeyData) {
        return;
    }
    data->key = g_curKeyData.key;
    data->state = g_curKeyData.state;
    g_isPendingKeyData = false;
}

static void LvRotateEventCallback(InputDevData* data)
{
    if ((data == NULL) || (data->type != INDEV_TYPE_CROWN)) {
        return;
    }
    g_curRotateData.enc_diff = data->rotate;
    g_isPendingRotateData = true;
#if LV_USE_LOW_POWER_MANAGER
    ExitLowPower();
#endif
}

bool LvOpenRotateDev(void)
{
    if (!LvIsDevAvailable(INDEV_TYPE_CROWN)) {
        return false;
    }
    if (g_inputFuncs->RegisterEventCallback(INDEV_TYPE_CROWN, LvRotateEventCallback) != INPUT_SUCCESS) {
        return false;
    }
    if (g_inputFuncs->OpenInputDevice(INDEV_TYPE_CROWN) != INPUT_SUCCESS) {
        g_inputFuncs->UnregisterReportCallback(INDEV_TYPE_CROWN);
        return false;
    }
    return true;
}

void LvRotateDevRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data)
{
    (void) indevDrv;
    if (!g_isPendingRotateData) {
        return;
    }
    data->enc_diff = g_curRotateData.enc_diff;
    g_isPendingRotateData = false;
}

bool LvIsTouchEventEmpty(void)
{
    if (g_isPendingTouchData) {
        return false;
    }
    if (g_curTouchData.state != LV_INDEV_STATE_RELEASED) {
        return false;
    }
    return true;
}

bool LvIsKeyEventEmpty(void)
{
    if (g_isPendingKeyData) {
        return false;
    }
    if (g_curKeyData.state != LV_INDEV_STATE_RELEASED) {
        return false;
    }
    return true;
}

bool LvIsRotateEventEmpty(void)
{
    if (g_isPendingRotateData) {
        return false;
    }
    return true;
}
