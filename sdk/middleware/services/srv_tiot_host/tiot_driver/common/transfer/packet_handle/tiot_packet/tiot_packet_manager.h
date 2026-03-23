/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: TIoT Packet Manager. \n
 *
 * History: \n
 * 2024-01-13, Create file. \n
 */
#ifndef TIOT_PACKET_MANAGER_H
#define TIOT_PACKET_MANAGER_H

#include "tiot_packet_ops.h"
#include "tiot_xfer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_transfer_packet_handle_tiot_packet_manager Packet Manager
 * @ingroup  tiot_common_transfer_packet_handle_tiot_packet
 * @{
 */

#define TIOT_PACKET_QUEUE_RX_WAITING    0x1

typedef struct {
    tiot_circ_queue_t queue;
    const tiot_packet *queue_nodes;
    uint16_t queue_size;
    uint8_t flags;
    tiot_packet active_node;
    osal_wait rx_wait;
} tiot_packet_queue;

typedef uint16_t (* tiot_packet_match_func)(uint32_t msg_type);

typedef struct {
    const tiot_packet_queue *queues;
    uint16_t queue_num;
    tiot_packet_match_func match;
} tiot_packet_manager;

typedef struct {
    tiot_xfer_manager *xfer;
    tiot_packet_queue *queue;
} tiot_packet_manager_query;

/**
 * @brief  Get packet pointer for queue.
 * @param  queue (Non NULL) The packet rx queue.
 * @return Return packet pointer for queue，if malloc fail, return NULL.
 */
tiot_packet *tiot_packet_queue_get_packet(tiot_packet_queue *queue);

/**
 * @brief  init packet manager.
 * @param  manager Packet manager.
 */
int32_t tiot_packet_manager_init(tiot_packet_manager *manager);

/**
 * @brief  deinit packet manager.
 * @param  manager Packet manager.
 */
void tiot_packet_manager_deinit(tiot_packet_manager *manager);

/* Caller entures that manager is not NULL. */
static inline tiot_packet_queue *tiot_packet_manager_find_queue(tiot_packet_manager *manager, uint16_t queue_id)
{
    if (queue_id >= manager->queue_num) {
        return NULL;
    }
    return (tiot_packet_queue *)&manager->queues[queue_id];
}

static inline int32_t tiot_packet_queue_enqueue(tiot_packet_queue *queue)
{
    return tiot_circ_queue_enqueue(&queue->queue, (uintptr_t)&queue->active_node);
}

static inline int32_t tiot_packet_queue_has_data(tiot_packet_queue *queue)
{
    return (tiot_circ_queue_empty(&queue->queue) == 0);
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif