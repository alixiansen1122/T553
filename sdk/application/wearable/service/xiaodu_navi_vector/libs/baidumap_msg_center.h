/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef BAIDU_MAP_MSG_CENTER_H
#define BAIDU_MAP_MSG_CENTER_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * 请求接口的 type_id,目前只有svg的请求
 */
typedef enum duer_msg_center_type_id {
    DUER_MSG_TYPE_ID_REQ_SVG             = 1,
    DUER_MSG_TYPE_ID_MAX
} duer_msg_center_type_id_t;

struct Content
{
    int x;
    int y;
    int z;
    void *data;
    char *json;
};


/**
 * 接收msg_center返回的数据,填充对应的content
 */
uint8_t duer_msg_center_recv_map_info(uint8_t command_id, uint8_t type, struct Content *content);

/**
 * 通过【duer_msg_center_type_id】来区分对应请求的是msg_center接口，方便以后扩展
 */
uint8_t duer_msg_center_send_map_info(uint8_t duer_type, void *usr_data, uint16_t usr_len);

/**
 * 接收APP下发的缓存第一次批量请求
 */
uint8_t duer_msg_center_map_to_title(void *data, uint16_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* BAIDU_MAP_MSG_CENTER_H */
