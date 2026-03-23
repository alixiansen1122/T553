/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#include "input_manager.h"
#include "touch_screen_def.h"

typedef enum {
    INDEV_UNINITIALIZED,
    INDEV_INITIALIZED,
} InputDevInitState;

typedef struct {
    /* Init an input device(e.g. beginning the execution of thread to handle input event). */
    int32_t (*Init)(void);
    /* Close an input device. */
    int32_t (*Close)(void);
    /* Read InputDevData directly from the input device. */
    int32_t (*Read)(InputDevData* data);
    /* Register a report event callback function which will be called when comes an event. */
    int32_t (*RegisterReportEventCb)(ReportEventCb func);
    /* Unregister a report event callback function. */
    int32_t (*UnregisterReportEventCb)(void);
} InputDevFuncs;

typedef struct {
    InputDevType type;
    InputDevInitState initState;
    InputDevFuncs funcs;
} InputDev;

static InputDev g_inputDevs[INDEV_TYPE_UNKNOWN];

__attribute__((weak)) uint32_t button_register_report_event_cb(report_event_cb callback)
{
    (void)(callback);
    return INPUT_FAILURE;
}
__attribute__((weak)) uint32_t button_unregister_report_event_cb(void)
{
    return INPUT_FAILURE;
}
__attribute__((weak)) uint32_t button_init(void)
{
    return INPUT_FAILURE;
}
__attribute__((weak)) uint32_t button_close(void)
{
    return INPUT_FAILURE;
}

static void InitInputDevices(void)
{
    for (uint8_t i = 0; i < INDEV_TYPE_UNKNOWN; i++) {
        g_inputDevs[i].type = INDEV_TYPE_UNKNOWN;
        g_inputDevs[i].initState = INDEV_UNINITIALIZED;
    }

    /* Init Touch Device */
    g_inputDevs[INDEV_TYPE_TOUCH].type = INDEV_TYPE_TOUCH;
    g_inputDevs[INDEV_TYPE_TOUCH].funcs.Init = ts_init;
    g_inputDevs[INDEV_TYPE_TOUCH].funcs.Close = ts_close;
    g_inputDevs[INDEV_TYPE_TOUCH].funcs.RegisterReportEventCb = ts_register_report_event_cb;
    g_inputDevs[INDEV_TYPE_TOUCH].funcs.UnregisterReportEventCb = ts_unregister_report_event_cb;

    /* Init Touch Device */
    g_inputDevs[INDEV_TYPE_KEY].type = INDEV_TYPE_KEY;
    g_inputDevs[INDEV_TYPE_KEY].funcs.Init = button_init;
    g_inputDevs[INDEV_TYPE_KEY].funcs.Close = button_close;
    g_inputDevs[INDEV_TYPE_KEY].funcs.RegisterReportEventCb = button_register_report_event_cb;
    g_inputDevs[INDEV_TYPE_KEY].funcs.UnregisterReportEventCb = button_unregister_report_event_cb;
}

static InputDevType* ScanInputDevice(uint8_t* size)
{
    InputDevType* openedInputDevs = malloc(sizeof(InputDevType) * INDEV_TYPE_UNKNOWN);
    if (openedInputDevs == NULL) {
        return NULL;
    }

    InitInputDevices();

    *size = 0;
    for (uint8_t i = 0; i < INDEV_TYPE_UNKNOWN; i++) {
        if (g_inputDevs[i].type != INDEV_TYPE_UNKNOWN) {
            openedInputDevs[*size] = g_inputDevs[i].type;
            ++(*size);
        }
    }
    return openedInputDevs;
}

static int32_t CheckTypeAndInitState(InputDevType type, InputDevInitState state)
{
    if ((g_inputDevs[type].type != type) || (g_inputDevs[type].initState != state)) {
        return INPUT_FAILURE;
    }
    return INPUT_SUCCESS;
}

static int32_t RegisterEventCallback(InputDevType type, ReportEventCb func)
{
    if ((g_inputDevs[type].type != type) || (g_inputDevs[type].funcs.RegisterReportEventCb == NULL)) {
        return INPUT_FAILURE;
    }
    g_inputDevs[type].funcs.RegisterReportEventCb(func);
    return INPUT_SUCCESS;
}

static int32_t UnregisterReportCallback(InputDevType type)
{
    if ((g_inputDevs[type].type != type) || (g_inputDevs[type].funcs.UnregisterReportEventCb == NULL)) {
        return INPUT_FAILURE;
    }
    g_inputDevs[type].funcs.UnregisterReportEventCb();
    return INPUT_SUCCESS;
}

static int32_t OpenInputDevice(InputDevType type)
{
    if (CheckTypeAndInitState(type, INDEV_UNINITIALIZED) == INPUT_FAILURE) {
        return INPUT_FAILURE;
    }
    if (g_inputDevs[type].funcs.Init == NULL) {
        return INPUT_FAILURE;
    }
    g_inputDevs[type].funcs.Init();
    g_inputDevs[type].initState = INDEV_INITIALIZED;
    return INPUT_SUCCESS;
}

static int32_t CloseInputDevice(InputDevType type)
{
    if (CheckTypeAndInitState(type, INDEV_INITIALIZED) == INPUT_FAILURE) {
        return INPUT_FAILURE;
    }
    if (g_inputDevs[type].funcs.Close == NULL) {
        return INPUT_FAILURE;
    }
    g_inputDevs[type].funcs.Close();
    return INPUT_SUCCESS;
}

int32_t InitInputManager(InputFuncs **funcs)
{
    if (funcs == NULL) {
        return INPUT_NULL_PTR;
    }
    InputFuncs *iFuncs = (InputFuncs *)malloc(sizeof(InputFuncs));
    if (iFuncs == NULL) {
        return INPUT_NULL_PTR;
    }
    (void)memset_s(iFuncs, sizeof(InputFuncs), 0, sizeof(InputFuncs));
    iFuncs->ScanInputDevice = ScanInputDevice;
    iFuncs->RegisterEventCallback = RegisterEventCallback;
    iFuncs->UnregisterReportCallback = UnregisterReportCallback;
    iFuncs->OpenInputDevice = OpenInputDevice;
    iFuncs->CloseInputDevice = CloseInputDevice;
    *funcs = iFuncs;
    return INPUT_SUCCESS;
}

int32_t DeinitInputManager(InputFuncs *funcs)
{
    if (funcs == NULL) {
        return INPUT_NULL_PTR;
    }
    free(funcs);
    return INPUT_SUCCESS;
}
