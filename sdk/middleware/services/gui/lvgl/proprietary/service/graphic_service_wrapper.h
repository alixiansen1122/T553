/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef GRAPHIC_SERVICE_WRAPPER_H
#define GRAPHIC_SERVICE_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GraphicEventCb)(void*);

/**
 * @brief Set frequence for graphic, which is the max value during refresh
 */
void SetFrequence(uint32_t freq);

/**
 * @brief Notify the Screen is on, for c language.
 */
void NotifyScreenOn(void);

/**
 * @brief Notify the Screen is off, for c language.
 */
void NotifyScreenOff(void);

/**
 * @brief start up graphic main loop, for c language.
 */
void GraphicStartMain(void);
/**
 * @brief GPU Reset start.
 */
void GpuResetStart(void);

/**
 * @brief GPU Reset end.
 */
void GpuResetEnd(void);

/**
 * @brief Post graphic event.
 * @param eventCb a function to execute once in the event loop.
 * @param userData the pointer to the custom user data.
 * @param exitLowPower whether to exit low power mode. if the graphic event includes ui updates, it should be true.
 */
void PostGraphicEvent(GraphicEventCb eventCb, void* userData, bool exitLowPower);

#ifdef __cplusplus
};
#endif
#endif
/**
 * @}
 */
