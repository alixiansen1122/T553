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

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INDEV_TYPE_TOUCH,       /**< Touchscreen */
    INDEV_TYPE_KEY,         /**< Physical key */
    INDEV_TYPE_KEYBOARD,    /**< Keyboard */
    INDEV_TYPE_MOUSE,       /**< Mouse */
    INDEV_TYPE_BUTTON,      /**< Virtual button */
    INDEV_TYPE_CROWN,       /**< Watch crown */
    INDEV_TYPE_ENCODER,     /**< Customized type of a specific function or event */
    INDEV_TYPE_UNKNOWN,     /**< Unknown input device type */
} InputDevType;

typedef enum {
    INDEV_STATE_RELEASE,
    INDEV_STATE_PRESS,
    INDEV_STATE_LONG_PRESS,
    INDEV_STATE_UNKNOWN,
} InputDevEventState;

typedef enum {
    INDEV_KEY_POWER,
    INDEV_KEY_FUNC,
    INDEV_KEY_UNKNOWN,
} InputDevKeyId;

typedef struct {
    InputDevType type;
    union {
        struct {
            int16_t x;
            int16_t y;
        };
        uint16_t keyId;
        uint16_t buttonId;
        int16_t rotate;
        int16_t encoderDiff;
    };
    uint16_t state;
    uint32_t timestamp;
} InputDevData;

#define input_dev_data_t InputDevData
typedef void (*ReportEventCb)(InputDevData* data);
#define report_event_cb ReportEventCb

/**
 * @brief Enumerates return values.
 */
typedef enum {
    INPUT_SUCCESS        = 0,     /**< Success */
    INPUT_FAILURE        = -1,    /**< Failure */
    INPUT_INVALID_PARAM  = -2,    /**< Invalid parameter */
    INPUT_NOMEM          = -3,    /**< Insufficient memory */
    INPUT_NULL_PTR       = -4,    /**< Null pointer */
    INPUT_TIMEOUT        = -5,    /**< Execution timed out */
    INPUT_UNSUPPORTED    = -6,    /**< Unsupported feature */
} RetStatus;

typedef struct {
    /**
     * @brief Obtain types of opened input devices.
     * @param size Indicates the number of types of opened input devices(out parameter).
     * @return return a pointer to types of opened input devices.
     */
    InputDevType* (*ScanInputDevice)(uint8_t* size);

    /**
     * @brief Register an event callback function to an input devive.
     * @param type refers to InputDevType.
     * @param func refers to ReportEventCb.
     * @return return INPUT_SUCCESS if register the callback successfully; return error code otherwise.
     */
    int32_t (*RegisterEventCallback)(InputDevType type, ReportEventCb func);

    /**
     * @brief Unregister an event callback function to an input devive.
     * @param type refers to InputDevType.
     * @return return INPUT_SUCCESS if unregister the callback successfully; return error code otherwise.
     */
    int32_t (*UnregisterReportCallback)(InputDevType type);

    /**
     * @brief Open an input device.
     * @param type refers to InputDevType.
     * @return return INPUT_SUCCESS if opens the input device successfully; return error code otherwise.
     */
    int32_t (*OpenInputDevice)(InputDevType type);

    /**
     * @brief Close an input device.
     * @param type refers to InputDevType.
     * @return return INPUT_SUCCESS if closes the input device successfully; return error code otherwise.
     */
    int32_t (*CloseInputDevice)(InputDevType type);
} InputFuncs;

/**
 * @brief Init input manager to obtain th pointer to related functions.
 */
int32_t InitInputManager(InputFuncs **funcs);

/**
 * @brief Init input managerto release the pointer to related functions.
 */
int32_t DeinitInputManager(InputFuncs *funcs);

#ifdef __cplusplus
}
#endif
#endif