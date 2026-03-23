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

/**
 * @addtogroup Graphic_Service
 * @{
 *
 * @brief Defines graphic service api for customer.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_GRAPHIC_SERVICE_WRAPPER_H
#define OHOS_GRAPHIC_SERVICE_WRAPPER_H
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GraphicEventCb)(void*);

/**
 *  @brief Get current frequence.
 */
int GetFrequence(void);

/**
 * @brief Set frequence for graphic, cannot use PostGraphicEvent() to excute this method.
 */
void SetFrequence(int freq);

/**
 * @brief Get native running state.
 */
int IsNativeRunning(void);

/**
 * @brief Set frequence for graphic inner.
 */
void SetFrequenceInner(int freq);

/**
 * @brief Notify the Screen is on, for c language.
 */
void NotifyScreenOn(void);

/**
 * @brief Notify the Screen is off, for c language.
 */
void NotifyScreenOff(void);

/**
 * @brief start up graphic service, for c language.
 */
void InitGraphicService(void);

/**
 * @brief show graphic logo, for c language.
 */
void ShowGraphicLogo(void);

/**
 * @brief hide graphic logo, for c language.
 */
void HideGraphicLogo(void);

void PostGraphicEvent(GraphicEventCb eventCb, void* userData);

/**
 * @brief GPU Reset start.
 */
void GpuResetStart(void);

/**
 * @brief GPU Reset end.
 */
void GpuResetEnd(void);

#ifdef __cplusplus
};
#endif
#endif
/**
 * @}
 */
