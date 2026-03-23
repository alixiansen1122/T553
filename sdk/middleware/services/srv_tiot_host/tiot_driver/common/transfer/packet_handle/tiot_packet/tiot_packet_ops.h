/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: TIoT Packet Ops. \n
 *
 * History: \n
 * 2024-12-02, Create file. \n
 */
#ifndef TIOT_PACKET_OPS_H
#define TIOT_PACKET_OPS_H

#include "tiot_types.h"
#include "tiot_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_transfer_packet_handle_tiot_packet_ops Packet Ops
 * @ingroup  tiot_common_transfer_packet_handle_tiot_packet
 * @{
 */

/**
 * @brief  Tiot Complete Packet, simple replace for skb_queue.
 */
#ifdef CONFIG_BOARD_DYNAMIC_ALLOC
typedef struct {
    struct tiot_list_head *head;  /* Dynamic Allocated. */
    uint16_t tag;                 /* BINARY | ASCII. */
} tiot_packet;

typedef struct {
    struct tiot_list_head node;
    uint8_t *data;
    uint32_t len;
} tiot_packet_rx_node;
#else

typedef struct {
    uint32_t len;
    uint16_t tag;        /* BINARY | ASCII. */
} tiot_packet;
#endif

#ifdef CONFIG_BOARD_DYNAMIC_ALLOC
/**
 * @brief  init packet head.
 * @return return alloced pointer if OK else NULL.
 */
struct tiot_list_head *tiot_packet_head_init(void);

/**
 * @brief  expand packet.
 * @param  packet Pointer to tiot packet.
 * @param  len Expanded space length.
 * @return return start addr of len if OK else NULL.
 */
uint8_t *tiot_packet_expand(tiot_packet *packet, uint32_t len);

/**
 * @brief  copy packet data to buff.
 * @param  packet Pointer to tiot packet.
 * @param  buff Pointer to user buff.
 */
void tiot_packet_copy_to(tiot_packet *packet, uint8_t *buff);

/**
 * @brief  free packet.
 * @param  packet Pointer to tiot packet.
 */
void tiot_packet_free(tiot_packet *packet);
#else
/**
 * @brief  expand packet.
 * @param  packet Pointer to tiot packet.
 * @param  len Expanded space length.
 */
void tiot_packet_expand(tiot_packet *packet, uint32_t len);
#endif

/**
 * @brief  get packet len.
 * @param  packet Pointer to tiot packet.
 * @return return >0 if packet is valid else 0.
 */
uint32_t tiot_packet_len(tiot_packet *packet);

/**
 * @brief  reset packet.
 * @param  packet Pointer to tiot packet.
 */
void tiot_packet_reset(tiot_packet *packet);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif