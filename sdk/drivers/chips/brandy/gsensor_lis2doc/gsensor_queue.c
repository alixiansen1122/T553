/**
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved. \n
 *
 * Description: Provides gsensor queue driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-05-20, Create file. \n
 */
#include "osal_mutex.h"
#include "gsensor_queue.h"

static osal_mutex g_queue_mutex;
static gsensor_queue_t g_queue;

errcode_t queue_mutex_init(void)
{
    return osal_mutex_init(&g_queue_mutex);
}

void queue_mutex_deinit(void)
{
    osal_mutex_destroy(&g_queue_mutex);
}

void queue_init(void)
{
    osal_mutex_lock(&g_queue_mutex);
    g_queue.front = 0;
    g_queue.rear = 0;
    g_queue.size = 0;
    g_queue.capacity = QUEUE_SIZE;
    memset_s(g_queue.data, sizeof(g_queue.data), 0, sizeof(g_queue.data));
    osal_mutex_unlock(&g_queue_mutex);
}

bool queue_is_empty(void)
{
    return (g_queue.size == 0) ? true : false;
}

bool queue_is_full(void)
{
    return (g_queue.size == g_queue.capacity) ? true : false;
}

void queue_enqueue(gsensor_data_t data)
{
    osal_mutex_lock(&g_queue_mutex);
    if (queue_is_full()) {
        gsensor_data_t temp;
        queue_dequeue(&temp);
    }

    g_queue.data[g_queue.rear] = data;
    g_queue.rear = (g_queue.rear + 1) % QUEUE_SIZE;
    g_queue.size++;
    osal_mutex_unlock(&g_queue_mutex);
}

errcode_t queue_dequeue(gsensor_data_t *data)
{
    osal_mutex_lock(&g_queue_mutex);
    if (queue_is_empty()) {
        osal_mutex_unlock(&g_queue_mutex);
        return ERRCODE_FAIL;
    }

    *data = g_queue.data[g_queue.front];
    g_queue.front = (g_queue.front + 1) % QUEUE_SIZE;
    g_queue.size--;
    osal_mutex_unlock(&g_queue_mutex);
    return ERRCODE_SUCC;
}

uint32_t queue_size_get(void)
{
    return g_queue.size;
}
