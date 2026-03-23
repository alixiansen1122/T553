/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_INPUT_DEVICE_MANAGER_H
#define GRAPHIC_LITE_INPUT_DEVICE_MANAGER_H

#include "dock/input_device.h"
#include "gfx_utils/list.h"
#include "common/task.h"

namespace OHOS {
/**
 * @brief Manage all input devices.
 */
class InputDeviceManager : public Task {
public:
enum class InputDeviceType {
    /** All registered input device*/
    ALL = 0,
    /** Point input device */
    POINT = 1,
    /** Key input device */
    KEY = 2,
    /** Rotate input device */
    ROTATE = 3,
};

    /**
     * @brief Get instance of InputDeviceManager.
     * @returns Instance of InputDeviceManager
     */
    static InputDeviceManager* GetInstance();

    void Init() override;

    /**
     * @brief Add an input device.
     *
     * @param [in] device Specific input device
     */
    void Add(InputDevice* device);

    /**
     * @brief Remove an input device.
     *
     * @param [in] Device Specific device to remove
     */
    void Remove(InputDevice* device);

    /**
     * Clear all display devices.
     */
    void Clear();

    /**
     * Clear state of input devices.
     */
    void ClearInputDeviceState(InputDeviceManager::InputDeviceType type = InputDeviceManager::InputDeviceType::ALL);

    /**
     * Clear pressable views recorded in the pointer input device
     */
    void ClearPressableViews();

    void Callback() override;

    /**
     * Is there a input event exist.
     */
    bool IsEventExist();

private:
    InputDeviceManager() {}
    ~InputDeviceManager() {}

    InputDeviceManager(const InputDeviceManager&) = delete;
    InputDeviceManager& operator=(const InputDeviceManager&) = delete;
    InputDeviceManager(InputDeviceManager&&) = delete;
    InputDeviceManager& operator=(InputDeviceManager&&) = delete;

    /**
     * Clear rotate state.
     */
    void ClearRotateState();

    List<InputDevice*> deviceList_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_INPUT_DEVICE_MANAGER_H
