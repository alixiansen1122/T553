/**
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved. \n
 *
 * Description: Provides gsensor queue driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-05-20, Create file. \n
 */
#ifndef GSENSOR_QUEUE_H
#define GSENSOR_QUEUE_H

#include "errcode.h"
#include "stdbool.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define QUEUE_SIZE 128

typedef struct {
    int16_t accel_raw[3];
    float accel_proc[3];
}gsensor_data_t;

typedef struct {
    gsensor_data_t data[QUEUE_SIZE];
    int front;
    int rear;
    int size;
    int capacity;
}gsensor_queue_t;

/**
 * @brief  Queue mutex init.
 * @return 0 means success, otherwise check reason.
 */
errcode_t queue_mutex_init(void);

/**
 * @brief  Queue mutex deinit.
 */
void queue_mutex_deinit(void);

/**
 * @brief  Queue init.
 */
void queue_init(void);

/**
 * @brief  Check queue is empty.
 * @return 1 means empty, otherwise not empty.
 */
bool queue_is_empty(void);

/**
 * @brief  Check queue is full.
 * @return 1 means full, otherwise not full.
 */
bool queue_is_full(void);

/**
 * @brief  Insert data into the queue.
 * @param  Gsensordata.
 */
void queue_enqueue(gsensor_data_t data);

/**
 * @brief  Get data from the queue.
 * @param  Gsensordata pointer.
 * @return 0 means success, otherwise fail.
 */
errcode_t queue_dequeue(gsensor_data_t *data);

/**
 * @brief  Get the size of queue.
 * @return queue size.
 */
uint32_t queue_size_get(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif