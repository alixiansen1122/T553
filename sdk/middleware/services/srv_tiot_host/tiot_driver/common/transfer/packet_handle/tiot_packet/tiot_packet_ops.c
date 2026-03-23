/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: TIoT Packet Ops. \n
 *
 * History: \n
 * 2024-12-02, Create file. \n
 */

#include "tiot_packet_ops.h"
#include "tiot_osal.h"
#include "securec.h"

#ifdef CONFIG_BOARD_DYNAMIC_ALLOC
struct tiot_list_head *tiot_packet_head_init(void)
{
    struct tiot_list_head *head = (struct tiot_list_head *)osal_kmalloc(sizeof(struct tiot_list_head), OSAL_GFP_KERNEL);
    if (head != NULL) {
        tiot_init_list_head(head);
    }
    return head;
}

void tiot_packet_free(tiot_packet *packet)
{
    tiot_packet_rx_node *pos, *next;
    tiot_list_head_create(tmp_head);

    if (packet->head == NULL) {
        return;
    }
    tiot_list_replace(packet->head, &tmp_head);
    /* 使用临时节点遍历删除. */
    tiot_list_for_each_entry_safe(pos, next, &tmp_head, node, tiot_packet_rx_node, tiot_packet_rx_node) {
        if (pos != NULL) {
            tiot___list_del_entry(&pos->node);
            osal_kfree(pos);
        }
    }
    osal_kfree(packet->head);
}

void tiot_packet_reset(tiot_packet *packet)
{
    packet->head = NULL;
}

uint32_t tiot_packet_len(tiot_packet *packet)
{
    uint32_t len = 0;
    tiot_packet_rx_node *pos;

    if (packet->head == NULL) {
        return 0;
    }
    tiot_list_for_each_entry(pos, packet->head, node, tiot_packet_rx_node) {
        if ((pos->data == NULL) || (pos->len == 0)) {
            return 0;
        }
        len += pos->len;
    }
    return len;
}

uint8_t *tiot_packet_expand(tiot_packet *packet, uint32_t len)
{
    tiot_packet_rx_node *node = (tiot_packet_rx_node *)osal_kmalloc(sizeof(tiot_packet_rx_node) + len, OSAL_GFP_KERNEL);
    if (node == NULL) {
        return NULL;
    }

    node->data = (uint8_t *)node + sizeof(tiot_packet_rx_node);
    node->len = len;

    tiot_list_add_tail(&(node->node), packet->head);

    return node->data;
}

void tiot_packet_copy_to(tiot_packet *packet, uint8_t *buff)
{
    tiot_packet_rx_node *pos;
    tiot_list_for_each_entry(pos, packet->head, node, tiot_packet_rx_node) {
        (void)memcpy_s((void *)buff, pos->len, (void *)pos->data, pos->len);
        buff += pos->len;
    }
}
#else
void tiot_packet_reset(tiot_packet *packet)
{
    packet->len = 0;
}

uint32_t tiot_packet_len(tiot_packet *packet)
{
    return packet->len;
}

void tiot_packet_expand(tiot_packet *packet, uint32_t len)
{
    packet->len += len;
}
#endif
